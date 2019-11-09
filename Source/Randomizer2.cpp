#include "Randomizer2.h"
#include "Puzzle.h"
#include "Random.h"
#include "Solver.h"

Randomizer2::Randomizer2(const std::shared_ptr<Memory>& memory) : _memory(memory) {}

void Randomizer2::Randomize() {
    // 4x4
    // 14 gaps
    // start (x=0, y=8)
    // end (x=8, y=0) Up
    // 1 solution
    Puzzle p;
    int attemptCount = 0;
    while (true) {
        attemptCount++;
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
            int edge = Random::RandInt(0, static_cast<int>(edges.size() - 1));
            Pos pos = edges[edge];
            p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
            edges.erase(edges.begin() + edge);
        }

        p.grid[0][8].start = true;
        p.grid[8][0].end = Cell::Dir::UP;

        auto solutions = Solver::Solve(p);
        if (solutions.size() == 1) {
            auto solution = solutions[0];
            int solutionLength = 0;
            for (int x=0; x<solution.width; x++) {
                for (int y=0; y<solution.height; y++) {
                    if (solution.grid[x][y].color == Cell::Color::BLACK) solutionLength++;
                }
            }
            if (solutionLength == 25) break;
        }
    }
    PuzzleSerializer(_memory).WritePuzzle(p, 0x293);




    // 7x7
    // 35 gaps
    // start (x=8, y=8)
    // end (x=4, y=0) Up
    // 2 solutions, 37 & 39
    attemptCount = 0;
    while (true) {
        attemptCount++;
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
            int edge = Random::RandInt(0, static_cast<int>(edges.size() - 1));
            Pos pos = edges[edge];
            p.grid[pos.x][pos.y].gap = Cell::Gap::FULL;
            edges.erase(edges.begin() + edge);
        }

        p.grid[8][8].start = true;
        p.grid[4][0].end = Cell::Dir::UP;

        auto solutions = Solver::Solve(p);
        if (solutions.size() > 0) break;
        if (solutions.size() > 0 && solutions.size() < 5) {
            auto solution = solutions[0];
            int solutionLength = 0;
            for (int x=0; x<solution.width; x++) {
                for (int y=0; y<solution.height; y++) {
                    if (solution.grid[x][y].color == Cell::Color::BLACK) solutionLength++;
                }
            }
            if (solutionLength > 30 && solutionLength < 40) break;
        }
    }
    PuzzleSerializer(_memory).WritePuzzle(p, 0x295);

}
