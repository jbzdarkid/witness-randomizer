#include "Randomizer2.h"
#include "Puzzle.h"
#include "Random.h"
#include "Solver.h"
#include "Memory.h"

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
                p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
                edges.erase(edges.begin() + edge);

                if (FloodFill(p)) {
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
                p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
                edges.erase(edges.begin() + edge);
                
                if (FloodFill(p)) {
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
    Puzzle p;
    p.width = 9;
    p.height = 10;
    p.grid.clear();
    p.grid.resize(p.width);
    for (int x=0; x<p.width; x++) p.grid[x].resize(p.height);

    p.NewGrid(5, 5);
    p.grid[2][1].gap = Cell::Gap::FULL;
    p.grid[4][1].gap = Cell::Gap::FULL;
    p.grid[6][1].gap = Cell::Gap::FULL;
    p.grid[8][1].gap = Cell::Gap::FULL;
    p.grid[3][2].gap = Cell::Gap::FULL;
    p.grid[5][2].gap = Cell::Gap::FULL;
    p.grid[8][3].gap = Cell::Gap::FULL;
    p.grid[2][5].gap = Cell::Gap::FULL;
    p.grid[6][5].gap = Cell::Gap::FULL;
    p.grid[7][6].gap = Cell::Gap::FULL;
    p.grid[2][7].gap = Cell::Gap::FULL;
    p.grid[4][7].gap = Cell::Gap::FULL;
    p.grid[0][9].gap = Cell::Gap::FULL;
    p.grid[2][9].gap = Cell::Gap::FULL;
    p.grid[6][9].gap = Cell::Gap::FULL;
    p.grid[8][9].gap = Cell::Gap::FULL;

    p.grid[8][0].end = Cell::Dir::UP;
    p.grid[4][9].start = true;

    // 0x00496
    // 0x00344
    // 0x00495
    // 0x00488
    // 0x00489

    SetGate(0x00496, 2, 1);
}

void Randomizer2::SetGate(int panel, int X, int Y) {
    // x: (-1.5)X + (0.7)Y + 67.1
    // y: (0.3)X + (1.5)Y + 106.9
    // z: -19.1
    // Horizontal: (0, 0, -.1, 1)
    // Vertical: (0, 0, -.77, .63)

    float x = -1.5f * X + 0.7f * Y +  67.1f;
    float y =  0.3f * X + 1.5f * Y + 106.9f;
    _memory->WritePanelData<float>(panel, POSITION, {x, y, 19.1f});

    float z, w;
    if (X%2 == 0 && Y%2 == 1) { // Horizontal
        z = -0.1f;
        w = 1.0f;
    } else if (X%2 == 1 && Y%2 == 0) { // Vertical
        z = -.77f;
        w = .63f;
    } else {
        assert(false);
        return;
    }

    _memory->WritePanelData<float>(panel, ORIENTATION, {0.0f, 0.0f, z, w});
}