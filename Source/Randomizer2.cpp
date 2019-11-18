#include "Memory.h"
#include "Randomizer2.h"
#include "Randomizer2Core.h"
#include "Puzzle.h"
#include "Random.h"
#include "Solver.h"

#include <cassert>
#include <string>

#pragma warning (disable: 26451)

Randomizer2::Randomizer2(const std::shared_ptr<Memory>& memory) : _memory(memory), _serializer(PuzzleSerializer(_memory)) {}

void Randomizer2::Randomize() {
    RandomizeTutorial();
    RandomizeKeep();
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

    {
        Puzzle p;
        p.NewGrid(7, 7);

        switch (Random::RandInt(1, 4)) {
            case 1:
                p.grid[Random::RandInt(0, p.width-1)][0].end = Cell::Dir::UP;
                break;
            case 2:
                p.grid[Random::RandInt(0, p.width-1)][p.height-1].end = Cell::Dir::DOWN;
                break;
            case 3:
                p.grid[0][Random::RandInt(0, p.height-1)].end = Cell::Dir::LEFT;
                break;
            case 4:
                p.grid[p.width-1][Random::RandInt(0, p.height-1)].end = Cell::Dir::RIGHT;
                break;
        }
        switch (Random::RandInt(1, 3)) {
            case 1: // Horiz (6) [5/7][4/6/8]
                p.grid[Random::RandInt(0, 1)*2 + 5][Random::RandInt(0, 2)*2 + 4].start = true;
                break;
            case 2: // Verti (6) [4/6/8][5/7]
                p.grid[Random::RandInt(0, 2)*2 + 4][Random::RandInt(0, 1)*2 + 5].start = true;
                break;
            case 3: // Inter (9) [4/6/8][4/6/8]
                p.grid[Random::RandInt(0, 2)*2 + 4][Random::RandInt(0, 2)*2 + 4].start = true;
                break;
        }
    
        for (Pos pos : Randomizer2Core::CutEdges(p, 35)) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }

        _serializer.WritePuzzle(p, 0x295);
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

        std::vector<Pos> cutEdges = Randomizer2Core::CutEdges2(p, 5);
        Puzzle copy = p;
        std::vector<int> gates = {0x00344, 0x00488,  0x00489, 0x00495, 0x00496};
        for (int i=0; i<gates.size(); i++) {
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

        std::vector<Pos> cutEdges = Randomizer2Core::CutEdges2(p, 7);
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
        for (Pos pos : Randomizer2Core::CutEdges2(q, 6)) {
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

        std::vector<int> pebbleMarkers = {0x034a9, 0x034b1, 0x034be, 0x034c4};

        std::vector<Pos> cutEdges = Randomizer2Core::CutEdges2(p, 7);
        for (Pos pos : cutEdges) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
        }
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

        std::vector<Pos> cutEdges = Randomizer2Core::CutEdges2(p, 2);
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
        for (Pos pos : Randomizer2Core::CutEdges2(q, 7)) {
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
    _memory->WriteEntityData<float>(panel, ORIENTATION, {0.0f, 0.0f, z, w});
}

void Randomizer2::SetPos(int panel, float x, float y, float z) {
    _memory->WriteEntityData<float>(panel, POSITION, {x, y, z});
}