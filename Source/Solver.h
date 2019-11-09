#pragma once
#include <vector>

#ifndef MAX_SOLUTIONS
#define MAX_SOLUTIONS 10000
#endif

struct Puzzle;
class Solver {
public:
    static std::vector<Puzzle> Solve(Puzzle& p);

private:
    static void SolveLoop(Puzzle& p, int x, int y, std::vector<Puzzle>& solutions);
};

