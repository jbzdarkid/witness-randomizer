#include "Randomizer2.h"
#include "Puzzle.h"
#include "Random.h"
#include "Solver.h"
#include "Memory.h"
#include "Randomizer2Core.h"
#include "PuzzlerSerializer.h"
#include <cassert>
#include <string>

void FloodFillInternal(const Puzzle& p, std::vector<std::vector<bool>>& reached, int x, int y) {
    if (x%2 == 1 && y%2 == 1) return;
    auto cell = p.GetCell(x, y);
    if (cell.undefined) return;
    if (cell.gap != Cell::Gap::NONE) return;
    if (reached[x][y]) return;

    reached[x][y] = true;
    FloodFillInternal(p, reached, x-1, y);
    FloodFillInternal(p, reached, x+1, y);
    FloodFillInternal(p, reached, x, y-1);
    FloodFillInternal(p, reached, x, y+1);
}

// Returns true: All nodes reachable / false: Some node disconnected
bool FloodFill(const Puzzle& p) {
    std::vector<std::vector<bool>> reached;
    reached.resize(p.width);
    for (int x=0; x<p.width; x++) reached[x].resize(p.height);
    FloodFillInternal(p, reached, 0, 0);

    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
            if (x%2 == 0 && y%2 == 0) { // @Specialized
                if (!reached[x][y]) return false;
            }
        }
    }
    return true;
}

Randomizer2::Randomizer2(const std::shared_ptr<Memory>& memory) : _memory(memory) {}

void Randomizer2::Randomize() {
    // 4x4
    // 14 gaps
    // start (x=0, y=8)
    // end (x=8, y=0) Up
    // 1 solution
    Puzzle p;
    while (true) {
        p.NewGrid(4, 4);

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

        for (int i=0; i<14; i++) {
            for (int j=0; j<edges.size(); j++) {
                int edge = Random::RandInt(0, static_cast<int>(edges.size() - 1));
                Pos pos = edges[edge];
                edges.erase(edges.begin() + edge);

                if (FloodFill(p)) {
                    p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
                    break;
                } else {
                    p.grid[pos.x][pos.y].gap = Cell::Gap::NONE;
                }
            }
        }

        p.grid[0][8].start = true;
        p.grid[8][0].end = Cell::Dir::UP;

        auto solutions = Solver::Solve(p);
        if (solutions.size() > 0) break;
    }
    PuzzleSerializer(_memory).WritePuzzle(p, 0x293);

    // 7x7
    // 35 gaps
    // start (x=8, y=8)
    // end (x=4, y=0) Up
    // 2 solutions, 37 & 39
    while (true) {
        p.NewGrid(7, 7);

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

        for (int i=0; i<35; i++) {
            for (int j=0; j<edges.size(); j++) {
                // TODO: Precompute edge weights and make a weighted cut.
                // This will also preclude the need for flood filling.
                // https://en.wikipedia.org/wiki/Biconnected_component

                int edge = Random::RandInt(0, static_cast<int>(edges.size() - 1));
                Pos pos = edges[edge];
                edges.erase(edges.begin() + edge);
                
                if (FloodFill(p)) {
                    p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
                    break;
                } else {
                    p.grid[pos.x][pos.y].gap = Cell::Gap::NONE;
                }
            }
        }

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

        auto solutions = Solver::Solve(p);
        if (solutions.size() > 0) break;
    }
    PuzzleSerializer(_memory).WritePuzzle(p, 0x295);

}

void Randomizer2::RandomizeKeep() {
    // *** Hedges 1 ***
    {
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

        std::vector<Pos> cutEdges = Randomizer2Core::CutEdgesToBeUnique(p);
        assert(cutEdges.size() == 5);
        Puzzle copy = p;
        std::vector<int> gates = {0x00344, 0x00488,  0x00489, 0x00495, 0x00496};
        for (int i=0; i<gates.size(); i++) {
            Pos pos = cutEdges[i];
            copy.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
            SetGate(gates[i], pos.x, pos.y);
        }
        auto solutions = Solver::Solve(copy);
        assert(solutions.size() == 1);
        p.sequence = solutions[0].sequence;
        PuzzleSerializer(_memory).WritePuzzle(solutions[0], 0x139);
    }

    // *** Hedges 2 ***
    {
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

        std::vector<Pos> cutEdges = Randomizer2Core::CutEdgesToBeUnique(p);
        assert(cutEdges.size() == 7);
        for (Pos pos : cutEdges) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }
        auto solutions = Solver::Solve(p);
        assert(solutions.size() == 1);

        Puzzle q;
        q.NewGrid(4, 4);
        q.grid[0][8].start = true;
        q.grid[8][0].end = Cell::Dir::RIGHT;
        q.sequence = solutions[0].sequence;
        for (Pos pos : cutEdges) {
            q.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }
        // Cut to 4 of 9 additional edges (total: 11)
        Randomizer2Core::CutEdgesNotOutsideNotBreakingSequence(q, 4);
        PuzzleSerializer(_memory).WritePuzzle(q, 0x19DC);
    }
    
    // *** Hedges 3 **
    {
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


        std::vector<Pos> cutEdges = Randomizer2Core::CutEdgesToBeUnique(p);
        assert(cutEdges.size() == 7);
        for (Pos pos : cutEdges) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
        }
        PuzzleSerializer(_memory).WritePuzzle(p, 0x19E7);
    }

    // *** Hedges 4 ***
    {
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

        std::vector<Pos> cutEdges = Randomizer2Core::CutEdgesToBeUnique(p);
        assert(cutEdges.size() == 2);
        for (Pos pos : cutEdges) {
            p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }
        auto solutions = Solver::Solve(p);
        assert(solutions.size() == 1);

        Puzzle q;
        q.NewGrid(4, 4);
        q.grid[0][8].start = true;
        q.grid[4][0].end = Cell::Dir::UP;
        q.sequence = solutions[0].sequence;
        for (Pos pos : cutEdges) {
            q.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
        }
        // 9 cuts, -2 from existing cuts
        Randomizer2Core::CutEdgesNotOutsideNotBreakingSequence(q, 7);
        PuzzleSerializer(_memory).WritePuzzle(q, 0x1A0F);
    }
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