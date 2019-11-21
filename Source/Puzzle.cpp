#include "Puzzle.h"
#include "Memory.h"
#include <cassert>

Cell Puzzle::GetCell(int x, int y) const {
    x = Mod(x);
    if (!SafeCell(x, y)) return Cell::Undefined();
    return grid[x][y];
}

Cell::Color Puzzle::GetLine(int x, int y) const {
    return grid[x][y].color;
}

void Puzzle::NewGrid(int newWidth, int newHeight) {
    if (newWidth == 0) {
        assert(false);
        newWidth = width;
        newHeight = height;
    } else {
        // @Cleanup! This should be in the ctor...
        width = 2*newWidth + 1;
        height = 2*newHeight + 1;
    }
    grid.clear();
    grid.resize(width);
    for (int x=0; x<width; x++) grid[x].resize(height);
}

Pos Puzzle::GetSymmetricalPos(int x, int y) const {
    if (symmetry != Symmetry::NONE) {
        if (pillar) {
            x += width/2;
            if (symmetry & Symmetry::X) {
                x = width - x;
            }
        } else {
            if (symmetry & Symmetry::X) {
                x = (width-1) - x;
            }
        }
        if (symmetry & Symmetry::Y) {
            y = (height-1) - y;
        }
    }
    return Pos{x, y};
}

int Puzzle::Mod(int x) const {
    if (!pillar) return x;
    return (x + width * height * 2) % width;
}

bool Puzzle::SafeCell(int x, int y) const {
    if (x < 0 || x >= width) return false;
    if (y < 0 || y >= height) return false;
    return true;
}
