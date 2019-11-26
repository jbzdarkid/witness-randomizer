#pragma once

class Randomizer2Core {
public:
    // CAUTION: These do not actually cut edges, they just returns a list of suggested cuts.
    static std::vector<Pos> CutEdges(const Puzzle& p, size_t numEdges);
    static std::vector<Pos> CutInsideEdges(const Puzzle& p, size_t numEdges);
    static std::vector<Pos> CutSymmetricalEdgePairs(const Puzzle& p, size_t numEdges);

private:
    static std::vector<Pos> CutEdgesInternal(const Puzzle& p, int xMin, int xMax, int yMin, int yMax, size_t numEdges);
    static void DebugColorGrid(const std::vector<std::vector<int>>& colorGrid);
    static void FloodFill(const Puzzle& p, std::vector<std::vector<int>>& colorGrid, int color, int x, int y);
    static void FloodFillOutside(const Puzzle& p, std::vector<std::vector<int>>& colorGrid, int x, int y);
    static std::tuple<std::vector<std::vector<int>>, int> CreateColorGrid(const Puzzle& p);
};

