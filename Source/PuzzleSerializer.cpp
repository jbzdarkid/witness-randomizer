#include "pch.h"
#include "PuzzleSerializer.h"

PuzzleSerializer::PuzzleSerializer(const std::shared_ptr<Memory>& memory) : _memory(memory) {}

Puzzle PuzzleSerializer::ReadPuzzle(int id) {
    Puzzle p;
    try {
        int width = _memory->ReadEntityData<int>(id, GRID_SIZE_X, 1)[0];
        int height = _memory->ReadEntityData<int>(id, GRID_SIZE_Y, 1)[0];
        if (width == 0) width = height;
        if (height == 0) height = width;
        if (width < 0 || height < 0) return p; // @Error: Grid size should be always positive? Looks like the starting panels break this rule, though.

        _numGridLocations = width * height; // Highest location which represents a gridded intersection
        _numIntersections = _memory->ReadEntityData<int>(id, NUM_DOTS, 1)[0];
        _intersectionFlags = _memory->ReadArray<int>(id, DOT_FLAGS, _numIntersections);
        int numConnections = _memory->ReadEntityData<int>(id, NUM_CONNECTIONS, 1)[0];
        _connectionsA = _memory->ReadArray<int>(id, DOT_CONNECTION_A, numConnections);
        _connectionsB = _memory->ReadArray<int>(id, DOT_CONNECTION_B, numConnections);
        _intersectionLocations = _memory->ReadArray<float>(id, DOT_POSITIONS, _numIntersections*2);

        p.NewGrid(width - 1, height - 1);
        ReadIntersections(p);
        ReadExtras(p);
        ReadDecorations(p, id);
        ReadSequence(p, id);
        ReadSymmetry(p, id);
    } catch (MemoryException exc) {
        MemoryException::HandleException(exc);
    }
    return p;
}

void PuzzleSerializer::WritePuzzle(const Puzzle& p, int id) {
    try {
        _intersectionFlags.clear();
        _connectionsA.clear();
        _connectionsB.clear();
        _intersectionLocations.clear();
        _extraLocations.clear();

        MIN = 0.1f;
        MAX = 0.9f;

        if (p.height > p.width) {
            INTERVAL = (MAX - MIN) / (p.height - (p.height%2));
            X_OFF = (p.height - p.width) / 2;
            Y_OFF = 0;
        } else {
            INTERVAL = (MAX - MIN) / (p.width - (p.width%2));
            X_OFF = 0;
            Y_OFF = (p.width - p.height) / 2;
        }
    
        WriteIntersections(p);
        WriteEndpoints(p);
        WriteDots(p);
        WriteGaps(p);
        WriteDecorations(p, id);
        WriteSequence(p, id);
        WriteSymmetry(p, id);

        _memory->WriteEntityData<int>(id, GRID_SIZE_X, {(p.width + 1)/2});
        _memory->WriteEntityData<int>(id, GRID_SIZE_Y, {(p.height + 1)/2});
        _memory->WriteEntityData<int>(id, NUM_DOTS, {static_cast<int>(_intersectionFlags.size())});
        _memory->WriteArray<float>(id, DOT_POSITIONS, _intersectionLocations);
        _memory->WriteArray<int>(id, DOT_FLAGS, _intersectionFlags);
        _memory->WriteEntityData<int>(id, NUM_CONNECTIONS, {static_cast<int>(_connectionsA.size())});
        _memory->WriteArray<int>(id, DOT_CONNECTION_A, _connectionsA);
        _memory->WriteArray<int>(id, DOT_CONNECTION_B, _connectionsB);
        _memory->WriteEntityData<int>(id, NEEDS_REDRAW, {1});
    } catch (MemoryException exc) {
        MemoryException::HandleException(exc);
    }
}

void PuzzleSerializer::ReadIntersections(Puzzle& p) {
    // @Cleanup: Just change the defaults, instead of this?
    // Mark every edge as a full gap
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
            if (x%2 == y%2) continue;
            p.grid[x][y].gap = Cell::Gap::FULL;
        }
    }

    // Iterate all connections (that are in the grid) to see which edges are connected.
    for (int i=0; i<_connectionsA.size(); i++) {
        int locationA = _connectionsA[i];
        int locationB = _connectionsB[i];
        if (locationA > locationB) std::swap(locationA, locationB); // A < B
        if (locationB >= _numGridLocations) continue; // Connection goes to a non-grid location

        float x1 = _intersectionLocations[2*locationA];
        float y1 = _intersectionLocations[2*locationA+1];
        float x2 = _intersectionLocations[2*locationB];
        float y2 = _intersectionLocations[2*locationB+1];
        auto [x, y] = loc_to_xy(p, locationA);

             if (x1 < x2) x++;
        else if (x1 > x2) x--;
        else if (y1 < y2) y--;
        else if (y1 > y2) y++;
        p.grid[x][y].gap = Cell::Gap::NONE;
    }
}

void PuzzleSerializer::ReadExtras(Puzzle& p) {
    // This iterates left-right, bottom-top
    int i = 0;
    for (; i < _numGridLocations; i++) {
        int flags = _intersectionFlags[i];
        auto [x, y] = loc_to_xy(p, i);
        if (flags & Flags::IS_STARTPOINT) {
            p.grid[x][y].start = true;
        }
        p.grid[x][y].dot = FlagsToDot(flags);
        if (flags & Flags::HAS_NO_CONN) {
            p.grid[x][y].gap = Cell::Gap::FULL;
        }
    }

    // Maps "extra gap intersection location" -> grid location. Note that there should be two locations for each position.
    std::unordered_map<int, Pos> gapLocations;

    // Iterate the remaining intersections (endpoints, dots, gaps)
    for (; i < _numIntersections; i++) {
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
            else assert(false);
        } else if (_intersectionFlags[i] & Flags::HAS_DOT) {
                 if (x1 < x2) x--;
            else if (x1 > x2) x++;
            else if (y1 < y2) y++;
            else if (y1 > y2) y--;
            else assert(false);
            p.grid[x][y].dot = FlagsToDot(_intersectionFlags[i]);
        } else if (_intersectionFlags[i] & Flags::HAS_ONE_CONN) {
                 if (x1 < x2) x--;
            else if (x1 > x2) x++;
            else if (y1 < y2) y++;
            else if (y1 > y2) y--;
            else assert(false);
            p.grid[x][y].gap = Cell::Gap::BREAK;
            gapLocations[i] = Pos{x, y};
        }
    }

    // Fixups for asymmetrical gaps
    for (int i=0; i<_connectionsA.size(); i++) {
        // Only consider connections to non-grid locations
        int locationA = _connectionsA[i];
        if (locationA < _numGridLocations) continue;
        int locationB = _connectionsB[i];
        if (locationB < _numGridLocations) continue;

        Pos pos = gapLocations[locationA];
        if (pos == gapLocations[locationB]) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::NONE;
        }
    }
}

void PuzzleSerializer::ReadDecorations(Puzzle& p, int id) {
    int numDecorations = _memory->ReadEntityData<int>(id, NUM_DECORATIONS, 1)[0];
    if (numDecorations == 0) return;
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
    int sequenceLength = _memory->ReadEntityData<int>(id, SEQUENCE_LEN, 1)[0];
    if (sequenceLength == 0) return;
    std::vector<int> sequence = _memory->ReadArray<int>(id, SEQUENCE, sequenceLength);

    for (int location : sequence) {
        p.sequence.emplace_back(loc_to_xy(p, location));
    }
}

void PuzzleSerializer::ReadSymmetry(Puzzle& p, int id) {
    int hasSymmetry = _memory->ReadEntityData<int>(id, REFLECTION_DATA, 1)[0];
    if (hasSymmetry == 0) return; // Array is null, no puzzle symmetry

    std::vector<int> reflectionData = _memory->ReadArray<int>(id, REFLECTION_DATA, _numIntersections);
    Pos p1 = loc_to_xy(p, reflectionData[0]);
    Pos p2 = loc_to_xy(p, reflectionData[reflectionData[0]]);
    if (p1.x != p2.x) {
        p.symmetry = Puzzle::Symmetry::X;
    } else if (p1.y != p2.y) {
        p.symmetry = Puzzle::Symmetry::Y;
    } else {
        p.symmetry = Puzzle::Symmetry::XY;
    }
}

void PuzzleSerializer::WriteIntersections(const Puzzle& p) {
    // @Cleanup: If I write directly to locations, then I can simplify this gross loop iterator.
    // Use _numGridIntersections computation: = (p.width / 2 + 1) * (p.height / 2 + 1);
    // Grided intersections
    for (int y=p.height-1; y>=0; y-=2) {
        for (int x=0; x<p.width; x+=2) {
            int flags = 0;
            if (p.grid[x][y].start) {
                flags |= Flags::IS_STARTPOINT;
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
            // Create connections for this intersection for top/left only.
            // Top connection
            if (y > 0 && p.grid[x][y-1].gap != Cell::Gap::FULL) {
                _connectionsA.push_back(xy_to_loc(p, x, y-2));
                _connectionsB.push_back(xy_to_loc(p, x, y));
                flags |= Flags::HAS_VERTI_CONN;
                numConnections++;
            }
            // Bottom connection
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
            if (numConnections == 0) flags |= HAS_NO_CONN;
            if (numConnections == 1) flags |= HAS_ONE_CONN;

            auto [xPos, yPos] = xy_to_pos(p, x, y);
            AddIntersection(p, x, y, xPos, yPos, flags);
        }
    }
}

void PuzzleSerializer::WriteEndpoints(const Puzzle& p) {
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
            if (p.grid[x][y].end == Cell::Dir::NONE) continue;
            _connectionsA.push_back(xy_to_loc(p, x, y));
            _connectionsB.push_back(static_cast<int>(_intersectionFlags.size()));

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
            AddIntersection(p, x, y, xPos, yPos, Flags::IS_ENDPOINT);
        }
    }
}

void PuzzleSerializer::WriteDots(const Puzzle& p) {
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
            if (x%2 == y%2) continue; // Cells are invalid, intersections are already handled.

            bool shouldWriteDot = false;
            if (p.grid[x][y].dot != Cell::Dot::NONE) {
                shouldWriteDot = true;
            } else if (p.symmetry != Puzzle::Symmetry::NONE) {
                Pos sym = p.GetSymmetricalPos(x, y);
                // Write symmetrical dots, but don't actually set the flag for them. They're only there for symmetrical tracing.
                if (p.grid[sym.x][sym.y].dot != Cell::Dot::NONE) {
                    shouldWriteDot = true;
                }
            }
            if (!shouldWriteDot) continue;

            // We need to introduce a new segment which contains this dot. Break the existing segment, and add one.
            int connectionLocation = -1;
            for (int i=0; i<_connectionsA.size(); i++) {
                auto [x1, y1] = loc_to_xy(p, _connectionsA[i]);
                auto [x2, y2] = loc_to_xy(p, _connectionsB[i]);
                if ((x1+1 == x && x2-1 == x && y1 == y && y2 == y) ||
                    (y1+1 == y && y2-1 == y && x1 == x && x2 == x)) {
                    connectionLocation = i;
                    break;
                }
            }
            if (connectionLocation == -1) continue; // @Error

            // @Assume: B > A for connections. To remove, add the horiz/verti check, see gaps.
            int other_connection = _connectionsB[connectionLocation];
            _connectionsB[connectionLocation] = static_cast<int>(_intersectionFlags.size());
            _connectionsA.push_back(other_connection);
            _connectionsB.push_back(static_cast<int>(_intersectionFlags.size()));

            int flags = 0;
            if (p.symmetry != Puzzle::Symmetry::NONE && p.grid[x][y].dot == Cell::Dot::NONE) {
                // A dot was asked to be introduced strictly for tracing reasons, don't set any flags.
            } else {
                flags |= Flags::HAS_DOT;
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
            }

            auto [xPos, yPos] = xy_to_pos(p, x, y);
            AddIntersection(p, x, y, xPos, yPos, flags);
        }
    }
}

void PuzzleSerializer::WriteGaps(const Puzzle& p) {
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
            if (x%2 == y%2) continue; // Cells are invalid, intersections are already handled.

            bool shouldWriteGap = false;
            if (p.grid[x][y].gap == Cell::Gap::BREAK) {
                shouldWriteGap = true;
            } else if (p.symmetry != Puzzle::Symmetry::NONE) {
                Pos sym = p.GetSymmetricalPos(x, y);
                // Write symmetrical gaps, but also add an extra connection so they don't look like a gap.
                if (p.grid[sym.x][sym.y].gap == Cell::Gap::BREAK) {
                    shouldWriteGap = true;
                }
            }
            if (!shouldWriteGap) continue;

            // We need to introduce a new segment which contains this dot. Break the existing segment, and add one.
            int connectionLocation = -1;
            for (int i=0; i<_connectionsA.size(); i++) {
                auto [x1, y1] = loc_to_xy(p, _connectionsA[i]);
                auto [x2, y2] = loc_to_xy(p, _connectionsB[i]);
                if ((x1+1 == x && x2-1 == x && y1 == y && y2 == y) ||
                    (y1+1 == y && y2-1 == y && x1 == x && x2 == x)) {
                    connectionLocation = i;
                    break;
                }
            }
            if (connectionLocation == -1) continue; // @Error

            int gap1Location, gap2Location;
            auto [xPos, yPos] = xy_to_pos(p, x, y);
            // Reminder: Y goes from 0.0 (bottom) to 1.0 (top)
            if (x%2 == 0) { // Vertical gap
                gap1Location = static_cast<int>(_intersectionFlags.size());
                _connectionsA[connectionLocation] = xy_to_loc(p, x, y-1);
                _connectionsB[connectionLocation] = gap1Location;
                AddIntersection(p, x, y, xPos, yPos + INTERVAL / 2, Flags::HAS_ONE_CONN | Flags::HAS_VERTI_CONN);

                gap2Location = static_cast<int>(_intersectionFlags.size());
                _connectionsA.push_back(xy_to_loc(p, x, y+1));
                _connectionsB.push_back(gap2Location);
                AddIntersection(p, x, y, xPos, yPos - INTERVAL / 2, Flags::HAS_ONE_CONN | Flags::HAS_VERTI_CONN);
            } else if (y%2 == 0) { // Horizontal gap
                gap1Location = static_cast<int>(_intersectionFlags.size());
                _connectionsA[connectionLocation] = xy_to_loc(p, x-1, y);
                _connectionsB[connectionLocation] = gap1Location;
                AddIntersection(p, x, y, xPos - INTERVAL / 2, yPos, Flags::HAS_ONE_CONN | Flags::HAS_HORIZ_CONN);

                gap2Location = static_cast<int>(_intersectionFlags.size());
                _connectionsA.push_back(xy_to_loc(p, x+1, y));
                _connectionsB.push_back(gap2Location);
                AddIntersection(p, x, y, xPos + INTERVAL / 2, yPos, Flags::HAS_ONE_CONN | Flags::HAS_HORIZ_CONN);
            }
            if (p.symmetry != Puzzle::Symmetry::NONE && p.grid[x][y].gap == Cell::Gap::NONE) {
                // A gap was asked to be introduced strictly for tracing reasons, but it shouldn't look like a gap.
                // Add a connection between two halves of the gap to cover it graphically.
                _connectionsA.push_back(gap1Location);
                _connectionsB.push_back(gap2Location);
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

    _memory->WriteEntityData<int>(id, NUM_DECORATIONS, {static_cast<int>(decorations.size())});
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

    // TODO: Orphaned code?
    // Pos endpoint = p.sequence[p.sequence.size() - 1];
    // int location = extra_xy_to_loc(p, endpoint.x, endpoint.y);

    _memory->WriteEntityData<int>(id, SEQUENCE_LEN, {static_cast<int>(sequence.size())});
    _memory->WriteNewArray<int>(id, SEQUENCE, sequence);
}

void PuzzleSerializer::WriteSymmetry(const Puzzle& p, int id) {
    if (p.symmetry == Puzzle::Symmetry::NONE) {
        _memory->WriteEntityData<int>(id, REFLECTION_DATA, {0});
        return;
    }

    std::vector<int> reflectionData;
    reflectionData.resize(_intersectionFlags.size());

    // Wow, what a horrible solution. But hey, whatever, if it works.
    for (int x=0; x<p.width; x+=2) {
        for (int y=0; y<p.height; y+=2) {
            Pos sym = p.GetSymmetricalPos(x, y);
            int location = xy_to_loc(p, x, y);
            int symLocation = xy_to_loc(p, sym.x, sym.y);
            reflectionData[location] = symLocation;
            reflectionData[symLocation] = location;
            if (p.grid[x][y].end != Cell::Dir::NONE) {
                location = extra_xy_to_loc(p, x, y);
                Pos sym = p.GetSymmetricalPos(x, y);
                symLocation = extra_xy_to_loc(p, sym.x, sym.y);
                reflectionData[location] = symLocation; // @Assume the symmetrical endpoint will write the other pair
            }
        }
    }

    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
            if (x%2 == y%2) continue;
            if (p.grid[x][y].gap == Cell::Gap::BREAK) {
                Pos sym = p.GetSymmetricalPos(x, y);
                int location = extra_xy_to_loc(p, x, y);
                int symLocation = extra_xy_to_loc(p, sym.x, sym.y);
                // Each gap results in two intersections, @Assume they're written consecutively

                if ((x%2 != 0 && p.symmetry & Puzzle::Symmetry::X) || 
                    (y%2 != 0 && p.symmetry & Puzzle::Symmetry::Y)) {
                    // Write data inverted, because it's being reflected
                    reflectionData[location] = symLocation-1;
                    reflectionData[location-1] = symLocation;
                    reflectionData[symLocation] = location-1;
                    reflectionData[symLocation-1] = location;
                } else { // Write data normally
                    reflectionData[location] = symLocation;
                    reflectionData[location-1] = symLocation-1;
                    reflectionData[symLocation] = location;
                    reflectionData[symLocation-1] = location-1;
                }
            } else if (p.grid[x][y].dot == Cell::Dot::BLACK) {
                Pos sym = p.GetSymmetricalPos(x, y);
                int location = extra_xy_to_loc(p, x, y);
                int symLocation = extra_xy_to_loc(p, sym.x, sym.y);
                reflectionData[location] = symLocation;
                reflectionData[symLocation] = location;
            }
        }
    }

    _memory->WriteArray<int>(id, REFLECTION_DATA, reflectionData);
}

std::tuple<int, int> PuzzleSerializer::loc_to_xy(const Puzzle& p, int location) const {
    int height2 = (p.height - 1) / 2;
    int width2 = (p.width + 1) / 2;

    int x = 2 * (location % width2);
    int y = 2 * (height2 - location / width2);
    assert(x >= 0 && x < p.width);
    assert(y >= 0 && y < p.height);
    return {x, y};
}

int PuzzleSerializer::xy_to_loc(const Puzzle& p, int x, int y) const {
    assert(x%2 == 0);
    assert(y%2 == 0);
    int height2 = (p.height - 1) / 2;
    int width2 = (p.width + 1) / 2;

    int rowsFromBottom = height2 - y/2;
    return rowsFromBottom * width2 + x/2;
}

int PuzzleSerializer::extra_xy_to_loc(const Puzzle& p, int x, int y) const {
    auto search = _extraLocations.find(x * p.height + y);
    if (search == _extraLocations.end()) {
        assert(false);
        return -1; // @Error
    }
    return search->second;
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
        MIN + (x + X_OFF) * INTERVAL,
        MAX - (y + Y_OFF) * INTERVAL
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

void PuzzleSerializer::AddIntersection(const Puzzle& p, int x, int y, float xPos, float yPos, int flags) {
    _extraLocations[x * p.height + y] = static_cast<int>(_intersectionFlags.size());
    _intersectionLocations.push_back(xPos);
    _intersectionLocations.push_back(yPos);
    _intersectionFlags.push_back(flags);
}
