#include "Panel.h"
#include "Memory.h"

#pragma warning (disable:26451)
#pragma warning (disable:26812)

PuzzleSerializer::PuzzleSerializer(const std::shared_ptr<Memory>& memory) : _memory(memory) {}

Puzzle PuzzleSerializer::ReadPuzzle(int id) {
    Puzzle p;
    p.width = 2 * _memory->ReadPanelData<int>(id, GRID_SIZE_X, 1)[0] - 1;
    p.height = 2 * _memory->ReadPanelData<int>(id, GRID_SIZE_Y, 1)[0] - 1;
    if (p.width < 0 || p.height < 0) return p; // @Error: Grid size should be always positive? Looks like the starting panels break this rule, though.
    p.grid.resize(p.width);
	for (auto& row : p.grid) row.resize(p.height);

	ReadIntersections(p, id);
	ReadDecorations(p, id);

    return p;
}

void PuzzleSerializer::ReadIntersections(Puzzle& p, int id) {
	int numIntersections = _memory->ReadPanelData<int>(id, NUM_DOTS, 1)[0];
	std::vector<int> intersectionFlags = _memory->ReadArray<int>(id, DOT_FLAGS, numIntersections);
	int numConnections = _memory->ReadPanelData<int>(id, NUM_CONNECTIONS, 1)[0];
	std::vector<int> connections_a = _memory->ReadArray<int>(id, DOT_CONNECTION_A, numConnections);
	std::vector<int> connections_b = _memory->ReadArray<int>(id, DOT_CONNECTION_B, numConnections);
	std::vector<float> intersectionLocations = _memory->ReadArray<float>(id, DOT_POSITIONS, numIntersections*2);

    // @Cleanup: Change defaults?
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
            if (x%2 == y%2) continue;
            p.grid[x][y].gap = Cell::Gap::FULL;
        }
    }

    for (int j=0; j<numIntersections; j++) {
		float x1 = intersectionLocations[2*connections_a[j]];
		float y1 = intersectionLocations[2*connections_a[j]+1];
		float x2 = intersectionLocations[2*connections_b[j]];
		float y2 = intersectionLocations[2*connections_b[j]+1];
        auto [x, y] = loc_to_xy(p, connections_a[j]);
        
			 if (x1 < x2) x++;
		else if (x1 > x2) x--;
		else if (y1 < y2) y--;
		else if (y1 > y2) y++;
        p.grid[x][y].gap = Cell::Gap::NONE;
    }

    // This iterates bottom-top, left-right
	int i = 0;
	for (;; i++) {
        int flags = intersectionFlags[i];
		auto [x, y] = loc_to_xy(p, i);
		if (y < 0) break;
		if (flags & Flags::IS_STARTPOINT) {
            p.grid[x][y].start = true;
		}
        p.grid[x][y].dot = FlagsToDot(flags);
        if (flags & Flags::IS_FULL_GAP) {
            p.grid[x][y].gap = Cell::Gap::FULL;
        }
	}

	// Iterate the remaining intersections (endpoints, dots, gaps)
	for (; i < numIntersections; i++) {
        int location = FindConnection(i, connections_a, connections_b);
		if (location == -1) continue; // @Error: Unable to find connection point
        // (x1, y1) location of this intersection
        // (x2, y2) location of the connected intersection
		float x1 = intersectionLocations[2*i];
		float y1 = intersectionLocations[2*i+1];
		float x2 = intersectionLocations[2*location];
		float y2 = intersectionLocations[2*location+1];
        auto [x, y] = loc_to_xy(p, location);

		if (intersectionFlags[i] & Flags::IS_ENDPOINT) {
            // Our x coordinate is less than the target's
			     if (x1 < x2) p.grid[x][y].end = Cell::Dir::LEFT;
			else if (x1 > x2) p.grid[x][y].end = Cell::Dir::RIGHT;
            // Note that Y coordinates are reversed: 0.0 (bottom) 1.0 (top)
			else if (y1 < y2) p.grid[x][y].end = Cell::Dir::DOWN;
            else if (y1 > y2) p.grid[x][y].end = Cell::Dir::UP;
		} else if (intersectionFlags[i] & Flags::HAS_DOT) {
			     if (x1 < x2) x--;
			else if (x1 > x2) x++;
			else if (y1 < y2) y++;
			else if (y1 > y2) y--;
            p.grid[x][y].dot = FlagsToDot(intersectionFlags[i]);
        } else if (intersectionFlags[i] & Flags::HAS_NO_CONN) {
			     if (x1 < x2) x--;
			else if (x1 > x2) x++;
			else if (y1 < y2) y++;
			else if (y1 > y2) y--;
            p.grid[x][y].gap = Cell::Gap::BREAK;
        }
	}	
}

void PuzzleSerializer::ReadDecorations(Puzzle& p, int id) {
	int numDecorations = _memory->ReadPanelData<int>(id, NUM_DECORATIONS, 1)[0];
	std::vector<int> decorations = _memory->ReadArray<int>(id, DECORATIONS, numDecorations);
    if (numDecorations > 0) p.hasDecorations = true;

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
    if (p.hasDecorations) WriteDecorations(p, id);

	_memory->WritePanelData<int>(id, NEEDS_REDRAW, {1});
}

void PuzzleSerializer::WriteIntersections(const Puzzle& p, int id) {
	std::vector<float> intersectionLocations;
	std::vector<int> intersectionFlags;
	std::vector<int> connections_a;
	std::vector<int> connections_b;

	float min = 0.1f;
	float max = 0.9f;
	float width_interval = (max - min) / (p.width/2);
	float height_interval = (max - min) / (p.height/2);
    float horiz_gap_size = width_interval / 2;
    float verti_gap_size = height_interval / 2;

    // TODO: Compute HAS_NO_CONN / HAS_HORIZ_CONN / HAS_VERTI_CONN in this loop.
	// @Cleanup: If I write directly to locations, then I can simplify this gross loop iterator.
    // int numIntersections = (p.width / 2 + 1) * (p.height / 2 + 1);
    // Grided intersections
	for (int y=p.height-1; y>=0; y-=2) {
		for (int x=0; x<p.width; x+=2) {
			intersectionLocations.push_back(min + (x/2) * width_interval);
			intersectionLocations.push_back(max - (y/2) * height_interval);
			int flags = 0;
            if (p.grid[x][y].start) {
                flags |= Flags::IS_STARTPOINT;
            }
            if (p.grid[x][y].gap == Cell::Gap::FULL) {
                flags |= Flags::IS_FULL_GAP;
            }
            switch (p.grid[x][y].dot) {
                case Cell::Dot::BLACK:
                    flags |= Flags::HAS_DOT;
                    break;
                case Cell::Dot::BLUE:
                    flags |= Flags::HAS_DOT | Flags::DOT_IS_BLUE;
                    break;
                case Cell::Dot::YELLOW:
                    flags |= Flags::HAS_DOT | Flags::DOT_IS_ORANGE;
                    break;
                case Cell::Dot::INVISIBLE:
                    flags |= Flags::HAS_DOT | Flags::DOT_IS_INVISIBLE;
                    break;
            }

			intersectionFlags.push_back(flags);

			// Create connections for this intersection -- always write the smaller value into a, the larger into b
            // Bottom connection
			if (y > 0 && p.grid[x][y-1].gap == Cell::Gap::NONE) {
				connections_a.push_back(xy_to_loc(p, x, y-2));
				connections_b.push_back(xy_to_loc(p, x, y));
			}
            // Left connection
			if (x > 0 && p.grid[x-1][y].gap == Cell::Gap::NONE) {
				connections_a.push_back(xy_to_loc(p, x-2, y));
				connections_b.push_back(xy_to_loc(p, x, y));
			}
		}
	}

    // Endpoints
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
		    intersectionLocations.push_back(xPos);
		    intersectionLocations.push_back(yPos);
		    intersectionFlags.push_back(Flags::IS_ENDPOINT);
        }
    }

	// Dots
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
			if (p.grid[x][y].dot == Cell::Dot::NONE) continue;
			if (x%2 == y%2) continue; // Cells are invalid, intersections are already handled.

            // We need to introduce a new segment -- 
			// Locate the segment we're breaking
			for (int i=0; i<connections_a.size(); i++) {
				auto [x1, y1] = loc_to_xy(p, connections_a[i]);
				auto [x2, y2] = loc_to_xy(p, connections_b[i]);
				if ((x1+1 == x && x2-1 == x && y1 == y && y2 == y) ||
					(y1+1 == y && y2-1 == y && x1 == x && x2 == x)) {
					int other_connection = connections_b[i];
					connections_b[i] = static_cast<int>(intersectionFlags.size()); // This endpoint
					
					connections_a.push_back(other_connection);
					connections_b.push_back(static_cast<int>(intersectionFlags.size())); // This endpoint
					break;
				}
			}
			// Add this dot to the end
			float xPos = min + (x/2.0f) * width_interval;
			float yPos = max - (y/2.0f) * height_interval;
			intersectionLocations.push_back(xPos);
			intersectionLocations.push_back(yPos);

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
            intersectionFlags.push_back(flags);
		}
	}

    // Gaps
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
			if (p.grid[x][y].gap == Cell::Gap::NONE) continue;
			if (x%2 == y%2) continue; // Cells are invalid, intersections are already handled.

			for (int i=0; i<connections_a.size(); i++) {
				auto [x1, y1] = loc_to_xy(p, connections_a[i]);
				auto [x2, y2] = loc_to_xy(p, connections_b[i]);
				if ((x1+1 == x && x2-1 == x && y1 == y && y2 == y) ||
					(y1+1 == y && y2-1 == y && x1 == x && x2 == x)) {
					int other_connection = connections_b[i];
					connections_b[i] = static_cast<int>(intersectionFlags.size()); // This endpoint
					
					connections_a.push_back(other_connection);
					connections_b.push_back(static_cast<int>(intersectionFlags.size() + 1)); // Next endpoint
					break;
				}
			}
            // Add the two halves of this gap to the end
			float xPos = min + (x/2.0f) * width_interval;
			float yPos = max - (y/2.0f) * height_interval;
            // Reminder: Y goes from 0.0 (bottom) to 1.0 (top)
            if (x%2 == 0) { // Vertical gap
			    intersectionLocations.push_back(xPos);
			    intersectionLocations.push_back(yPos + verti_gap_size / 2);
			    intersectionLocations.push_back(xPos);
			    intersectionLocations.push_back(yPos - verti_gap_size / 2);
                intersectionFlags.push_back(Flags::HAS_NO_CONN | Flags::HAS_VERTI_CONN);
                intersectionFlags.push_back(Flags::HAS_NO_CONN | Flags::HAS_VERTI_CONN);
            } else if (y%2 == 0) { // Horizontal gap
			    intersectionLocations.push_back(xPos - horiz_gap_size / 2);
			    intersectionLocations.push_back(yPos);
			    intersectionLocations.push_back(xPos + horiz_gap_size / 2);
			    intersectionLocations.push_back(yPos);
                intersectionFlags.push_back(Flags::HAS_NO_CONN | Flags::HAS_VERTI_CONN);
                intersectionFlags.push_back(Flags::HAS_NO_CONN | Flags::HAS_VERTI_CONN);
            }
		}
	}

	_memory->WritePanelData<int>(id, NUM_DOTS, {static_cast<int>(intersectionFlags.size())});
	_memory->WriteArray<float>(id, DOT_POSITIONS, intersectionLocations);
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

std::tuple<int, int> PuzzleSerializer::loc_to_xy(const Puzzle& p, int location) const {
    int height2 = (p.height - 1) / 2;
    int width2 = (p.width + 1) / 2;

    int x = 2 * (location % width2);
    int y = 2 * (height2 - location / width2);
    return {x, y};
}

int PuzzleSerializer::xy_to_loc(const Puzzle& p, int x, int y) const {
    int height2 = (p.height - 1) / 2;
    int width2 = (p.width + 1) / 2;

    int rowsFromBottom = height2 - y/2;
    return rowsFromBottom * width2 + x/2;
}

std::tuple<int, int> PuzzleSerializer::dloc_to_xy(const Puzzle& p, int location) const {
    int height2 = (p.height - 3) / 2;
    int width2 = (p.width - 1) / 2;

    int x = 2 * (location % width2) + 1;
    int y = 2 * (height2 - location / width2) + 1;
    return {x, y};
}

int PuzzleSerializer::xy_to_dloc(const Puzzle& p, int x, int y) const {
    int height2 = (p.height - 3) / 2;
    int width2 = (p.width - 1) / 2;

    int rowsFromBottom = height2 - (y - 1)/2;
    return rowsFromBottom * width2 + (x - 1)/2;
}

Cell::Dot PuzzleSerializer::FlagsToDot(int flags) const {
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

int PuzzleSerializer::FindConnection(int i, const std::vector<int>& connections_a, const std::vector<int>& connections_b) const {
    for (int j=0; j<connections_a.size(); j++) {
	    if (connections_a[j] == i) return connections_b[j];
	    if (connections_b[j] == i) return connections_a[j];
    }
    return -1;
}
