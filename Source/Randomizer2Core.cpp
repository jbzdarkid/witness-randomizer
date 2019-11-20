#include "Randomizer2Core.h"
#include "Puzzle.h"
#include "Random.h"

#include <string>
#include <iostream>
#include <cassert>

std::vector<Pos> Randomizer2Core::CutEdges(const Puzzle& p, size_t numEdges, bool allowEdges) {
    std::vector<Pos> edges;
    int xMin = allowEdges ? 0 : 1;
    int xMax = allowEdges ? p.width : p.width-1;
    int yMin = allowEdges ? 0 : 1;
    int yMax = allowEdges ? p.height : p.height-1;

    for (int x=xMin; x<xMax; x++) {
        for (int y=yMin; y<yMax; y++) {
            if (x%2 == y%2) continue;
            if (p.grid[x][y].gap != Cell::Gap::NONE) continue;
            if (p.grid[x][y].start) continue;
            if (p.grid[x][y].end != Cell::Dir::NONE) continue;

            // If the puzzle already has a sequence, don't cut along it.
            bool inSequence = false;
            for (Pos pos : p.sequence) inSequence |= (pos.x == x && pos.y == y);
            if (inSequence) continue;
            edges.emplace_back(x, y);
        }
    }
    return CutEdgesInternal(p, edges, numEdges);
}

std::vector<Pos> Randomizer2Core::CutEdgesInternal(const Puzzle& p, std::vector<Pos>& edges, size_t numEdges) {
    auto [colorGrid, numColors] = CreateColorGrid(p);
    assert(numEdges <= numColors);

    std::vector<Pos> cutEdges;
    for (int i=0; i<numEdges; i++) {
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
    for (int y=0; y<colorGrid[0].size(); y++) {
        std::string row;
        for (int x=0; x<colorGrid.size(); x++) {
            row += std::to_string(colorGrid[x][y]);
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

/*
    undefined -> 1 (color of outside) or * (any colored cell) or -1 (edge/intersection not part of any region)

    0 -> {} (this is a special edge case, which I don't need right now)
    1 -> 0 (uncolored / ready to color)
    2 -> 
*/
std::tuple<std::vector<std::vector<int>>, int> Randomizer2Core::CreateColorGrid(const Puzzle& p) {
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

    return {colorGrid, color};
}