#include "Watchdog.h"
#include <thread>

void Watchdog::start()
{
	std::thread{ &Watchdog::run, this }.detach();
}

void Watchdog::run()
{
	while (!terminate) {
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleepTime * 1000)));
		action(condition());
	}
}

//Keep Watchdog - Keep the big panel off until all panels are solved
//The condition is not entirely correct, need to figure out how to check for sure if the puzzle is solved

bool KeepWatchdog::condition() {
	if (!ready) {
		float power = ReadPanelData<float>(0x03317, POWER);
		if (!power) return false;
		ready = true;
	}
	int numTraced = ReadPanelData<int>(0x01BE9, TRACED_EDGES);
	int tracedptr = ReadPanelData<int>(0x01BE9, TRACED_EDGE_DATA);
	std::vector<SolutionPoint> traced; if (tracedptr) traced = ReadArray<SolutionPoint>(0x01BE9, TRACED_EDGE_DATA, numTraced);
	if (traced.size() < 12 || traced.size() > 26 || traced[traced.size() - 1].pointB != 25) return false;
	for (SolutionPoint p : traced) {
		if (p.pointA == 16 && p.pointB == 17 || p.pointB == 16 && p.pointA == 17) {
			return true;
		}
	}
	return false;
}

void KeepWatchdog::action(bool status) {
	if (status) {
		WritePanelData<float>(0x03317, POWER, { 1, 1 });
	}
	else {
		WritePanelData<float>(0x03317, POWER, { 0, 0 });
	}
}

//Arrow Watchdog - To run the arrow puzzles

bool ArrowWatchdog::condition() {
	int length = ReadPanelData<int>(id, TRACED_EDGES);
	return length != 0 && length != solLength;
}

void ArrowWatchdog::action(bool status) {
	if (!status) {
		sleepTime = 0.2f;
		return;
	}
	sleepTime = 0.01f;
	solLength = 0;
	int length = ReadPanelData<int>(id, TRACED_EDGES);
	if (length == tracedLength) return;
	initPath();
	if (tracedLength == solLength || tracedLength == solLength - 1) {
		for (int x = 1; x < width; x++) {
			for (int y = 1; y < height; y++) {
				if (!checkArrow(x, y)) {
					WritePanelData<int>(id, STYLE_FLAGS, { style | Panel::Style::HAS_TRIANGLES });
					return;
				}
			}
		}
		WritePanelData<int>(id, STYLE_FLAGS, { style & ~Panel::Style::HAS_TRIANGLES });
	}
}

void ArrowWatchdog::initPath()
{
	int numTraced = ReadPanelData<int>(id, TRACED_EDGES);
	int tracedptr = ReadPanelData<int>(id, TRACED_EDGE_DATA);
	if (!tracedptr) return;
	std::vector<SolutionPoint> traced = ReadArray<SolutionPoint>(id, TRACED_EDGE_DATA, numTraced);
	if (style & Panel::Style::SYMMETRICAL) {
		for (int i = 0; i < numTraced; i++) {
			SolutionPoint sp;
			sp.pointA = (width / 2 + 1) * (height / 2 + 1) - 1 - traced[i].pointA;
			sp.pointB = (width / 2 + 1) * (height / 2 + 1) - 1 - traced[i].pointB;
			traced.push_back(sp);
		}
	}
	grid = backupGrid;
	tracedLength = numTraced;
	if (traced.size() == 0) return;
	int exitPos = Point::pillarWidth > 0 ? (width / 2) * (height / 2 + 1) : (width / 2 + 1) * (height / 2 + 1);
	for (SolutionPoint p : traced) {
		int p1 = p.pointA, p2 = p.pointB;
		if (p1 == this->exitPos || p2 == this->exitPos) {
			solLength = numTraced + 1;
		}
		if (p1 == exitPos || p2 == exitPos) {
			solLength = numTraced;
			continue;
		}
		else if (p1 > exitPos || p2 > exitPos) continue;
		if (p1 == 0 && p2 == 0 || p1 < 0 || p2 < 0) {
			return;
		}
		int x1 = (p1 % (width / 2 + 1)) * 2, y1 = height - 1 - (p1 / (width / 2 + 1)) * 2;
		int x2 = (p2 % (width / 2 + 1)) * 2, y2 = height - 1 - (p2 / (width / 2 + 1)) * 2;
		if (Point::pillarWidth > 0) {
			x1 = (p1 % (width / 2)) * 2, y1 = height - 1 - (p1 / (width / 2)) * 2;
			x2 = (p2 % (width / 2)) * 2, y2 = height - 1 - (p2 / (width / 2)) * 2;
			grid[x1][y1] = PATH;
			grid[x2][y2] = PATH;
			if (x1 == x2 || x1 == x2 + 2 || x1 == x2 - 2) grid[(x1 + x2) / 2][(y1 + y2) / 2] = PATH;
			else grid[width - 1][(y1 + y2) / 2] = PATH;
		}
		else {
			grid[x1][y1] = PATH;
			grid[x2][y2] = PATH;
			grid[(x1 + x2) / 2][(y1 + y2) / 2] = PATH;
		}
	}
}

bool ArrowWatchdog::checkArrow(int x, int y)
{
	if (Point::pillarWidth > 0) return checkArrowPillar(x, y);
	int symbol = grid[x][y];
	if ((symbol & 0x700) == Decoration::Triangle && (symbol & 0xf0000) != 0) {
		int count = 0;
		if (grid[x - 1][y] == PATH) count++;
		if (grid[x + 1][y] == PATH) count++;
		if (grid[x][y - 1] == PATH) count++;
		if (grid[x][y + 1] == PATH) count++;
		return count == (symbol >> 16);
	}
	if ((symbol & 0x700) != Decoration::Arrow)
		return true;
	int targetCount = (symbol & 0xf000) >> 12;
	Point dir = DIRECTIONS[(symbol & 0xf0000) >> 16];
	x += dir.first / 2; y += dir.second / 2;
	int count = 0;
	while (x >= 0 && x < width && y >= 0 && y < height) {
		if (grid[x][y] == PATH) {
			if (++count > targetCount) return false;
		}
		x += dir.first; y += dir.second;
	}
	return count == targetCount;
}

bool ArrowWatchdog::checkArrowPillar(int x, int y)
{
	int symbol = grid[x][y];
	if ((symbol & 0x700) == Decoration::Triangle && (symbol & 0xf0000) != 0) {
		int count = 0;
		if (grid[x - 1][y] == PATH) count++;
		if (grid[x + 1][y] == PATH) count++;
		if (grid[x][y - 1] == PATH) count++;
		if (grid[x][y + 1] == PATH) count++;
		return count == (symbol >> 16);
	}
	if ((symbol & 0x700) != Decoration::Arrow)
		return true;
	int targetCount = (symbol & 0xf000) >> 12;
	Point dir = DIRECTIONS[(symbol & 0xf0000) >> 16];
	x = (x + (dir.first > 2 ? -2 : dir.first) / 2 + Point::pillarWidth) % Point::pillarWidth; y += dir.second / 2;
	int count = 0;
	while (y >= 0 && y < height) {
		if (grid[x][y] == PATH) {
			if (++count > targetCount) return false;
		}
		x = (x + dir.first + Point::pillarWidth) % Point::pillarWidth; y += dir.second;
	}
	return count == targetCount;
}

bool BridgeWatchdog::condition()
{
	return true;
}

void BridgeWatchdog::action(bool status)
{
	int length1 = _memory->ReadPanelData<int>(id1, TRACED_EDGES);
	int length2 = _memory->ReadPanelData<int>(id2, TRACED_EDGES);
	if (solLength1 > 0 && length1 == 0) {
		_memory->WritePanelData<int>(id2, STYLE_FLAGS, { _memory->ReadPanelData<int>(id2, STYLE_FLAGS) | Panel::Style::HAS_DOTS });
	}
	if (solLength2 > 0 && length2 == 0) {
		_memory->WritePanelData<int>(id1, STYLE_FLAGS, { _memory->ReadPanelData<int>(id1, STYLE_FLAGS) | Panel::Style::HAS_DOTS });
	}
	if (length1 != solLength1 && length1 > 0 && !checkTouch(id2)) {
		_memory->WritePanelData<int>(id2, STYLE_FLAGS, { _memory->ReadPanelData<int>(id2, STYLE_FLAGS) & ~Panel::Style::HAS_DOTS });
	}
	if (length2 != solLength2 && length2 > 0 && !checkTouch(id1)) {
		_memory->WritePanelData<int>(id1, STYLE_FLAGS, { _memory->ReadPanelData<int>(id1, STYLE_FLAGS) & ~Panel::Style::HAS_DOTS });
	}
	solLength1 = length1;
	solLength2 = length2;
}

bool BridgeWatchdog::checkTouch(int id)
{
	int length = _memory->ReadPanelData<int>(id, TRACED_EDGES);
	if (length == 0) return false;
	int numIntersections = _memory->ReadPanelData<int>(id, NUM_DOTS);
	std::vector<int> intersectionFlags = _memory->ReadArray<int>(id, DOT_FLAGS, numIntersections);
	std::vector<SolutionPoint> edges = _memory->ReadArray<SolutionPoint>(id, TRACED_EDGE_DATA, length);
	for (SolutionPoint sp : edges) if (intersectionFlags[sp.pointA] == Decoration::Dot_Intersection || intersectionFlags[sp.pointB] == Decoration::Dot_Intersection) return true;
	return false;
}

bool PowerWatchdog::condition()
{
	float power = ReadPanelData<float>(id, POWER);
	return power != 1;
}

void PowerWatchdog::action(bool status)
{
	WritePanelData<float>(id, POWER, { 1, 1 });
}
