#include "pch.h"
#include "Randomizer2Core.h"
#include "Random.h"

std::vector<Pos> Randomizer2Core::CutEdges(const Puzzle& p, size_t numEdges) {
    return CutEdgesInternal(p, 0, p.width, 0, p.height, numEdges);
}

std::vector<Pos> Randomizer2Core::CutInsideEdges(const Puzzle& p, size_t numEdges) {
    return CutEdgesInternal(p, 1, p.width-1, 1, p.height-1, numEdges);
}

std::vector<Pos> Randomizer2Core::CutSymmetricalEdgePairs(const Puzzle& p, size_t numEdges) {
    Puzzle copy = p;
    // Prevent cuts from landing on the midline
    if (p.symmetry == Puzzle::Symmetry::X) {
        for (int y=0; y<p.height; y++) {
            copy.grid[p.width/2][y].gap = Cell::Gap::FULL;
        }
        return CutEdgesInternal(copy, 0, (p.width-1)/2, 0, p.height, numEdges);
    } else if (p.symmetry == Puzzle::Symmetry::Y) {
        for (int x=0; x<p.width; x++) {
            copy.grid[x][p.height/2].gap = Cell::Gap::FULL;
        }
        return CutEdgesInternal(copy, 0, p.width, 0, (p.height-1)/2, numEdges);
    } else {
        assert(p.symmetry == Puzzle::Symmetry::XY);
        int midX = p.width/2;
        int midY = p.height/2;
        if (p.width%4 == 1 && p.height%4 == 1) { // For double-even grids, cut around the center
            copy.grid[midX-1][midY].gap = Cell::Gap::FULL;
            copy.grid[midX][midY-1].gap = Cell::Gap::FULL;
            copy.grid[midX][midY+1].gap = Cell::Gap::FULL;
            copy.grid[midX+1][midY].gap = Cell::Gap::FULL;
        } else if (p.width%4 == 1 && p.height%4 == 3) { // For half-even grids, there's only one line to cut
            copy.grid[midX][midY].gap = Cell::Gap::FULL;
        } else if (p.width%4 == 3 && p.height%4 == 1) { // For half-even grids, there's only one line to cut
            copy.grid[midX][midY].gap = Cell::Gap::FULL;
        }
        return CutEdgesInternal(copy, 0, p.width, 0, p.height, numEdges);
    }
}

std::vector<Pos> Randomizer2Core::CutEdgesInternal(const Puzzle& p, int xMin, int xMax, int yMin, int yMax, size_t numEdges) {
    std::vector<Pos> edges;
    for (int x=xMin; x<xMax; x++) {
        for (int y=yMin; y<yMax; y++) {
            if (x%2 == y%2) continue;
            if (p.grid[x][y].gap != Cell::Gap::NONE) continue;
            if (p.grid[x][y].start) continue;
            if (p.grid[x][y].end != Cell::Dir::NONE) continue;

            if (p.symmetry == Puzzle::Symmetry::XY) {
                assert(p.width == p.height); // TODO: This solution only supports square rotational symmetry.
                if (x > y) continue; // Only allow cuts bottom-left of the diagonal
            }

            // If the puzzle already has a sequence, don't cut along it.
            bool inSequence = false;
            for (Pos pos : p.sequence) inSequence |= (pos.x == x && pos.y == y);
            if (inSequence) continue;
            edges.emplace_back(x, y);
        }
    }
    assert(numEdges <= edges.size());

    auto [colorGrid, numColors] = CreateColorGrid(p);
    assert(numEdges <= numColors);

    // @Hack... sort of. I couldn't think of a better way to do this.
    if (p.symmetry == Puzzle::Symmetry::XY) {
        // Recolor the diagonal so that opposite cells share a color. This is because we're only cutting along half their edges,
        // so they are in fact two sides of the same cell.
        for (int x=1; x<p.width/2; x+=2) {
            assert(p.width == p.height); // TODO: This solution only supports square rotational symmetry.
            colorGrid[x][x] = colorGrid[p.width-x-1][p.width-x-1];
        }
    }

    std::vector<Pos> cutEdges;
    for (int i=0; i<numEdges; i++) {
        while (edges.size() > 0) {
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
                cutEdges.emplace_back(pos);
                break;
            }
        }
    }
    assert(cutEdges.size() == numEdges);
    return cutEdges;
}

#ifndef NDEBUG
#include <Windows.h>
#endif

void Randomizer2Core::DebugColorGrid(const std::vector<std::vector<int>>& colorGrid) {
#ifndef NDEBUG
    static std::string colors = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int y=0; y<colorGrid[0].size(); y++) {
        std::string row;
        for (int x=0; x<colorGrid.size(); x++) {
            row += colors[colorGrid[x][y]];
        }
        row += "\n";
        OutputDebugStringA(row.c_str());
    }
    OutputDebugStringA("\n");
#endif
}

void Randomizer2Core::FloodFill(const Puzzle& p, std::vector<std::vector<int>>& colorGrid, int color, int x, int y) {
    if (!p.SafeCell(x, y)) return;
    if (colorGrid[x][y] != 0) return; // Already processed.
    colorGrid[x][y] = color;

    FloodFill(p, colorGrid, color, x, y+1);
    FloodFill(p, colorGrid, color, x, y-1);
    FloodFill(p, colorGrid, color, x+1, y);
    FloodFill(p, colorGrid, color, x-1, y);
}

void Randomizer2Core::FloodFillOutside(const Puzzle& p, std::vector<std::vector<int>>& colorGrid, int x, int y) {
    if (!p.SafeCell(x, y)) return;
    if (colorGrid[x][y] != 0) return; // Already processed.
    if (x%2 != y%2 && p.grid[x][y].gap == Cell::Gap::NONE) return; // Only flood-fill through gaps
    colorGrid[x][y] = 1; // Outside color

    FloodFillOutside(p, colorGrid, x, y+1);
    FloodFillOutside(p, colorGrid, x, y-1);
    FloodFillOutside(p, colorGrid, x+1, y);
    FloodFillOutside(p, colorGrid, x-1, y);
}

// Color key:
// 0 (default): Uncolored
// 1: Outside color and separator color
// 2+: Flood-filled region color
std::tuple<std::vector<std::vector<int>>, int> Randomizer2Core::CreateColorGrid(const Puzzle& p) {
    std::vector<std::vector<int>> colorGrid;
    colorGrid.resize(p.width);

    for (int x=0; x<p.width; x++) {
        colorGrid[x].resize(p.height);
        for (int y=0; y<p.height; y++) {
            if (x%2 == 1 && y%2 == 1) continue;
            // Mark all unbroken edges and intersections as 'do not color'
            if (p.grid[x][y].gap == Cell::Gap::NONE) colorGrid[x][y] = 1;
        }
    }

    // @Future: Skip this loop if pillar = true;
    for (int y=0; y<p.height; y++) {
        FloodFillOutside(p, colorGrid, 0, y);
        FloodFillOutside(p, colorGrid, p.width - 1, y);
    }

    for (int x=0; x<p.width; x++) {
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

    return {colorGrid, color};
}