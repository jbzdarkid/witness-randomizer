#include "Panel.h"
#include "Random.h"
#include "Memory.h"
#include <sstream>

template <class T>
int find(const std::vector<T> &data, T search, size_t startIndex = 0) {
	for (size_t i=startIndex ; i<data.size(); i++) {
		if (data[i] == search) return i;
	}
	return -1;
}

Panel::Panel(int id) {
	_width = 2 * _memory.ReadPanelData<int>(id, GRID_SIZE_X, 1)[0] - 1;
	_height = 2 * _memory.ReadPanelData<int>(id, GRID_SIZE_Y, 1)[0] - 1;
	_grid.resize(_width);
	for (auto& row : _grid) row.resize(_height);

	ReadIntersections(id);
	ReadDecorations(id);
}

void Panel::Write(int id) {
	WriteIntersections(id);
	WriteDecorations(id);
	
	_memory.WritePanelData<int>(id, GRID_SIZE_X, {(_width + 1)/2});
	_memory.WritePanelData<int>(id, GRID_SIZE_Y, {(_height + 1)/2});
}

nlohmann::json Panel::Serialize() {
	nlohmann::json puzzle = {
		{"pillar", false},
		{"dots", nlohmann::json::array()},
		{"gaps", nlohmann::json::array()},
		{"name", "Imported from The Witness :O"},
		{"regionCache", nlohmann::json::object()},
	};
	if (_grid.empty()) return {};
	puzzle["grid"] = nlohmann::json::array();
	
	for (int x=0; x<_width; x++) {
		for (int y=0; y<_height; y++) {
			if (x%2 == 1 && y%2 == 1) {
				puzzle["grid"][x][y] = Decoration::to_json(_grid[x][y]);
			} else {
				puzzle["grid"][x][y] = false;
			}
		}
	}

	puzzle["startPoints"] = nlohmann::json::array();
	for (auto [x, y] : _startpoints) {
		nlohmann::json startPoint = {{"x", x}, {"y", y}};
		puzzle["startPoints"].emplace_back(startPoint);
	}
	puzzle["endPoints"] = nlohmann::json::array();
	for (Endpoint endpoint : _endpoints) {
		puzzle["endPoints"].emplace_back(endpoint.to_json());
	}

	std::string out = puzzle.dump();
	return puzzle;
}

void Panel::Random() {
/*
	for (auto& row : _decorations) {
		for (auto& cell : row) {
			cell.SetShape(cell.GetShape() & 0xFFFFFFF0);
			cell.SetShape(cell.GetShape() | Random::RandInt(1, 10));
		}
	}
*/
}

void Panel::ReadDecorations(int id) {
	int numDecorations = _memory.ReadPanelData<int>(id, NUM_DECORATIONS, 1)[0];
	std::vector<int> decorations = _memory.ReadArray<int>(id, DECORATIONS, numDecorations);

	int x = 1;
	int y = _height - 2;
	for (int decoration : decorations) {
		_grid[x][y] = decoration;
		x += 2;
		if (x > _width - 1) {
			x = 1;
			y -= 2;
		}
	}
}

void Panel::WriteDecorations(int id) {
	std::vector<int> decorations;
	for (int y=_height - 2; y>0; y-=2) {
		for (int x=1; x<_width - 1; x+=2) {
			decorations.push_back(_grid[x][y]);
		}
	}

	_memory.WritePanelData<int>(id, NUM_DECORATIONS, {static_cast<int>(decorations.size())});
	_memory.WriteArray<int>(id, DECORATIONS, decorations);
}

void Panel::ReadIntersections(int id) {
	int numIntersections = _memory.ReadPanelData<int>(id, NUM_DOTS, 1)[0];
	std::vector<int> intersectionFlags = _memory.ReadArray<int>(id, DOT_FLAGS, numIntersections);

	int x = 0;
	int y = _height - 1;
	int i = 0;
	for (;; i++) {
		if (intersectionFlags[i] & IntersectionFlags::IS_STARTPOINT) {
			_startpoints.push_back({x, y});
		}
		x += 2;
		if (x > _width) {
			x = 0;
			y -= 2;
		}
		if (y < 0) break;
	}

	std::pair<std::vector<int>, std::vector<int>> connections;
	int numConnections = _memory.ReadPanelData<int>(id, NUM_CONNECTIONS, 1)[0];
	connections.first = _memory.ReadArray<int>(id, DOT_CONNECTION_A, numConnections);
	connections.second = _memory.ReadArray<int>(id, DOT_CONNECTION_B, numConnections);
	std::vector<float> intersections = _memory.ReadArray<float>(id, DOT_POSITIONS, numIntersections*2);

	// Iterate the remaining intersections (either endpoints or gaps)
	for (; i < numIntersections; i++) {
		if (intersectionFlags[i] & IntersectionFlags::IS_ENDPOINT) {
			for (int j=0; j<numConnections; j++) {
				int location = 0;
				if (connections.first[j] == i) location = connections.second[j];
				if (connections.second[j] == i) location = connections.first[j];
				if (location != 0) {
					Endpoint::Direction dir;
					if (intersections[2*i] < intersections[2*location]) { // Our (i) x coordinate is less than the target's (location)
						dir = Endpoint::Direction::LEFT;
					} else if (intersections[2*i] > intersections[2*location]) {
						dir = Endpoint::Direction::RIGHT;
					} else if (intersections[2*i + 1] > intersections[2*location + 1]) { // y coordinate is 0 (bottom) 1 (top), so this check is reversed.
						dir = Endpoint::Direction::UP;
					} else {
						dir = Endpoint::Direction::DOWN;
					}
					int x = 2 * (location % ((_width + 1) / 2));
					int y = (_height - 1) - 2 * (location / ((_width + 1) / 2));
					_endpoints.push_back(Endpoint(x, y, dir));
				}
			}
		}
	}	
}

void Panel::WriteIntersections(int id) {
	std::vector<float> intersections;
	std::vector<int> intersectionFlags;
	std::pair<std::vector<int>, std::vector<int>> connections;

	double min = 0.1;
	double max = 0.9;
	double width_interval = (max - min) / (_width - 1);
	double height_interval = (max - min) / (_height - 1);

	for (int y=0; y<_height; y++) {
		for (int x=0; x<_width; x++) {
			intersections.push_back(min + x * width_interval);
			intersections.push_back(min + y * height_interval);
			int flags = 0;
			if (find(_startpoints, {x, y}) != -1) flags |= IntersectionFlags::IS_STARTPOINT;
			intersectionFlags.push_back(flags);
			if (y > 0) {
				connections.first.push_back(y * _width + x);
				connections.second.push_back((y - 1) * _width + x);
			}
			if (x > 0) {
				connections.first.push_back(y * _width + x);
				connections.second.push_back(y * _width + (x - 1));
			}
		}
	}

	for (Endpoint endpoint : _endpoints) {
		float xPos = min + endpoint.GetX() * width_interval;
		float yPos = min + endpoint.GetY() * height_interval;
		if (endpoint.GetDir()== Endpoint::Direction::LEFT) {
			xPos -= .05f;
		} else if (endpoint.GetDir() == Endpoint::Direction::RIGHT) {
			xPos += .05f;
		} else if (endpoint.GetDir() == Endpoint::Direction::UP) {
			yPos += .05f; // Y position goes from 0 (bottom) to 1 (top), so this is reversed.
		} else if (endpoint.GetDir() == Endpoint::Direction::DOWN) {
			yPos -= .05f;
		}
		intersections.push_back(xPos);
		intersections.push_back(yPos);

		connections.first.push_back(endpoint.GetY() * _width + endpoint.GetX()); // Target to connect to
		connections.second.push_back(intersectionFlags.size()); // This endpoint
		intersectionFlags.push_back(IntersectionFlags::IS_ENDPOINT);
	}

	_memory.WritePanelData<int>(id, NUM_DOTS, {static_cast<int>(intersectionFlags.size())});
	_memory.WriteArray<float>(id, DOT_POSITIONS, intersections);
	_memory.WriteArray<int>(id, DOT_FLAGS, intersectionFlags);
	_memory.WritePanelData<int>(id, NUM_CONNECTIONS, {static_cast<int>(connections.first.size())});
	_memory.WriteArray<int>(id, DOT_CONNECTION_A, connections.first);
	_memory.WriteArray<int>(id, DOT_CONNECTION_B, connections.second);
	_memory.WritePanelData<int>(id, NEEDS_REDRAW, {1});
}

