#pragma once
#include <vector>

class Puzzle;
class Solver {
public:
    static int MAX_SOLUTIONS;
    static std::vector<Puzzle> Solve(Puzzle& p);

private:
    static void SolveLoop(Puzzle& p, int x, int y, std::vector<Puzzle>& solutions);
};

