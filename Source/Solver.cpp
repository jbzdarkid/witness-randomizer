#include "pch.h"
#include "Solver.h"
#include "Validator.h"

int Solver::MAX_SOLUTIONS = 10000;

std::vector<Puzzle> Solver::Solve(Puzzle& p) {
    std::vector<Puzzle> solutions;
    // var start = (new Date()).getTime()
    for (int x = 0; x < p.width; x++) {
        for (int y = 0; y < p.height; y++) {
            Cell cell = p.grid[x][y];
            if (cell.start) {
                SolveLoop(p, x, y, solutions);
            }
        }
    }
    // var end = (new Date()).getTime()
    // console.info('Solved', puzzle, 'in', (end-start)/1000, 'seconds')
    return solutions;
}

void Solver::SolveLoop(Puzzle& p, int x, int y, std::vector<Puzzle>& solutions) {
 // Stop trying to solve once we reach our goal
    if (solutions.size() >= MAX_SOLUTIONS) return;
    Cell cell = p.GetCell(x, y);
    if (cell.undefined) return;
    if (cell.gap != Cell::Gap::NONE) return;

    if (p.symmetry == Puzzle::Symmetry::NONE) {
        if (cell.color != Cell::Color::NONE) return; // Collided with ourselves
        p.grid[x][y].color = Cell::Color::BLACK; // Otherwise, mark this cell as visited
        p.sequence.emplace_back(x, y);
    } else {
        // Get the symmetrical position, and try coloring it
        auto sym = p.GetSymmetricalPos(x, y);
        Cell::Color oldColor = p.GetLine(sym.x, sym.y);
        p.grid[sym.x][sym.y].color = Cell::Color::YELLOW;

        // Collided with ourselves or our reflection
        if (cell.color != Cell::Color::NONE) {
            p.grid[sym.x][sym.y].color = oldColor;
            return;
        }
        p.grid[x][y].color = Cell::Color::BLUE; // Otherwise, mark this cell as visited
    }

    if (cell.end != Cell::Dir::NONE) {
        // Reached an endpoint, validate solution and keep going -- there may be other endpoints
        Validator::Validate(p);
        if (p.valid) {
            Puzzle clone = p;
            solutions.push_back(clone);
        }
    }

    // Recursion order (LRUD) is optimized for BL->TR and mid-start puzzles
    // Extend path left and right
    if (y % 2 == 0) {
        SolveLoop(p, x - 1, y, solutions);
        SolveLoop(p, x + 1, y, solutions);
    }
    // Extend path up and down
    if (x % 2 == 0) {
        SolveLoop(p, x, y - 1, solutions);
        SolveLoop(p, x, y + 1, solutions);
    }

    // Tail recursion: Back out of this cell
    p.grid[x][y].color = Cell::Color::NONE;
    p.sequence.pop_back();
    if (p.symmetry != Puzzle::Symmetry::NONE) {
        auto sym = p.GetSymmetricalPos(x, y);
        p.grid[sym.x][sym.y].color = Cell::Color::NONE;
    }
}
