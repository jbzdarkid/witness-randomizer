#include "Panel.h"
#include "Memory.h"

#pragma warning (disable:26451)
#pragma warning (disable:26812)

PuzzleSerializer::PuzzleSerializer(const std::shared_ptr<Memory>& memory) : _memory(memory) {}

Puzzle PuzzleSerializer::ReadPuzzle(int id) {
    Puzzle p;
    p.width = 2 * _memory->ReadPanelData<int>(id, GRID_SIZE_X, 1)[0] - 1;
    p.height = 2 * _memory->ReadPanelData<int>(id, GRID_SIZE_Y, 1)[0] - 1;
    p.grid.resize(p.width);
	for (auto& row : p.grid) row.resize(p.height);

	ReadIntersections(p, id);
	ReadDecorations(p, id);

    return p;
}

void PuzzleSerializer::ReadIntersections(Puzzle& p, int id) {
	int numIntersections = _memory->ReadPanelData<int>(id, NUM_DOTS, 1)[0];
	std::vector<int> intersectionFlags = _memory->ReadArray<int>(id, DOT_FLAGS, numIntersections);

	int i = 0;
	for (;; i++) {
		auto [x, y] = loc_to_xy(p, i);
		if (y < 0) break;
		if (intersectionFlags[i] & Flags::IS_STARTPOINT) {
            p.grid[x][y].start = true;
		}
        p.grid[x][y].dot = FlagsToDot(intersectionFlags[i]);
	}

	int numConnections = _memory->ReadPanelData<int>(id, NUM_CONNECTIONS, 1)[0];
	std::vector<int> connections_a = _memory->ReadArray<int>(id, DOT_CONNECTION_A, numConnections);
	std::vector<int> connections_b = _memory->ReadArray<int>(id, DOT_CONNECTION_B, numConnections);
	std::vector<float> intersections = _memory->ReadArray<float>(id, DOT_POSITIONS, numIntersections*2);

	// Iterate the remaining intersections (endpoints, dots, gaps)
	for (; i < numIntersections; i++) {
		if (intersectionFlags[i] & Flags::IS_ENDPOINT) {
			for (int j=0; j<numConnections; j++) {
				int location = 0;
				if (connections_a[j] == i) location = connections_b[j];
				if (connections_b[j] == i) location = connections_a[j];
				if (location != 0) {
					auto [x, y] = loc_to_xy(p, location);
					if (intersections[2*i] < intersections[2*location]) { // Our (i) x coordinate is less than the target's (location)
                        p.grid[x][y].end = Cell::Dir::LEFT;
					} else if (intersections[2*i] > intersections[2*location]) {
                        p.grid[x][y].end = Cell::Dir::RIGHT;
					} else if (intersections[2*i + 1] > intersections[2*location + 1]) { // y coordinate is 0 (bottom) 1 (top), so this check is reversed.
                        p.grid[x][y].end = Cell::Dir::UP;
					} else {
                        p.grid[x][y].end = Cell::Dir::DOWN;
					}
					break;
				}
			}
		} else if (intersectionFlags[i] & Flags::HAS_DOT) {
			for (int j=0; j<numConnections; j++) {
				int location = 0;
				if (connections_a[j] == i) location = connections_b[j];
				if (connections_b[j] == i) location = connections_a[j];
				if (location != 0) {
					auto [x, y] = loc_to_xy(p, location);
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

                    p.grid[x][y].dot = FlagsToDot(intersectionFlags[i]);
					break;
				}
			}
		}
	}	
}

void PuzzleSerializer::ReadDecorations(Puzzle& p, int id) {
	int numDecorations = _memory->ReadPanelData<int>(id, NUM_DECORATIONS, 1)[0];
	std::vector<int> decorations = _memory->ReadArray<int>(id, DECORATIONS, numDecorations);
    if (numDecorations != decorations.size()) return; // @Error!

	for (int i=0; i<numDecorations; i++) {
		auto [x, y] = dloc_to_xy(p, i);
        auto d = std::make_shared<Decoration>();
        p.grid[x][y].decoration = d;
        d->type = static_cast<Shape>(decorations[i] & 0xFF00);
        switch(d->type) {
            case Shape::Poly:
            case Shape::RPoly:
            case Shape::Ylop:
                d->polyshape = decorations[i] & 0xFFFF0000;
                break;
            case Shape::Triangle:
                d->count = decorations[i] & 0x000F0000;
                break;
        }
        d->color = static_cast<Color>(decorations[i] & 0xF);
	}
}

void PuzzleSerializer::WritePuzzle(const Puzzle& p, int id) {
	_memory->WritePanelData<int>(id, GRID_SIZE_X, {(p.width + 1)/2});
	_memory->WritePanelData<int>(id, GRID_SIZE_Y, {(p.height + 1)/2});

	WriteIntersections(p, id);
	WriteDecorations(p, id);

	_memory->WritePanelData<int>(id, NEEDS_REDRAW, {1});
}

void PuzzleSerializer::WriteIntersections(const Puzzle& p, int id) {
	std::vector<float> intersections;
	std::vector<int> intersectionFlags;
	std::vector<int> connections_a;
	std::vector<int> connections_b;

	float min = 0.1f;
	float max = 0.9f;
	float width_interval = (max - min) / (p.width/2);
	float height_interval = (max - min) / (p.height/2);

	// @Cleanup: If I write directly to locations, then I can remove this gross loop iterator.
	for (int y=p.height-1; y>=0; y-=2) {
		for (int x=0; x<p.width; x+=2) {
			intersections.push_back(static_cast<float>(min + (x/2) * width_interval));
			intersections.push_back(static_cast<float>(max - (y/2) * height_interval));
			int flags = 0;
            if (p.grid[x][y].start) {
                flags |= Flags::IS_STARTPOINT;
            }
			intersectionFlags.push_back(flags);

			// Create connections for this intersection -- always write low -> high
			if (y > 0) {
				connections_a.push_back(xy_to_loc(p, x, y-2));
				connections_b.push_back(xy_to_loc(p, x, y));
			}
			if (x > 0) {
				connections_a.push_back(xy_to_loc(p, x-2, y));
				connections_b.push_back(xy_to_loc(p, x, y));
			}
		}
	}

    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
            if (p.grid[x][y].end == Cell::Dir::NONE) continue;
            connections_a.push_back(xy_to_loc(p, x, y)); // Target to connect to
		    connections_b.push_back(static_cast<int>(intersectionFlags.size())); // This endpoint

		    float xPos = min + (x/2) * width_interval;
		    float yPos = max - (y/2) * height_interval;
            switch (p.grid[x][y].end) {
                case Cell::Dir::LEFT:
			        xPos -= .05f;
                    break;
                case Cell::Dir::RIGHT:
			        xPos += .05f;
                    break;
                case Cell::Dir::UP:
			        yPos += .05f; // Y position goes from 0 (bottom) to 1 (top), so this is reversed.
                    break;
                case Cell::Dir::DOWN:
			        yPos -= .05f;
                    break;
            }
		    intersections.push_back(xPos);
		    intersections.push_back(yPos);
		    intersectionFlags.push_back(Flags::IS_ENDPOINT);
        }
    }

	// Dots
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
			if (p.grid[x][y].dot == Cell::Dot::NONE) continue;
			if (x%2 == 1 && y%2 == 1) continue;

            int flags = Flags::HAS_DOT;
            switch (p.grid[x][y].dot) {
                case Cell::Dot::BLACK:
                    break;
                case Cell::Dot::BLUE:
                    flags |= DOT_IS_BLUE;
                    break;
                case Cell::Dot::YELLOW:
                    flags |= DOT_IS_ORANGE;
                    break;
                case Cell::Dot::INVISIBLE:
                    flags |= DOT_IS_INVISIBLE;
                    break;
            }

            // Dot is already a point the grid, just overwrite the flags
			if (x%2 == 0 && y%2 == 0) {
				intersectionFlags[xy_to_loc(p, x, y)] |= flags;
				continue;
			}

            // Else, we need to introduce a new segment
			// Locate the segment we're breaking
			for (int i=0; i<connections_a.size(); i++) {
				auto [x1, y1] = loc_to_xy(p, connections_a[i]);
				auto [x2, y2] = loc_to_xy(p, connections_b[i]);
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
			float xPos = min + (x/2.0f) * width_interval;
			float yPos = max - (y/2.0f) * height_interval;
			intersections.push_back(xPos);
			intersections.push_back(yPos);
            intersectionFlags.push_back(flags);
		}
	}

	_memory->WritePanelData<int>(id, NUM_DOTS, {static_cast<int>(intersectionFlags.size())});
	_memory->WriteArray<float>(id, DOT_POSITIONS, intersections);
	_memory->WriteArray<int>(id, DOT_FLAGS, intersectionFlags);
	_memory->WritePanelData<int>(id, NUM_CONNECTIONS, {static_cast<int>(connections_a.size())});
	_memory->WriteArray<int>(id, DOT_CONNECTION_A, connections_a);
	_memory->WriteArray<int>(id, DOT_CONNECTION_B, connections_b);
}

void PuzzleSerializer::WriteDecorations(const Puzzle& p, int id) {
	std::vector<int> decorations;
	for (int y=p.height-2; y>0; y-=2) {
		for (int x=1; x<p.width-1; x+=2) {
            auto d = p.grid[x][y].decoration;
            if (d) {
                decorations.push_back(d->color | d->type | d->count | d->polyshape);
            } else {
                decorations.push_back(0);
            }
		}
	}

	_memory->WritePanelData<int>(id, NUM_DECORATIONS, {static_cast<int>(decorations.size())});
	_memory->WriteArray<int>(id, DECORATIONS, decorations);
}

std::tuple<int, int> PuzzleSerializer::loc_to_xy(const Puzzle& p, int location) {
    int height2 = (p.height - 1) / 2;
    int width2 = (p.width + 1) / 2;

    int x = 2 * (location % width2);
    int y = 2 * (height2 - location / width2);
    return {x, y};
}

int PuzzleSerializer::xy_to_loc(const Puzzle& p, int x, int y) {
    int height2 = (p.height - 1) / 2;
    int width2 = (p.width + 1) / 2;

    int rowsFromBottom = height2 - y/2;
    return rowsFromBottom * width2 + x/2;
}

std::tuple<int, int> PuzzleSerializer::dloc_to_xy(const Puzzle& p, int location) {
    int height2 = (p.height - 3) / 2;
    int width2 = (p.width - 1) / 2;

    int x = 2 * (location % width2) + 1;
    int y = 2 * (height2 - location / width2) + 1;
    return {x, y};
}

int PuzzleSerializer::xy_to_dloc(const Puzzle& p, int x, int y) {
    int height2 = (p.height - 3) / 2;
    int width2 = (p.width - 1) / 2;

    int rowsFromBottom = height2 - (y - 1)/2;
    return rowsFromBottom * width2 + (x - 1)/2;
}

Cell::Dot PuzzleSerializer::FlagsToDot(int flags) {
    if (!(flags & Flags::HAS_DOT)) return Cell::Dot::NONE;
    if (flags & Flags::DOT_IS_BLUE) {
        return Cell::Dot::BLUE;
    } else if (flags & Flags::DOT_IS_ORANGE) {
        return Cell::Dot::YELLOW;
    } else if (flags & Flags::DOT_IS_INVISIBLE) {
        return Cell::Dot::INVISIBLE;
    } else {
        return Cell::Dot::BLACK;
    }
}
