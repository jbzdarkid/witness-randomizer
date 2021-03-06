#include "pch.h"
#include "Randomizer2.h"
#include "PuzzleSerializer.h"
#include "Randomizer2Core.h"
#include "Random.h"
#include "Solver.h"
#include "Windows.h"

Randomizer2::Randomizer2(const PuzzleSerializer& serializer) : _serializer(serializer) {
}

void Randomizer2::Randomize() {
    // RandomizeTutorial();
    // RandomizeGlassFactory();
    RandomizeSymmetryIsland();
    // RandomizeKeep();
}

void Randomizer2::RandomizeTutorial() {
    { // Far center
        Puzzle p;
        p.NewGrid(4, 4);
        p.grid[0][8].start = true;
        p.grid[8][0].end = Cell::Dir::UP;

        for (Pos pos : Randomizer2Core::CutEdges(p, 14)) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }
        _serializer.WritePuzzle(p, 0x293);
    }

    { // Center left
        Puzzle p;
        p.NewGrid(6, 6);

        int x = Random::RandInt(0, (p.width-1)/2)*2;
        int y = Random::RandInt(0, (p.height-1)/2)*2;
        int rng = Random::RandInt(1, 4);
             if (rng == 1) p.grid[x][0].end = Cell::Dir::UP;
        else if (rng == 2) p.grid[x][p.height-1].end = Cell::Dir::DOWN;
        else if (rng == 3) p.grid[0][y].end = Cell::Dir::LEFT;
        else if (rng == 4) p.grid[p.width-1][y].end = Cell::Dir::RIGHT;

        // [4/6/8][4/6/8]
        p.grid[Random::RandInt(0, 2)*2 + 4][Random::RandInt(0, 2)*2 + 4].start = true;
    
        for (Pos pos : Randomizer2Core::CutEdges(p, 35)) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }

        _serializer.WritePuzzle(p, 0x295);
    }

    { // Far left
        Puzzle p;
        p.NewGrid(10, 10);

        p.grid[0][20].start = true;
        p.grid[20][0].end = Cell::Dir::RIGHT;

        for (Pos pos : Randomizer2Core::CutEdges(p, 96)) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }
        _serializer.WritePuzzle(p, 0x2C2);
    }

    { // Back left
        Puzzle p;
        p.NewGrid(6, 6);

        p.grid[0][12].start = true;
        p.grid[12][0].end = Cell::Dir::RIGHT;
        p.grid[12][12].end = Cell::Dir::RIGHT;

        for (Pos pos : Randomizer2Core::CutEdges(p, 27)) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
        }
        _serializer.WritePuzzle(p, 0xA3B5);
    }

    { // Back right
        Puzzle p;
        p.NewGrid(6, 6);

        p.grid[0][12].start = true;
        p.grid[12][12].start = true;
        p.grid[6][0].end = Cell::Dir::UP;

        // @Cleanup
        std::vector<Pos> cuts;
        bool toTheRight;
        // Start by generating a cut line, to ensure one of the two startpoints is inaccessible
        int x, y;
        switch (Random::RandInt(1, 4)) {
            case 1:
                x = 1; y = 1;
                toTheRight = true;
                cuts.emplace_back(0, 1);
                break;
            case 2:
                x = 1; y = 1;
                toTheRight = true;
                cuts.emplace_back(1, 0);
                break;
            case 3:
                x = 11; y = 1;
                toTheRight = false;
                cuts.emplace_back(12, 1);
                break;
            case 4:
                x = 11; y = 1;
                toTheRight = false;
                cuts.emplace_back(11, 0);
                break;
        }
        while (y < p.height) { // The final cut will push y below the bottom of the puzzle, which means we're done.
            switch (Random::RandInt(1, 4)) {
                case 1: // Go right
                    if (x < p.width-2) {
                        cuts.emplace_back(x+1, y);
                        x += 2;
                    }
                    break;
                case 2: // Go left
                    if (x > 1) {
                        cuts.emplace_back(x-1, y);
                        x -= 2;
                    }
                    break;
                case 3: 
                case 4: // Go down (biased x2)
                    cuts.emplace_back(x, y+1);
                    y += 2;
                    break;
            }
        }

        for (Pos pos : cuts) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
        }

        for (Pos pos : Randomizer2Core::CutEdges(p, 30 - cuts.size())) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
        }
        _serializer.WritePuzzle(p, 0xA3B2);
    }
}

void Randomizer2::RandomizeGlassFactory() {
    { // Back wall 1
        Puzzle p;
        p.NewGrid(3, 3);
        p.symmetry = Puzzle::Symmetry::X;
        p.grid[0][6].start = true;
        p.grid[6][6].start = true;
        p.grid[2][0].end = Cell::Dir::UP;
        p.grid[4][0].end = Cell::Dir::UP;

        std::vector<Pos> cutEdges = Randomizer2Core::CutSymmetricalEdgePairs(p, 2);
        for (Pos pos : cutEdges) {
            Pos sym = p.GetSymmetricalPos(pos.x, pos.y);
            p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
            p.grid[sym.x][sym.y].gap = Cell::Gap::BREAK;
        }
        _serializer.WritePuzzle(p, 0x86);
    }
    { // Back wall 2
        Puzzle p;
        p.NewGrid(4, 4);
        p.symmetry = Puzzle::Symmetry::X;
        p.grid[0][8].start = true;
        p.grid[8][8].start = true;
        p.grid[2][0].end = Cell::Dir::UP;
        p.grid[6][0].end = Cell::Dir::UP;
        std::vector<Pos> cutEdges = Randomizer2Core::CutSymmetricalEdgePairs(p, 4);
        for (int i=0; i<cutEdges.size(); i++) {
            Pos pos = cutEdges[i];
            if (i%2 == 0) {
                p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
            } else {
                Pos sym = p.GetSymmetricalPos(pos.x, pos.y);
                p.grid[sym.x][sym.y].gap = Cell::Gap::BREAK;
            }
        }

        _serializer.WritePuzzle(p, 0x87);
    }
    { // Back wall 3
        Puzzle p;
        p.NewGrid(5, 6);
        p.symmetry = Puzzle::Symmetry::X;
        p.grid[2][10].start = true;
        p.grid[8][10].start = true;
        p.grid[4][0].end = Cell::Dir::UP;
        p.grid[6][0].end = Cell::Dir::UP;
        std::vector<Pos> cutEdges = Randomizer2Core::CutSymmetricalEdgePairs(p, 10);
        for (int i=0; i<cutEdges.size(); i++) {
            Pos pos = cutEdges[i];
            if (i%2 == 0) {
                p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
            } else {
                Pos sym = p.GetSymmetricalPos(pos.x, pos.y);
                p.grid[sym.x][sym.y].gap = Cell::Gap::BREAK;
            }
        }

        _serializer.WritePuzzle(p, 0x59);
    }
    { // Back wall 4
        Puzzle p;
        p.NewGrid(5, 8);
        p.symmetry = Puzzle::Symmetry::X;
        p.grid[2][16].start = true;
        p.grid[8][16].start = true;
        p.grid[4][0].end = Cell::Dir::UP;
        p.grid[6][0].end = Cell::Dir::UP;
        std::vector<Pos> cutEdges = Randomizer2Core::CutSymmetricalEdgePairs(p, 15);
        for (int i=0; i<cutEdges.size(); i++) {
            Pos pos = cutEdges[i];
            if (i%2 == 0) {
                p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
            } else {
                Pos sym = p.GetSymmetricalPos(pos.x, pos.y);
                p.grid[sym.x][sym.y].gap = Cell::Gap::BREAK;
            }
        }

        _serializer.WritePuzzle(p, 0x62);
    }
    // TODO: Positioning is off, slightly -- which means you can start from the bottom left, if you peek around.
    { // Back wall 5
        Puzzle p;
        p.NewGrid(11, 8);
        p.symmetry = Puzzle::Symmetry::X;
        p.grid[0][16].start = true;
        p.grid[10][16].start = true;
        p.grid[12][16].start = true;
        p.grid[22][16].start = true;
        p.grid[2][0].end = Cell::Dir::UP;
        p.grid[8][0].end = Cell::Dir::UP;
        p.grid[14][0].end = Cell::Dir::UP;
        p.grid[20][0].end = Cell::Dir::UP;

        Puzzle q;
        q.NewGrid(5, 8);
        q.symmetry = Puzzle::Symmetry::X;

        for (Pos pos : Randomizer2Core::CutSymmetricalEdgePairs(q, 16)) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
        }
        for (Pos pos : Randomizer2Core::CutSymmetricalEdgePairs(q, 16)) {
            p.grid[pos.x + 12][pos.y].gap = Cell::Gap::BREAK;
        }

        for (int y=0; y<p.height; y+=2) {
            p.grid[5][y].gap = Cell::Gap::BREAK;
        }

        _serializer.WritePuzzle(p, 0x5C);
    }

    { // Rotational 1
        Puzzle p;
        p.NewGrid(3, 3);
        p.symmetry = Puzzle::Symmetry::XY;
        p.grid[6][0].start = true;
        p.grid[0][6].start = true;
        p.grid[4][0].end = Cell::Dir::UP;
        p.grid[2][6].end = Cell::Dir::DOWN;

        p.grid[5][0].gap = Cell::Gap::BREAK;
        p.grid[1][6].gap = Cell::Gap::BREAK;

        for (Pos pos : Randomizer2Core::CutSymmetricalEdgePairs(p, 1)) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
            Pos sym = p.GetSymmetricalPos(pos.x, pos.y);
            p.grid[sym.x][sym.y].gap = Cell::Gap::BREAK;
        }
        _serializer.WritePuzzle(p, 0x8D);
    }
    { // Rotational 2
        Puzzle p;
        p.NewGrid(3, 3);
        p.symmetry = Puzzle::Symmetry::XY;
        p.grid[6][0].start = true;
        p.grid[0][6].start = true;
        p.grid[4][0].end = Cell::Dir::UP;
        p.grid[2][6].end = Cell::Dir::DOWN;

        p.grid[5][0].gap = Cell::Gap::BREAK;
        p.grid[1][6].gap = Cell::Gap::BREAK;

        std::vector<Pos> cutEdges = Randomizer2Core::CutSymmetricalEdgePairs(p, 3);
        for (int i=0; i<cutEdges.size(); i++) {
            Pos pos = cutEdges[i];
            if (i%2 == 0) {
                p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
            } else {
                Pos sym = p.GetSymmetricalPos(pos.x, pos.y);
                p.grid[sym.x][sym.y].gap = Cell::Gap::BREAK;
            }
        }

        p.grid[1][6].gap = Cell::Gap::NONE;

        _serializer.WritePuzzle(p, 0x81);
    }
    { // Rotational 3
        Puzzle p;
        p.NewGrid(4, 4);
        p.symmetry = Puzzle::Symmetry::XY;
        p.grid[8][0].start = true;
        p.grid[0][8].start = true;
        p.grid[0][0].end = Cell::Dir::LEFT;
        p.grid[8][8].end = Cell::Dir::RIGHT;

        std::vector<Pos> cutEdges = Randomizer2Core::CutSymmetricalEdgePairs(p, 7);
        for (int i=0; i<cutEdges.size(); i++) {
            Pos pos = cutEdges[i];
            if (i%2 == 0) {
                p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
            } else {
                Pos sym = p.GetSymmetricalPos(pos.x, pos.y);
                p.grid[sym.x][sym.y].gap = Cell::Gap::BREAK;
            }
        }

        _serializer.WritePuzzle(p, 0x83);
    }
    { // Melting
        Puzzle p;
        p.NewGrid(6, 6);
        p.symmetry = Puzzle::Symmetry::XY;
        p.grid[12][0].start = true;
        p.grid[0][12].start = true;
        p.grid[0][0].end = Cell::Dir::LEFT;
        p.grid[12][12].end = Cell::Dir::RIGHT;
        Puzzle q = p;

        std::vector<Pos> cutEdges = Randomizer2Core::CutSymmetricalEdgePairs(p, 15);
        for (int i=0; i<cutEdges.size(); i++) {
            Pos pos = cutEdges[i];
            Pos sym = p.GetSymmetricalPos(pos.x, pos.y);

            if (i%2 == 0) {
                p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
            } else {
                p.grid[sym.x][sym.y].gap = Cell::Gap::BREAK;
            }

            if (pos.x < sym.x) {
                q.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
            } else {
                q.grid[sym.x][sym.y].gap = Cell::Gap::BREAK;
            }
        }

        _serializer.WritePuzzle(p, 0x84); // Melting 1
        _serializer.WritePuzzle(q, 0x82); // Melting 2
        _serializer.WritePuzzle(q, 0x343A); // Melting 3
    }
}

void Randomizer2::RandomizeSymmetryIsland() {
    { // Entry door
        Puzzle p;
        p.NewGrid(4, 3);
        p.grid[0][6].start = true;
        p.grid[8][0].end = Cell::Dir::RIGHT;
        p.grid[4][3].gap = Cell::Gap::FULL;

        std::vector<Pos> corners;
        std::vector<Pos> cells;
        std::vector<Pos> edges;
        for (int x=0; x<p.width; x++) {
            for (int y=0; y<p.height; y++) {
                if (x%2 == 0 && y%2 == 0) corners.emplace_back(Pos{x, y});
                else if (x%2 == 1 && y%2 == 1) cells.emplace_back(Pos{x, y});
                else edges.emplace_back(Pos{x, y});
            }
        }

        for (int j=0;; j++) {
            std::vector<Pos> dots = Random::SelectFromSet(edges, 4);
            for (Pos pos : dots) p.grid[pos.x][pos.y].dot = Cell::Dot::BLACK;

            auto solutions = Solver::Solve(p);
            if (solutions.size() > 0 && solutions.size() < 10) break;

            for (Pos pos : dots) p.grid[pos.x][pos.y].dot = Cell::Dot::NONE;
        }

        _serializer.WritePuzzle(p, 0xB0);
    }

    { // Dots 1
        Puzzle p;
        p.NewGrid(3, 3);
        p.symmetry = Puzzle::Symmetry::Y;
        p.grid[0][2].start = true;
        p.grid[0][4].start = true;
        p.grid[6][2].end = Cell::Dir::RIGHT;
        p.grid[6][4].end = Cell::Dir::RIGHT;

        std::vector<Pos> corners;
        std::vector<Pos> cells;
        std::vector<Pos> edges;
        for (int x=0; x<p.width; x++) {
            for (int y=0; y<p.height/2; y++) {
                if (x%2 == 0 && y%2 == 0) corners.emplace_back(Pos{x, y});
                else if (x%2 == 1 && y%2 == 1) cells.emplace_back(Pos{x, y});
                else edges.emplace_back(Pos{x, y});
            }
        }
        edges.insert(edges.end(), corners.begin(), corners.end());

        std::vector<Pos> dots;
        for (int j=0;; j++) {
            dots = Random::SelectFromSet(edges, 3);
            for (Pos pos : dots) p.grid[pos.x][pos.y].dot = Cell::Dot::BLACK;

            auto solutions = Solver::Solve(p);
            if (solutions.size() == 2) break;

            for (Pos pos : dots) p.grid[pos.x][pos.y].dot = Cell::Dot::NONE;
        }

        for (Pos pos : dots) {
            Pos sym = p.GetSymmetricalPos(pos.x, pos.y);
            p.grid[sym.x][sym.y].dot = Cell::Dot::BLACK;
        }

        _serializer.WritePuzzle(p, 0x22);
    }
    { // Dots 2
        Puzzle p;
        p.NewGrid(3, 3);
        p.symmetry = Puzzle::Symmetry::Y;
        p.grid[0][2].start = true;
        p.grid[0][4].start = true;
        p.grid[6][2].end = Cell::Dir::RIGHT;
        p.grid[6][4].end = Cell::Dir::RIGHT;

        std::vector<Pos> corners;
        std::vector<Pos> cells;
        std::vector<Pos> edges;
        for (int x=0; x<p.width; x++) {
            for (int y=0; y<p.height/2; y++) {
                if (x%2 == 0 && y%2 == 0) corners.emplace_back(Pos{x, y});
                else if (x%2 == 1 && y%2 == 1) cells.emplace_back(Pos{x, y});
                else edges.emplace_back(Pos{x, y});
            }
        }
        edges.insert(edges.end(), corners.begin(), corners.end());

        std::vector<Pos> dots;
        for (int j=0;; j++) {
            dots = Random::SelectFromSet(edges, 3);
            for (Pos pos : dots) p.grid[pos.x][pos.y].dot = Cell::Dot::BLACK;

            auto solutions = Solver::Solve(p);
            if (solutions.size() == 2) break;

            for (Pos pos : dots) p.grid[pos.x][pos.y].dot = Cell::Dot::NONE;
        }

        Pos pos = dots[1];
        Pos sym = p.GetSymmetricalPos(pos.x, pos.y);
        p.grid[pos.x][pos.y].dot = Cell::Dot::NONE;
        p.grid[sym.x][sym.y].dot = Cell::Dot::BLACK;

        _serializer.WritePuzzle(p, 0x23);
    }
}

void Randomizer2::RandomizeKeep() {
    { // Hedges 1
        Puzzle p;
        p.NewGrid(4, 4);

        p.grid[2][1].gap = Cell::Gap::FULL;
        p.grid[4][1].gap = Cell::Gap::FULL;
        p.grid[6][1].gap = Cell::Gap::FULL;
        p.grid[3][2].gap = Cell::Gap::FULL;
        p.grid[5][2].gap = Cell::Gap::FULL;
        p.grid[8][3].gap = Cell::Gap::FULL;
        p.grid[2][5].gap = Cell::Gap::FULL;
        p.grid[6][5].gap = Cell::Gap::FULL;
        p.grid[7][6].gap = Cell::Gap::FULL;
        p.grid[2][7].gap = Cell::Gap::FULL;
        p.grid[4][7].gap = Cell::Gap::FULL;

        p.grid[4][8].start = true;
        p.grid[6][0].end = Cell::Dir::UP;

        std::vector<Pos> cutEdges = Randomizer2Core::CutInsideEdges(p, 5);
        Puzzle copy = p;
        std::vector<int> gates = {0x00344, 0x00488,  0x00489, 0x00495, 0x00496};
        for (int i=0; i<cutEdges.size(); i++) {
            Pos pos = cutEdges[i];
            copy.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
            SetGate(gates[i], pos.x, pos.y);
        }
        auto solution = GetUniqueSolution(copy);
        p.sequence = solution.sequence;
        _serializer.WritePuzzle(p, 0x139);
    }

    { // Hedges 2
        Puzzle p;
        p.NewGrid(4, 4);

        p.grid[2][1].gap = Cell::Gap::FULL;
        p.grid[1][2].gap = Cell::Gap::FULL;
        p.grid[5][2].gap = Cell::Gap::FULL;
        p.grid[7][4].gap = Cell::Gap::FULL;
        p.grid[4][5].gap = Cell::Gap::FULL;
        p.grid[6][5].gap = Cell::Gap::FULL;
        p.grid[1][6].gap = Cell::Gap::FULL;
        p.grid[2][7].gap = Cell::Gap::FULL;
        p.grid[5][8].gap = Cell::Gap::FULL;

        p.grid[0][8].start = true;
        p.grid[8][0].end = Cell::Dir::RIGHT;

        std::vector<Pos> cutEdges = Randomizer2Core::CutInsideEdges(p, 7);
        for (Pos pos : cutEdges) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }
        auto solution = GetUniqueSolution(p);

        Puzzle q;
        q.NewGrid(4, 4);
        q.grid[0][8].start = true;
        q.grid[8][0].end = Cell::Dir::RIGHT;
        q.sequence = solution.sequence;
        for (Pos pos : cutEdges) {
            q.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }
        // Cut to 6 of 9 additional edges
        for (Pos pos : Randomizer2Core::CutInsideEdges(q, 6)) {
            q.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }
        _serializer.WritePuzzle(q, 0x19DC);
    }
    
    { // Hedges 3 [WIP]
        Puzzle p;
        p.NewGrid(4, 4);

        p.grid[2][1].gap = Cell::Gap::FULL;
        p.grid[5][2].gap = Cell::Gap::FULL;
        p.grid[7][2].gap = Cell::Gap::FULL;
        p.grid[4][3].gap = Cell::Gap::FULL;
        p.grid[1][4].gap = Cell::Gap::FULL;
        p.grid[6][5].gap = Cell::Gap::FULL;
        p.grid[1][6].gap = Cell::Gap::FULL;
        p.grid[3][6].gap = Cell::Gap::FULL;
        p.grid[6][7].gap = Cell::Gap::FULL;

        p.grid[0][8].start = true;
        p.grid[8][2].end = Cell::Dir::RIGHT;

        std::vector<Pos> cutEdges = Randomizer2Core::CutInsideEdges(p, 7);
        for (Pos pos : cutEdges) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
        }

        std::vector<int> pebbleMarkers = {0x034a9, 0x034b1, 0x034be, 0x034c4};

        // _serializer.WritePuzzle(p, 0x19E7);
    }

    { // Hedges 4
        Puzzle p;
        p.NewGrid(4, 4);

        p.grid[3][0].gap = Cell::Gap::FULL;
        p.grid[4][1].gap = Cell::Gap::FULL;
        p.grid[8][1].gap = Cell::Gap::FULL;
        p.grid[1][2].gap = Cell::Gap::FULL;
        p.grid[4][3].gap = Cell::Gap::FULL;
        p.grid[8][3].gap = Cell::Gap::FULL;
        p.grid[1][4].gap = Cell::Gap::FULL;
        p.grid[5][4].gap = Cell::Gap::FULL;
        p.grid[2][5].gap = Cell::Gap::FULL;
        p.grid[6][5].gap = Cell::Gap::FULL;
        p.grid[3][6].gap = Cell::Gap::FULL;
        p.grid[0][7].gap = Cell::Gap::FULL;
        p.grid[8][7].gap = Cell::Gap::FULL;
        p.grid[5][8].gap = Cell::Gap::FULL;

        p.grid[0][8].start = true;
        p.grid[4][0].end = Cell::Dir::UP;

        std::vector<Pos> cutEdges = Randomizer2Core::CutInsideEdges(p, 2);
        for (Pos pos : cutEdges) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }
        auto solution = GetUniqueSolution(p);

        Puzzle q;
        q.NewGrid(4, 4);
        q.grid[0][8].start = true;
        q.grid[4][0].end = Cell::Dir::UP;
        q.sequence = solution.sequence;
        for (Pos pos : cutEdges) {
            q.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }
        for (Pos pos : Randomizer2Core::CutInsideEdges(q, 7)) {
            q.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }
        _serializer.WritePuzzle(q, 0x1A0F);
    }
}

Puzzle Randomizer2::GetUniqueSolution(Puzzle& p) {
    auto solutions = Solver::Solve(p);
    assert(solutions.size() == 1);
    return solutions[0];
}

void Randomizer2::SetGate(int panel, int X, int Y) {
    float x, y, z, w;
    if (X%2 == 0 && Y%2 == 1) { // Horizontal
        x = -1.49f * X + 0.22f * Y + 66.58f;
        y = 0.275f * X +  1.6f * Y + 108.4f;
        z = -.77f;
        w = .63f;
    } else { // Vertical
        assert(X%2 == 1 && Y%2 == 0);
        x = -1.6f * X + 0.35f * Y +   66.5f;
        y = 0.25f * X +  1.6f * Y + 108.55f;
        z = -0.1f;
        w = 1.0f;
    }

    SetPos(panel, x, y, 19.2f);
    // _memory->WriteEntityData<float>(panel, ORIENTATION, {0.0f, 0.0f, z, w});
}

void Randomizer2::SetPos(int panel, float x, float y, float z) {
    // _memory->WriteEntityData<float>(panel, POSITION, {x, y, z});
}