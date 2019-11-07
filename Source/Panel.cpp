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

Panel::Panel(const std::shared_ptr<Memory>& memory, int id) : _memory(memory) {
	_width = 2 * _memory->ReadPanelData<int>(id, GRID_SIZE_X, 1)[0] - 1;
	_height = 2 * _memory->ReadPanelData<int>(id, GRID_SIZE_Y, 1)[0] - 1;
	_grid.resize(_width);
	for (auto& row : _grid) row.resize(_height);

	ReadIntersections(id);
	ReadDecorations(id);
}

void Panel::Write(int id) {
	WriteIntersections(id);
	WriteDecorations(id);
	
	_memory->WritePanelData<int>(id, GRID_SIZE_X, {(_width + 1)/2});
	_memory->WritePanelData<int>(id, GRID_SIZE_Y, {(_height + 1)/2});
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
				puzzle["grid"][x][y] = Decoration_to_json(_grid[x][y]);
			} else {
				if (_grid[x][y] & IntersectionFlags::HAS_DOT) {
					puzzle["dots"].emplace_back(nlohmann::json({{"x", x}, {"y", y}}));
				}
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

void Panel::ReadDecorations(int id) {
	int numDecorations = _memory->ReadPanelData<int>(id, NUM_DECORATIONS, 1)[0];
	std::vector<int> decorations = _memory->ReadArray<int>(id, DECORATIONS, numDecorations);

	for (int i=0; i<numDecorations; i++) {
		auto [x, y] = dloc_to_xy(i);
		_grid[x][y] = decorations[i];
	}
}

void Panel::WriteDecorations(int id) {
	std::vector<int> decorations;
	for (int y=_height-2; y>0; y-=2) {
		for (int x=1; x<_width - 1; x+=2) {
			decorations.push_back(_grid[x][y]);
		}
	}

	_memory->WritePanelData<int>(id, NUM_DECORATIONS, {static_cast<int>(decorations.size())});
	_memory->WriteArray<int>(id, DECORATIONS, decorations);
}

void Panel::ReadIntersections(int id) {
	int numIntersections = _memory->ReadPanelData<int>(id, NUM_DOTS, 1)[0];
	std::vector<int> intersectionFlags = _memory->ReadArray<int>(id, DOT_FLAGS, numIntersections);

	int i = 0;
	for (;; i++) {
		auto [x, y] = loc_to_xy(i);
		if (y < 0) break;
		if (intersectionFlags[i] & IntersectionFlags::IS_STARTPOINT) {
			_startpoints.push_back({x, y});
		}
	}

	int numConnections = _memory->ReadPanelData<int>(id, NUM_CONNECTIONS, 1)[0];
	std::vector<int> connections_a = _memory->ReadArray<int>(id, DOT_CONNECTION_A, numConnections);
	std::vector<int> connections_b = _memory->ReadArray<int>(id, DOT_CONNECTION_B, numConnections);
	std::vector<float> intersections = _memory->ReadArray<float>(id, DOT_POSITIONS, numIntersections*2);

	// Iterate the remaining intersections (endpoints, dots, gaps)
	for (; i < numIntersections; i++) {
		if (intersectionFlags[i] & IntersectionFlags::IS_ENDPOINT) {
			for (int j=0; j<numConnections; j++) {
				int location = 0;
				if (connections_a[j] == i) location = connections_b[j];
				if (connections_b[j] == i) location = connections_a[j];
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
					auto [x, y] = loc_to_xy(location);
					_endpoints.push_back(Endpoint(x, y, dir));
					break;
				}
			}
		} else if (intersectionFlags[i] & IntersectionFlags::HAS_DOT) {
			for (int j=0; j<numConnections; j++) {
				int location = 0;
				if (connections_a[j] == i) location = connections_b[j];
				if (connections_b[j] == i) location = connections_a[j];
				if (location != 0) {
					auto [x, y] = loc_to_xy(location);
					float x1 = intersections[2*i];
					float y1 = intersections[2*i+1];
					float x2 = intersections[2*location];
					float y2 = intersections[2*location+1];
					if (intersections[2*i] < intersections[2*location]) {
						// Our (i) x coordinate is less than the target's (location), so we are to the left
						x--;
					} else if (intersections[2*i] > intersections[2*location]) { // To the right
						x++;
					} else if (intersections[2*i + 1] > intersections[2*location + 1]) {
						// y coordinate is 0 (bottom) 1 (top), so this check is reversed. We are above the target (location)
						y--;
					} else { // Beleow the target
						y++;
					}

					_grid[x][y] |= IntersectionFlags::HAS_DOT;
					break;
				}
			}
		}
	}	
}

void Panel::WriteIntersections(int id) {
	std::vector<float> intersections;
	std::vector<int> intersectionFlags;
	std::vector<int> connections_a;
	std::vector<int> connections_b;

	double min = 0.1;
	double max = 0.9;
	double width_interval = (max - min) / (_width/2);
	double height_interval = (max - min) / (_height/2);

	// TODO(future): Stop using push_back and set these into explicit locations, unrequires loop iteration order
	for (int y=_height-1; y>=0; y-=2) {
		for (int x=0; x<_width; x+=2) {
			intersections.push_back(static_cast<float>(min + (x/2) * width_interval));
			intersections.push_back(static_cast<float>(max - (y/2) * height_interval));
			int flags = 0;
			if (find(_startpoints, {x, y}) != -1) flags |= IntersectionFlags::IS_STARTPOINT;
			intersectionFlags.push_back(flags);

			// Create connections for this intersection -- always write low -> high
			if (y > 0) {
				connections_a.push_back(xy_to_loc(x, y-2));
				connections_b.push_back(xy_to_loc(x, y));
			}
			if (x > 0) {
				connections_a.push_back(xy_to_loc(x-2, y));
				connections_b.push_back(xy_to_loc(x, y));
			}
		}
	}

	for (Endpoint endpoint : _endpoints) {
		connections_a.push_back(xy_to_loc(endpoint.GetX(), endpoint.GetY())); // Target to connect to
		connections_b.push_back(static_cast<int>(intersectionFlags.size())); // This endpoint

		double xPos = min + (endpoint.GetX()/2) * width_interval;
		double yPos = max - (endpoint.GetY()/2) * height_interval;
		if (endpoint.GetDir()== Endpoint::Direction::LEFT) {
			xPos -= .05;
		} else if (endpoint.GetDir() == Endpoint::Direction::RIGHT) {
			xPos += .05;
		} else if (endpoint.GetDir() == Endpoint::Direction::UP) {
			yPos += .05; // Y position goes from 0 (bottom) to 1 (top), so this is reversed.
		} else if (endpoint.GetDir() == Endpoint::Direction::DOWN) {
			yPos -= .05;
		}
		intersections.push_back(static_cast<float>(xPos));
		intersections.push_back(static_cast<float>(yPos));
		intersectionFlags.push_back(IntersectionFlags::IS_ENDPOINT);
	}

	// Dots
	for (int y=0; y<_height; y++) {
		for (int x=0; x<_width; x++) {
			if (!(_grid[x][y] & IntersectionFlags::HAS_DOT)) continue;
			if (x%2 == 1 && y%2 == 1) continue;
			if (x%2 == 0 && y%2 == 0) {
				intersectionFlags[xy_to_loc(x, y)] |= _grid[x][y];
				continue;
			}

			// Locate the segment we're breaking
			for (int i=0; i<connections_a.size(); i++) {
				auto [x1, y1] = loc_to_xy(connections_a[i]);
				auto [x2, y2] = loc_to_xy(connections_b[i]);
				if ((x1+1 == x && x2-1 == x && y1 == y && y2 == y) ||
					(y1+1 == y && y2-1 == y && x1 == x && x2 == x)) {
					int other_connection = connections_b[i];
					connections_b[i] = static_cast<int>(intersectionFlags.size()); // This endpoint
					
					connections_a.push_back(static_cast<int>(intersectionFlags.size())); // This endpoint
					connections_b.push_back(other_connection);
					break;
				}
			}
			// Add this dot to the end
			double xPos = min + (x/2.0) * width_interval;
			double yPos = max - (y/2.0) * height_interval;
			intersections.push_back(static_cast<float>(xPos));
			intersections.push_back(static_cast<float>(yPos));
			intersectionFlags.push_back(_grid[x][y]);
		}
	}


	_memory->WritePanelData<int>(id, NUM_DOTS, {static_cast<int>(intersectionFlags.size())});
	_memory->WriteArray<float>(id, DOT_POSITIONS, intersections);
	_memory->WriteArray<int>(id, DOT_FLAGS, intersectionFlags);
	_memory->WritePanelData<int>(id, NUM_CONNECTIONS, {static_cast<int>(connections_a.size())});
	_memory->WriteArray<int>(id, DOT_CONNECTION_A, connections_a);
	_memory->WriteArray<int>(id, DOT_CONNECTION_B, connections_b);
	_memory->WritePanelData<int>(id, NEEDS_REDRAW, {1});
}

