#include "PuzzlerSerializer.h"
#include "Memory.h"

#pragma warning (disable:26451)
#pragma warning (disable:26812)

PuzzleSerializer::PuzzleSerializer(const std::shared_ptr<Memory>& memory) : _memory(memory) {}

Puzzle PuzzleSerializer::ReadPuzzle(int id) {
    int width = 2 * _memory->ReadPanelData<int>(id, GRID_SIZE_X, 1)[0] - 1;
    int height = 2 * _memory->ReadPanelData<int>(id, GRID_SIZE_Y, 1)[0] - 1;
    if (width < 0 || height < 0) return Puzzle(); // @Error: Grid size should be always positive? Looks like the starting panels break this rule, though.

	int numIntersections = _memory->ReadPanelData<int>(id, NUM_DOTS, 1)[0];
	_intersectionFlags = _memory->ReadArray<int>(id, DOT_FLAGS, numIntersections);
	int numConnections = _memory->ReadPanelData<int>(id, NUM_CONNECTIONS, 1)[0];
	_connectionsA = _memory->ReadArray<int>(id, DOT_CONNECTION_A, numConnections);
	_connectionsB = _memory->ReadArray<int>(id, DOT_CONNECTION_B, numConnections);
	_intersectionLocations = _memory->ReadArray<float>(id, DOT_POSITIONS, numIntersections*2);

    Puzzle p;
    p.NewGrid(width, height);
	ReadIntersections(p);
    ReadExtras(p);
	ReadDecorations(p, id);
    ReadSequence(p, id);
    return p;
}

void PuzzleSerializer::WritePuzzle(const Puzzle& p, int id) {
    _intersectionFlags.clear();
    _connectionsA.clear();
    _connectionsB.clear();
    _intersectionLocations.clear();

    MIN = 0.1f;
	MAX = 0.9f;
	WIDTH_INTERVAL = (MAX - MIN) / (p.width/2);
	HEIGHT_INTERVAL = (MAX - MIN) / (p.height/2);
    HORIZ_GAP_SIZE = WIDTH_INTERVAL / 2;
    VERTI_GAP_SIZE = HEIGHT_INTERVAL / 2;

	WriteIntersections(p);
    WriteEndpoints(p);
    WriteDecorations(p, id);
    WriteSequence(p, id);

	_memory->WritePanelData<int>(id, GRID_SIZE_X, {(p.width + 1)/2});
	_memory->WritePanelData<int>(id, GRID_SIZE_Y, {(p.height + 1)/2});
	_memory->WritePanelData<int>(id, NUM_DOTS, {static_cast<int>(_intersectionFlags.size())});
	_memory->WriteArray<float>(id, DOT_POSITIONS, _intersectionLocations);
	_memory->WriteArray<int>(id, DOT_FLAGS, _intersectionFlags);
	_memory->WritePanelData<int>(id, NUM_CONNECTIONS, {static_cast<int>(_connectionsA.size())});
	_memory->WriteArray<int>(id, DOT_CONNECTION_A, _connectionsA);
	_memory->WriteArray<int>(id, DOT_CONNECTION_B, _connectionsB);
	_memory->WritePanelData<int>(id, NEEDS_REDRAW, {1});
}

void PuzzleSerializer::ReadIntersections(Puzzle& p) {
    // @Cleanup: Change defaults?
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
            if (x%2 == y%2) continue;
            p.grid[x][y].gap = Cell::Gap::FULL;
        }
    }

    for (int j=0; j<_intersectionFlags.size(); j++) {
        if (_intersectionFlags[_connectionsA[j]] & Flags::IS_ENDPOINT) break;
        if (_intersectionFlags[_connectionsB[j]] & Flags::IS_ENDPOINT) break;
		float x1 = _intersectionLocations[2*_connectionsA[j]];
		float y1 = _intersectionLocations[2*_connectionsA[j]+1];
		float x2 = _intersectionLocations[2*_connectionsB[j]];
		float y2 = _intersectionLocations[2*_connectionsB[j]+1];
        auto [x, y] = loc_to_xy(p, _connectionsA[j]);
        
			 if (x1 < x2) x++;
		else if (x1 > x2) x--;
		else if (y1 < y2) y--;
		else if (y1 > y2) y++;
        p.grid[x][y].gap = Cell::Gap::NONE;
    }
}

void PuzzleSerializer::ReadExtras(Puzzle& p) {
    // This iterates bottom-top, left-right
	int i = 0;
	for (; i < _intersectionFlags.size(); i++) {
        int flags = _intersectionFlags[i];
		auto [x, y] = loc_to_xy(p, i);
		if (y < 0) break; // This is the expected exit point
		if (flags & Flags::IS_STARTPOINT) {
            p.grid[x][y].start = true;
		}
        p.grid[x][y].dot = FlagsToDot(flags);
        if (flags & Flags::IS_FULL_GAP) {
            p.grid[x][y].gap = Cell::Gap::FULL;
        }
	}

	// Iterate the remaining intersections (endpoints, dots, gaps)
	for (; i < _intersectionFlags.size(); i++) {
        int location = FindConnection(i);
		if (location == -1) continue; // @Error: Unable to find connection point
        // (x1, y1) location of this intersection
        // (x2, y2) location of the connected intersection
		float x1 = _intersectionLocations[2*i];
		float y1 = _intersectionLocations[2*i+1];
		float x2 = _intersectionLocations[2*location];
		float y2 = _intersectionLocations[2*location+1];
        auto [x, y] = loc_to_xy(p, location);

		if (_intersectionFlags[i] & Flags::IS_ENDPOINT) {
            // Our x coordinate is less than the target's
			     if (x1 < x2) p.grid[x][y].end = Cell::Dir::LEFT;
			else if (x1 > x2) p.grid[x][y].end = Cell::Dir::RIGHT;
            // Note that Y coordinates are reversed: 0.0 (bottom) 1.0 (top)
			else if (y1 < y2) p.grid[x][y].end = Cell::Dir::DOWN;
            else if (y1 > y2) p.grid[x][y].end = Cell::Dir::UP;
		} else if (_intersectionFlags[i] & Flags::HAS_DOT) {
			     if (x1 < x2) x--;
			else if (x1 > x2) x++;
			else if (y1 < y2) y++;
			else if (y1 > y2) y--;
            p.grid[x][y].dot = FlagsToDot(_intersectionFlags[i]);
        } else if (_intersectionFlags[i] & Flags::HAS_ONE_CONN) {
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
        d->type = static_cast<Type>(decorations[i] & 0xFF00);
        switch(d->type) {
            case Type::Poly:
            case Type::RPoly:
            case Type::Ylop:
                d->polyshape = decorations[i] & 0xFFFF0000;
                break;
            case Type::Triangle:
                d->count = decorations[i] & 0x000F0000;
                break;
        }
        d->color = static_cast<Color>(decorations[i] & 0xF);
	}
}

void PuzzleSerializer::ReadSequence(Puzzle& p, int id) {
    int sequenceLength = _memory->ReadPanelData<int>(id, SEQUENCE_LEN, 1)[0];
    std::vector<int> sequence = _memory->ReadArray<int>(id, SEQUENCE, sequenceLength);

    for (int location : sequence) {
        auto [x, y] = loc_to_xy(p, location);
        p.sequence.emplace_back(Pos{x, y});
    }
}

void PuzzleSerializer::WriteIntersections(const Puzzle& p) {
	// @Cleanup: If I write directly to locations, then I can simplify this gross loop iterator.
    // int numIntersections = (p.width / 2 + 1) * (p.height / 2 + 1);
    // Grided intersections
	for (int y=p.height-1; y>=0; y-=2) {
		for (int x=0; x<p.width; x+=2) {
			_intersectionLocations.push_back(MIN + (x/2) * WIDTH_INTERVAL);
			_intersectionLocations.push_back(MAX - (y/2) * HEIGHT_INTERVAL);
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

            int numConnections = 0;
            if (p.grid[x][y].end != Cell::Dir::NONE) numConnections++;
			// Create connections for this intersection for bottom/left only.
            // Bottom connection
			if (y > 0 && p.grid[x][y-1].gap != Cell::Gap::FULL) {
				_connectionsA.push_back(xy_to_loc(p, x, y-2));
				_connectionsB.push_back(xy_to_loc(p, x, y));
                flags |= Flags::HAS_VERTI_CONN;
                numConnections++;
			}
            // Top connection
            if (y < p.height - 1 && p.grid[x][y+1].gap != Cell::Gap::FULL) {
                flags |= Flags::HAS_VERTI_CONN;
                numConnections++;
            }
            // Left connection
			if (x > 0 && p.grid[x-1][y].gap != Cell::Gap::FULL) {
				_connectionsA.push_back(xy_to_loc(p, x-2, y));
				_connectionsB.push_back(xy_to_loc(p, x, y));
                flags |= Flags::HAS_HORIZ_CONN;
                numConnections++;
			}
            // Right connection
            if (x < p.width - 1 && p.grid[x+1][y].gap != Cell::Gap::FULL) {
                flags |= Flags::HAS_HORIZ_CONN;
                numConnections++;
            }
            if (numConnections == 1) flags |= HAS_ONE_CONN;
			_intersectionFlags.push_back(flags);
		}
	}
}

void PuzzleSerializer::WriteEndpoints(const Puzzle& p) {
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
            if (p.grid[x][y].end == Cell::Dir::NONE) continue;
            _connectionsA.push_back(xy_to_loc(p, x, y)); // Target to connect to
		    _connectionsB.push_back(static_cast<int>(_intersectionFlags.size())); // This endpoint

            auto [xPos, yPos] = xy_to_pos(p, x, y);
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
		    _intersectionLocations.push_back(xPos);
		    _intersectionLocations.push_back(yPos);
		    _intersectionFlags.push_back(Flags::IS_ENDPOINT);
        }
    }
}

void PuzzleSerializer::WriteDots(const Puzzle& p) {
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
			if (x%2 == y%2) continue; // Cells are invalid, intersections are already handled.
			if (p.grid[x][y].dot == Cell::Dot::NONE) continue;

            // We need to introduce a new segment -- 
			// Locate the segment we're breaking
			for (int i=0; i<_connectionsA.size(); i++) {
				auto [x1, y1] = loc_to_xy(p, _connectionsA[i]);
				auto [x2, y2] = loc_to_xy(p, _connectionsB[i]);
				if ((x1+1 == x && x2-1 == x && y1 == y && y2 == y) ||
					(y1+1 == y && y2-1 == y && x1 == x && x2 == x)) {
					int other_connection = _connectionsB[i];
					_connectionsB[i] = static_cast<int>(_intersectionFlags.size()); // This endpoint
					
					_connectionsA.push_back(other_connection);
					_connectionsB.push_back(static_cast<int>(_intersectionFlags.size())); // This endpoint
					break;
				}
			}
			// Add this dot to the end
            auto [xPos, yPos] = xy_to_pos(p, x, y);
			_intersectionLocations.push_back(xPos);
			_intersectionLocations.push_back(yPos);

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
            _intersectionFlags.push_back(flags);
		}
	}
}

void PuzzleSerializer::WriteGaps(const Puzzle& p) {
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
			if (x%2 == y%2) continue; // Cells are invalid, intersections are already handled.
			if (p.grid[x][y].gap != Cell::Gap::BREAK) continue;

            auto [xPos, yPos] = xy_to_pos(p, x, y);
            // Reminder: Y goes from 0.0 (bottom) to 1.0 (top)
            if (x%2 == 0) { // Vertical gap
                _connectionsA.push_back(xy_to_loc(p, x, y-1));
                _connectionsB.push_back(static_cast<int>(_intersectionFlags.size())); // This endpoint
			    _intersectionLocations.push_back(xPos);
			    _intersectionLocations.push_back(yPos + VERTI_GAP_SIZE / 2);
                _intersectionFlags.push_back(Flags::HAS_ONE_CONN | Flags::HAS_VERTI_CONN);

                _connectionsA.push_back(xy_to_loc(p, x, y+1));
                _connectionsB.push_back(static_cast<int>(_intersectionFlags.size())); // This endpoint
			    _intersectionLocations.push_back(xPos);
			    _intersectionLocations.push_back(yPos - VERTI_GAP_SIZE / 2);
                _intersectionFlags.push_back(Flags::HAS_ONE_CONN | Flags::HAS_VERTI_CONN);
            } else if (y%2 == 0) { // Horizontal gap
                _connectionsA.push_back(xy_to_loc(p, x-1, y));
                _connectionsB.push_back(static_cast<int>(_intersectionFlags.size())); // This endpoint
			    _intersectionLocations.push_back(xPos - HORIZ_GAP_SIZE / 2);
			    _intersectionLocations.push_back(yPos);
                _intersectionFlags.push_back(Flags::HAS_ONE_CONN | Flags::HAS_HORIZ_CONN);

                _connectionsA.push_back(xy_to_loc(p, x+1, y));
                _connectionsB.push_back(static_cast<int>(_intersectionFlags.size())); // This endpoint
			    _intersectionLocations.push_back(xPos + HORIZ_GAP_SIZE / 2);
			    _intersectionLocations.push_back(yPos);
                _intersectionFlags.push_back(Flags::HAS_ONE_CONN | Flags::HAS_HORIZ_CONN);
            }
		}
	}
}

void PuzzleSerializer::WriteDecorations(const Puzzle& p, int id) {
    if (!p.hasDecorations) return;

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

void PuzzleSerializer::WriteSequence(const Puzzle& p, int id) {
    if (p.sequence.size() == 0) return;

    std::vector<int> sequence;
    for (Pos pos : p.sequence) {
        // Only include intersections, the game does not treat segments as real objects
        if (pos.x%2 == 0 && pos.y%2 == 0) {
            sequence.emplace_back(xy_to_loc(p, pos.x, pos.y));
        }
    }

    _memory->WritePanelData<int>(id, SEQUENCE_LEN, {static_cast<int>(sequence.size())});
    _memory->WriteNewArray<int>(id, SEQUENCE, sequence);
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

std::tuple<float, float> PuzzleSerializer::xy_to_pos(const Puzzle& p, int x, int y) const {
	return {
        MIN + (x/2) * WIDTH_INTERVAL,
        MAX - (y/2) * HEIGHT_INTERVAL
    };
}

Cell::Dot PuzzleSerializer::FlagsToDot(int flags) const {
    if (!(flags & Flags::HAS_DOT)) return Cell::Dot::NONE;
    if (flags & Flags::DOT_IS_BLUE) return Cell::Dot::BLUE;
    else if (flags & Flags::DOT_IS_ORANGE) return Cell::Dot::YELLOW;
    else if (flags & Flags::DOT_IS_INVISIBLE) return Cell::Dot::INVISIBLE;
    else return Cell::Dot::BLACK;
}

int PuzzleSerializer::FindConnection(int location) const {
    for (int j=0; j<_connectionsA.size(); j++) {
	    if (_connectionsA[j] == location) return _connectionsB[j];
	    if (_connectionsB[j] == location) return _connectionsA[j];
    }
    return -1;
}
