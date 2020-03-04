#include "pch.h"
#include "Puzzle.h"

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

#include "json.hpp"
nlohmann::json newGrid(int width, int height) {
    using namespace nlohmann;

    json grid;
    for (int x=0; x<width; x++) {
        grid[x] = json::array();
        for (int y=0; y<height; y++) {
            if (x%2 == 1 && y%2 == 1) grid[x][y] = json::object();
            else grid[x][y] = {{"type", "line"}, {"color", 0}};
        }
    }
    return grid;
}

std::string Puzzle::Serialize() {
    using namespace nlohmann;

    json puzzle;
    puzzle["grid"] = {};
    for (int x=0; x<width; x++) {
        puzzle["grid"][x] = json::array();
        for (int y=0; y<height; y++) {
            if (x%2 == 1 && y%2 == 1) puzzle["grid"][x][y] = json::object();
            else puzzle["grid"][x][y] = {{"type", "line"}, {"color", 0}};
        }
    }
    puzzle["pillar"] = pillar;
    if (symmetry == Symmetry::X)        puzzle["symmetry"] = {{"x", true},  {"y", false}};
    else if (symmetry == Symmetry::Y)   puzzle["symmetry"] = {{"x", false}, {"y", true}};
    else if (symmetry == Symmetry::XY)  puzzle["symmetry"] = {{"x", true},  {"y", true}};

    for (int x=0; x<width; x++) {
        for (int y=0; y<height; y++) {
            const Cell& cell = grid[x][y];
            json j = puzzle["grid"][x][y];
            if (cell.dot != Cell::Dot::NONE) {
                if (cell.dot == Cell::Dot::BLACK)       j["dot"] = 1;
                if (cell.dot == Cell::Dot::BLUE)        j["dot"] = 2;
                if (cell.dot == Cell::Dot::YELLOW)      j["dot"] = 3;
                if (cell.dot == Cell::Dot::INVISIBLE)   j["dot"] = 4;
            } else if (cell.gap != Cell::Gap::NONE) {
                if (cell.gap == Cell::Gap::BREAK)       j["gap"] = 1;
                if (cell.gap == Cell::Gap::FULL)        j["gap"] = 2;
            } else if (cell.decoration != nullptr) {
                if (cell.decoration->color == Color::Black)         j["color"] = "black";
                else if (cell.decoration->color == Color::Blue)     j["color"] = "blue";
                else if (cell.decoration->color == Color::Cyan)     j["color"] = "cyan";
                else if (cell.decoration->color == Color::Gray)     j["color"] = "gray";
                else if (cell.decoration->color == Color::Green)    j["color"] = "green";
                else if (cell.decoration->color == Color::Orange)   j["color"] = "orange";
                else if (cell.decoration->color == Color::Pink)     j["color"] = "pink";
                else if (cell.decoration->color == Color::Purple)   j["color"] = "purple";
                else if (cell.decoration->color == Color::White)    j["color"] = "white";
                else if (cell.decoration->color == Color::Yellow)   j["color"] = "yellow";

                if (cell.decoration->type == Type::Stone) {
                    j["type"] = "square";
                } else if (cell.decoration->type == Type::Star) {
                    j["type"] = "star";
                } else if (cell.decoration->type == Type::Poly) {
                    j["type"] = "poly";
                    j["polyshape"] = cell.decoration->polyshape;
                } else if (cell.decoration->type == Type::Ylop) {
                    j["type"] = "ylop";
                    j["polyshape"] = cell.decoration->polyshape;
                } else if (cell.decoration->type == Type::Nega) {
                    j["type"] = "nega";
                } else if (cell.decoration->type == Type::Triangle) {
                    j["type"] = "triangle";
                    j["count"] = cell.decoration->count;
                }
            }

            if (cell.start) j["start"] = true;
            if (cell.end != Cell::Dir::NONE) {
                if (cell.end == Cell::Dir::LEFT)        j["end"] = "left";
                else if (cell.end == Cell::Dir::RIGHT)  j["end"] = "right";
                else if (cell.end == Cell::Dir::UP)     j["end"] = "top";
                else if (cell.end == Cell::Dir::DOWN)   j["end"] = "bottom";
            }
            puzzle["grid"][x][y] = j;
        }
    }

    return puzzle.dump();
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
