#pragma once
#include <vector>

struct Pos;
class Puzzle;

class Randomizer2Core {
public:
    // CAUTION: Does not actually cut edges, just returns a list of suggested cuts.
    static std::vector<Pos> CutEdges(const Puzzle& p, size_t numEdges);
    static std::vector<Pos> CutEdges2(const Puzzle& p, size_t numEdges);

private:
    static std::vector<Pos> CutEdgesInternal(const Puzzle& p, std::vector<Pos>& edges, size_t numEdges);
    static void DebugColorGrid(const std::vector<std::vector<int>>& colorGrid);
    static void FloodFill(const Puzzle& p, std::vector<std::vector<int>>& colorGrid, int color, int x, int y);
    static void FloodFillOutside(const Puzzle& p, std::vector<std::vector<int>>& colorGrid, int x, int y);
    static std::tuple<std::vector<std::vector<int>>, int> CreateColorGrid(const Puzzle& p);
};

