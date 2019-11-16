#include "Randomizer2.h"
#include "Puzzle.h"
#include "Random.h"
#include "Solver.h"
#include "Memory.h"
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

void DebugColorGrid(const std::vector<std::vector<int>>& colorGrid) {
    for (int y=0; y<colorGrid[0].size(); y++) {
        std::wstring row;
        for (int x=0; x<colorGrid.size(); x++) {
            row += std::to_wstring(colorGrid[x][y]);
        }
        row += L'\n';
        OutputDebugString(row.c_str());
    }
    OutputDebugString(L"\n");
}

void FloodFill(const Puzzle& p, std::vector<std::vector<int>>& colorGrid, int color, int x, int y) {
    if (!p.SafeCell(x, y)) return;
    if (colorGrid[x][y] != 0) return; // Already processed.
    colorGrid[x][y] = color;

    FloodFill(p, colorGrid, color, x, y+1);
    FloodFill(p, colorGrid, color, x, y-1);
    FloodFill(p, colorGrid, color, x+1, y);
    FloodFill(p, colorGrid, color, x-1, y);
}

void FloodFillOutside(const Puzzle&p, std::vector<std::vector<int>>& colorGrid, int x, int y) {
    if (!p.SafeCell(x, y)) return;
    if (colorGrid[x][y] != 0) return; // Already processed.
    if (x%2 != y%2 && p.grid[x][y].gap != Cell::Gap::FULL) return; // Only flood-fill through full gaps
    colorGrid[x][y] = 1; // Outside color

    FloodFillOutside(p, colorGrid, x, y+1);
    FloodFillOutside(p, colorGrid, x, y-1);
    FloodFillOutside(p, colorGrid, x+1, y);
    FloodFillOutside(p, colorGrid, x-1, y);
}

/*
    undefined -> 1 (color of outside) or * (any colored cell) or -1 (edge/intersection not part of any region)

    0 -> {} (this is a special edge case, which I don't need right now)
    1 -> 0 (uncolored / ready to color)
    2 -> 
*/
std::vector<std::vector<int>> CreateColorGrid(const Puzzle& p) {
    std::vector<std::vector<int>> colorGrid;
    colorGrid.resize(p.width);

    for (int x=0; x<p.width; x++) {
        colorGrid[x].resize(p.height);
        for (int y=0; y<p.height; y++) {
            // Mark all unbroken edges and intersections as 'do not color'
            if (x%2 != y%2) {
                if (p.grid[x][y].gap == Cell::Gap::NONE) colorGrid[x][y] = 1;
            } else if (x%2 == 0 && y%2 == 0) {
                // @Future: What about empty intersections?
                colorGrid[x][y] = 1; // do not color intersections
            }
        }
    }

    // @Future: Skip this loop if pillar = true;
    for (int y=1; y<p.height; y+=2) {
        FloodFillOutside(p, colorGrid, 0, y);
        FloodFillOutside(p, colorGrid, p.width - 1, y);
    }

    for (int x=1; x<p.width; x+=2) {
        FloodFillOutside(p, colorGrid, x, 0);
        FloodFillOutside(p, colorGrid, x, p.height - 1);
    }

    int color = 1;
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
            if (colorGrid[x][y] != 0) continue; // No dead colors
            color++;
            FloodFill(p, colorGrid, color, x, y);
        }
    }

    return colorGrid;
}

void Randomizer2::RandomizeKeep() {
    Puzzle p;
    p.NewGrid(4, 4);
    // p.width = 9;
    // p.height = 10;
    // p.grid.clear();
    // p.grid.resize(p.width);
    // for (int x=0; x<p.width; x++) p.grid[x].resize(p.height);

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
    // p.grid[0][9].gap = Cell::Gap::FULL;
    // p.grid[2][9].gap = Cell::Gap::FULL;
    // p.grid[6][9].gap = Cell::Gap::FULL;
    // p.grid[8][9].gap = Cell::Gap::FULL;

    p.grid[6][0].end = Cell::Dir::UP;
    p.grid[4][8].start = true;

    auto colorGrid = CreateColorGrid(p);

    std::vector<Pos> edges;
    for (int x=0; x<p.width; x++) {
        for (int y=0; y<p.height; y++) {
            if (x%2 == y%2) continue;
            if (p.grid[x][y].gap != Cell::Gap::NONE) continue;
            edges.emplace_back(Pos{x, y});
        }
    }

    Puzzle copy = p;
    std::vector<int> gates = {0x00344, 0x00488,  0x00489, 0x00495, 0x00496};
    for (int i=0; i<5; i++) {
        for (int j=0; j<edges.size(); j++) {
            int edge = Random::RandInt(0, static_cast<int>(edges.size() - 1));
            Pos pos = edges[edge];
            edges.erase(edges.begin() + edge);

            int color1 = 0;
            int color2 = 0;
            if (pos.x%2 == 0 && pos.y%2 == 1) { // Vertical
                if (pos.x > 0) color1 = colorGrid[pos.x-1][pos.y];
                else color1 = 1;

                if (pos.x < p.width - 1) color2 = colorGrid[pos.x+1][pos.y];
                else color2 = 1;
            } else { // Horizontal
                assert(pos.x%2 == 1 && pos.y%2 == 0);
                if (pos.y > 0) color1 = colorGrid[pos.x][pos.y-1];
                else color1 = 1;

                if (pos.y < p.height - 1) color2 = colorGrid[pos.x][pos.y+1];
                else color2 = 1;
            }
            // Enforce color1 < color2
            if (color1 > color2) std::swap(color1, color2);

            // Colors mismatch, valid cut
            if (color1 != color2) {
                // @Performance... have a lookup table instead?
                for (int x=0; x<p.width; x++) {
                    for (int y=0; y<p.height; y++) {
                        if (colorGrid[x][y] == color2) colorGrid[x][y] = color1;
                    }
                }
                copy.grid[pos.x][pos.y].gap = Cell::Gap::BREAK;
                SetGate(gates[i], pos.x, pos.y);
                break;
            }
        }
    }

    auto solutions = Solver::Solve(copy);
    assert(solutions.size() == 1);
    p.sequence = solutions[0].sequence;
    PuzzleSerializer(_memory).WritePuzzle(solutions[0], 0x139);
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
    _memory->WritePanelData<float>(panel, ORIENTATION, {0.0f, 0.0f, z, w});
}

void Randomizer2::SetPos(int panel, float x, float y, float z) {
    _memory->WritePanelData<float>(panel, POSITION, {x, y, z});
}