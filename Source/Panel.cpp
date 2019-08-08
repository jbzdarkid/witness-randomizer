#include "Panel.h"
#include "Random.h"
#include "Memory.h"
#include "Randomizer.h"
#include <sstream>

template <class T>
int find(const std::vector<T> &data, T search, size_t startIndex = 0) {
	for (size_t i=startIndex ; i<data.size(); i++) {
		if (data[i] == search) return static_cast<int>(i);
	}
	return -1;
}

struct Color {
	int r;
	int g;
	int b;
	int a;
};

Panel::Panel(int id) {
	_memory = std::make_shared<Memory>("witness64_d3d11.exe");
	_width = 2 * _memory->ReadPanelData<int>(id, GRID_SIZE_X) - 1;
	_height = 2 * _memory->ReadPanelData<int>(id, GRID_SIZE_Y) - 1;
	_grid.resize(_width);
	for (auto& row : _grid) row.resize(_height);

	_style = _memory->ReadPanelData<int>(id, STYLE_FLAGS);
	ReadAllData(id);
	ReadIntersections(id);
	ReadDecorations(id);
}

void Panel::Write(int id) {
	WriteIntersections(id);
	WriteDecorations(id);
	
	//_memory->WritePanelData<int>(id, PUSH_SYMBOL_COLORS, { 0 });
	_memory->WritePanelData<int>(id, DECORATION_COLORS, { 0 });
	_memory->WritePanelData<int>(id, STYLE_FLAGS, { _style & ~NO_BLINK });
	_memory->WritePanelData<int>(id, GRID_SIZE_X, {(_width + 1)/2});
	_memory->WritePanelData<int>(id, GRID_SIZE_Y, {(_height + 1)/2});
	_memory->WritePanelData<int>(id, NEEDS_REDRAW, { 1 });
}

void Panel::SetSymbol(int x, int y, Decoration::Shape symbol, Decoration::Color color)
{
	SetGridSymbol(x * 2 + 1, y * 2 + 1, symbol, color);
}

void Panel::SetShape(int x, int y, int shape, bool rotate, bool negative, Decoration::Color color)
{
	if (!shape) return;
	int symbol = Decoration::Shape::Poly;
	while (!(shape & 0xf)) shape >>= 4;
	while (!(shape & 0x1111)) shape >>= 1;
	shape <<= 16;
	if (rotate) shape |= Decoration::Shape::Can_Rotate;
	else shape &= ~Decoration::Shape::Can_Rotate;
	if (negative) shape |= Decoration::Shape::Negative;
	else shape &= ~Decoration::Shape::Negative;
	_grid[x * 2 + 1][y * 2 + 1] = symbol | shape | color;
}

void Panel::ClearSymbol(int x, int y)
{
	ClearGridSymbol(x * 2 + 1, y * 2 + 1);
}

void Panel::SetGridSymbol(int x, int y, Decoration::Shape symbol, Decoration::Color color)
{
	if (symbol & IntersectionFlags::COLUMN) x--;
	if (symbol & IntersectionFlags::ROW) y--;
	if (symbol == Decoration::Start) _startpoints.push_back({ x, y });
	if (symbol == Decoration::Exit) {
		Endpoint::Direction dir;
		if (y == 0) dir = Endpoint::Direction::UP;
		else if (y == _height - 1) dir = Endpoint::Direction::DOWN;
		else if (x == 0) dir = Endpoint::Direction::LEFT;
		else dir = Endpoint::Direction::RIGHT;
		_endpoints.push_back(Endpoint(x, y, dir, IntersectionFlags::ENDPOINT | 
			(dir == Endpoint::Direction::UP || dir == Endpoint::Direction::DOWN ?
				IntersectionFlags::COLUMN : IntersectionFlags::ROW)));
	}
	else _grid[x][y] = symbol | color;
}

void Panel::ClearGridSymbol(int x, int y)
{
	_grid[x][y] = 0;
}

void Panel::Random() {
	//
}

void Panel::ReadAllData(int id) {
	Color pathColor = _memory->ReadPanelData<Color>(id, PATH_COLOR);
	Color rpathColor = _memory->ReadPanelData<Color>(id, REFLECTION_PATH_COLOR);
	std::vector<byte> pathColorb = _memory->ReadPanelData<byte>(id, PATH_COLOR, 16);
	std::vector<byte> rpathColorb = _memory->ReadPanelData<byte>(id, REFLECTION_PATH_COLOR, 16);
	Color successColor = _memory->ReadPanelData<Color>(id, SUCCESS_COLOR_A);
	Color strobeColor = _memory->ReadPanelData<Color>(id, STROBE_COLOR_A);
	Color errorColor = _memory->ReadPanelData<Color>(id, ERROR_COLOR);
	int numDecorations = _memory->ReadPanelData<int>(id, NUM_DECORATIONS);
	Color a = _memory->ReadPanelData<Color>(id, SYMBOL_A);
	Color b = _memory->ReadPanelData<Color>(id, SYMBOL_B);
	Color c = _memory->ReadPanelData<Color>(id, SYMBOL_C);
	Color d = _memory->ReadPanelData<Color>(id, SYMBOL_D);
	Color e = _memory->ReadPanelData<Color>(id, SYMBOL_E);
	Color ppColor = _memory->ReadPanelData<Color>(id, PATTERN_POINT_COLOR);
	Color ppColorA = _memory->ReadPanelData<Color>(id, PATTERN_POINT_COLOR_A);
	Color ppColorB = _memory->ReadPanelData<Color>(id, PATTERN_POINT_COLOR_B);
	int pushSymbolColors = _memory->ReadPanelData<int>(id, PUSH_SYMBOL_COLORS);
	int numColored = _memory->ReadPanelData<int>(id, NUM_COLORED_REGIONS);
	if (numColored) {
		numColored++;
		numColored--;
	}
	std::vector<int> colored = _memory->ReadArray<int>(id, COLORED_REGIONS, numColored);
	int numConnections = _memory->ReadPanelData<int>(id, NUM_CONNECTIONS);
	int style = _memory->ReadPanelData<int>(id, STYLE_FLAGS);
}

void Panel::ReadDecorations(int id) {
	int numDecorations = _memory->ReadPanelData<int>(id, NUM_DECORATIONS);
	std::vector<int> decorations = _memory->ReadArray<int>(id, DECORATIONS, numDecorations);
	std::vector<int> decorationFlags = _memory->ReadArray<int>(id, DECORATION_FLAGS, numDecorations);
	int ptr = _memory->ReadPanelData<int>(id, DECORATION_COLORS);
	if (ptr) std::vector<Color> decorationColors = _memory->ReadArray<Color>(id, DECORATION_COLORS, numDecorations);
	Color color = _memory->ReadPanelData<Color>(id, PATH_COLOR);

	for (int i=0; i<numDecorations; i++) {
		auto [x, y] = dloc_to_xy(i);
		_grid[x][y] = decorations[i];
	}
}

void Panel::WriteDecorations(int id) {
	std::vector<int> decorations;
	bool any = false;
	_style &= ~HAS_STONES & ~HAS_STARS & ~HAS_SHAPERS & ~HAS_ERASERS & ~HAS_TRIANGLES; //Remove all element flags
	for (int y=_height-2; y>0; y-=2) {
		for (int x=1; x<_width - 1; x+=2) {
			decorations.push_back(_grid[x][y]);
			if (_grid[x][y] != 0 && _grid[x][y] != OPEN) any = true;
			if ((_grid[x][y] & Decoration::Shape::Stone) == Decoration::Shape::Stone) _style |= HAS_STONES;
			if ((_grid[x][y] & Decoration::Shape::Star) == Decoration::Shape::Star) _style |= HAS_STARS;
			if ((_grid[x][y] & Decoration::Shape::Poly) == Decoration::Shape::Poly) _style |= HAS_SHAPERS;
			if ((_grid[x][y] & Decoration::Shape::Eraser) == Decoration::Shape::Eraser) _style |= HAS_ERASERS;
			if ((_grid[x][y] & Decoration::Shape::Triangle) == Decoration::Shape::Triangle) _style |= HAS_TRIANGLES;
		}
	}
	if (!any) {
		_memory->WritePanelData<int>(id, NUM_DECORATIONS, { 0 });
		return;
	}
	_memory->WritePanelData<int>(id, NUM_DECORATIONS, {static_cast<int>(decorations.size())});
	if (decorations.size()) _memory->WriteArray<int>(id, DECORATIONS, decorations);
}

void Panel::ReadIntersections(int id) {
	int numIntersections = _memory->ReadPanelData<int>(id, NUM_DOTS);
	std::vector<int> intersectionFlags = _memory->ReadArray<int>(id, DOT_FLAGS, numIntersections);
	int i = 0;
	
	for (; i < num_grid_points(); i++) {
		auto [x, y] = loc_to_xy(i);
		_grid[x][y] = intersectionFlags[i];
		if (intersectionFlags[i] & IntersectionFlags::STARTPOINT) {
			_startpoints.push_back({x, y});
		}
	}

	int numConnections = _memory->ReadPanelData<int>(id, NUM_CONNECTIONS);
	std::vector<int> connections_a = _memory->ReadArray<int>(id, DOT_CONNECTION_A, numConnections);
	std::vector<int> connections_b = _memory->ReadArray<int>(id, DOT_CONNECTION_B, numConnections);
	//Remove non-existent connections
	std::vector<std::string> out;
	for (int i = 0; i < connections_a.size(); i++) {
		out.push_back("(" + std::to_string(connections_a[i]) + ", " + std::to_string(connections_b[i]) + ")");
		if (connections_a[i] >= num_grid_points() || connections_b[i] >= num_grid_points()) continue;
		auto[x, y] = loc_to_xy(connections_a[i]);
		auto[x2, y2] = loc_to_xy(connections_b[i]);
		_grid[(x + x2) / 2][(y + y2) / 2] = OPEN;
	}
	for (int y = 0; y < _height; y++) {
		for (int x = 0; x < _width; x++) {
			if (x % 2 == y % 2) continue;
			_grid[x][y] ^= OPEN; //Using 1 to mark non-existent connections
		}
	}


	std::vector<float> intersections = _memory->ReadArray<float>(id, DOT_POSITIONS, numIntersections*2);
	int num_grid_points = this->num_grid_points();
	minx = intersections[0]; miny = intersections[1];
	maxx = intersections[num_grid_points * 2 - 2]; maxy = intersections[num_grid_points * 2 - 1];

	// Iterate the remaining intersections (endpoints, dots, gaps)
	float unitWidth = (maxx - minx) / (_width - 1);
	float unitHeight = (maxy - miny) / (_height - 1);
	for (i = num_grid_points; i < numIntersections; i++) {
		float xd = (intersections[i * 2] - minx) / unitWidth;
		float yd = (intersections[i * 2 + 1] - miny) / unitHeight;
		int x = std::clamp((int)std::round(xd), 0, _width - 1);
		int y = _height - 1 - std::clamp((int)std::round(yd), 0, _height - 1);
		if (intersectionFlags[i] & IntersectionFlags::GAP) {
			float xd2 = (intersections[i * 2 + 2] - minx) / unitWidth;
			float yd2 = (intersections[i * 2 + 3] - miny) / unitHeight;
			x = std::clamp((int)std::round((xd + xd2)/2), 0, _width - 1);
			y = _height - 1 - std::clamp((int)std::round((yd + yd2) / 2), 0, _height - 1);
			i++;
		}
		if (intersectionFlags[i] & IntersectionFlags::ENDPOINT) {
			for (int j = 0; j<numConnections; j++) {
				int location = 0;
				if (connections_a[j] == i) location = connections_b[j];
				if (connections_b[j] == i) location = connections_a[j];
				if (location != 0) {
					Endpoint::Direction dir;
					if (intersections[2 * i] < intersections[2 * location]) { // Our (i) x coordinate is less than the target's (location)
						dir = Endpoint::Direction::LEFT;
					}
					else if (intersections[2 * i] > intersections[2 * location]) {
						dir = Endpoint::Direction::RIGHT;
					}
					else if (intersections[2 * i + 1] > intersections[2 * location + 1]) { // y coordinate is 0 (bottom) 1 (top), so this check is reversed.
						dir = Endpoint::Direction::UP;
					}
					else {
						dir = Endpoint::Direction::DOWN;
					}
					auto[x, y] = loc_to_xy(location);
					_endpoints.push_back(Endpoint(x, y, dir, intersectionFlags[i]));
					break;
				}
			}
		}

		else {
			_grid[x][y] = intersectionFlags[i];
		}
	}	
}

void Panel::WriteIntersections(int id) {
	std::vector<float> intersections;
	std::vector<int> intersectionFlags;
	std::vector<int> connections_a;
	std::vector<int> connections_b;

	float unitWidth = (maxx - minx) / (_width - 1);
	float unitHeight = (maxy - miny) / (_height - 1);

	_style &= ~HAS_DOTS;

	for (int y = _height - 1; y >= 0; y -= 2) {
		for (int x = 0; x <_width; x += 2) {
			intersections.push_back(static_cast<float>(minx + x * unitWidth));
			intersections.push_back(static_cast<float>(miny + (_height - 1 - y) * unitHeight));
			intersectionFlags.push_back(_grid[x][y]);
			if (_grid[x][y] | DOT) _style |= HAS_DOTS;

			// Create connections for this intersection -- always write low -> high
			if (y > 0 && _grid[x][y - 1] != OPEN) {
				connections_a.push_back(xy_to_loc(x, y - 2));
				connections_b.push_back(xy_to_loc(x, y));
			}
			if (x > 0 && _grid[x - 1][y] != OPEN) {
				connections_a.push_back(xy_to_loc(x - 2, y));
				connections_b.push_back(xy_to_loc(x, y));
			}
		}
	}

	for (Endpoint endpoint : _endpoints) {
		connections_a.push_back(xy_to_loc(endpoint.GetX(), endpoint.GetY())); // Target to connect to
		connections_b.push_back(static_cast<int>(intersectionFlags.size())); // This endpoint

		double xPos = minx + endpoint.GetX() * unitWidth;
		double yPos = miny + (_height - 1 - endpoint.GetY()) * unitHeight;
		if (endpoint.GetDir() == Endpoint::Direction::LEFT) {
			xPos -= 0.05;
		}
		else if (endpoint.GetDir() == Endpoint::Direction::RIGHT) {
			xPos += 0.05;
		}
		else if (endpoint.GetDir() == Endpoint::Direction::UP) {
			yPos += 0.05;
		}
		else if (endpoint.GetDir() == Endpoint::Direction::DOWN) {
			yPos -= 0.05;
		}
		intersections.push_back(static_cast<float>(xPos));
		intersections.push_back(static_cast<float>(yPos));
		intersectionFlags.push_back(endpoint.GetFlags());
	}
	
	// Dots/Gaps
	for (int y = 0; y < _height; y++) {
		for (int x = 0; x < _width; x++) {
			if (x % 2 == y % 2) continue;
			if (_grid[x][y] == 0 || _grid[x][y] == OPEN) continue;

			// Locate the segment we're breaking
			for (int i = 0; i < connections_a.size(); i++) {
				auto[x1, y1] = loc_to_xy(connections_a[i]);
				auto[x2, y2] = loc_to_xy(connections_b[i]);
				if ((x1 == x - 1 && x2 == x + 1 && y1 == y && y2 == y) ||
					(y1 == y - 1 && y2 == y + 1 && x1 == x && x2 == x)) {
					int other_connection = connections_b[i];
					if (_grid[x][y] & IntersectionFlags::GAP) {
						connections_b[i] = static_cast<int>(intersectionFlags.size());
						connections_a.push_back(other_connection);
						connections_b.push_back(static_cast<int>(intersectionFlags.size() + 1));
					}
					else {
						connections_b[i] = static_cast<int>(intersectionFlags.size());
						connections_a.push_back(static_cast<int>(intersectionFlags.size()));
						connections_b.push_back(other_connection);
					}
					break;
				}
			}
			
			if (_grid[x][y] & IntersectionFlags::GAP) {
				double xOffset = _grid[x][y] & IntersectionFlags::ROW ? 0.5 : 0;
				double yOffset = _grid[x][y] & IntersectionFlags::COLUMN ? 0.5 : 0;
				intersections.push_back(static_cast<float>(minx + (x - xOffset) * unitWidth));
				intersections.push_back(static_cast<float>(miny + (_height - 1 - y + yOffset) * unitHeight));
				intersectionFlags.push_back(_grid[x][y]);
				intersections.push_back(static_cast<float>(minx + (x + xOffset) * unitWidth));
				intersections.push_back(static_cast<float>(miny + (_height - 1 - y - yOffset) * unitHeight));
				intersectionFlags.push_back(_grid[x][y]);
			}
			else {
				intersections.push_back(static_cast<float>(minx + x * unitWidth));
				intersections.push_back(static_cast<float>(miny + (_height - 1 - y) * unitHeight));
				intersectionFlags.push_back(_grid[x][y]);
				if (_grid[x][y] | DOT) _style |= HAS_DOTS;
			}
			
		}
	}

	_memory->WritePanelData<int>(id, NUM_DOTS, { static_cast<int>(intersectionFlags.size()) });
	_memory->WriteArray<float>(id, DOT_POSITIONS, intersections);
	_memory->WriteArray<int>(id, DOT_FLAGS, intersectionFlags);
	_memory->WritePanelData<int>(id, NUM_CONNECTIONS, { static_cast<int>(connections_a.size()) });
	_memory->WriteArray<int>(id, DOT_CONNECTION_A, connections_a);
	_memory->WriteArray<int>(id, DOT_CONNECTION_B, connections_b);
}

