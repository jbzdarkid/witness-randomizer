#pragma once
#include <memory>
#include <vector>

class Memory;

enum Type {
    Stone =      0x100,
    Square = Stone, // @Deprecated
    Star =       0x200,
    Poly =       0x400,
    Eraser =     0x500,
    Nega = Eraser, // @Deprecated
    Triangle =   0x600,
    RPoly =     0x1000, // @Cleanup!
    Ylop =      0x2000,
};

enum Color {
    Black =     0x1,
    White =     0x2,
    Gray =      0x3,
    Purple =    0x4,
    Green =     0x5,
    Cyan =      0x6,
    Pink =      0x7,
    Yellow =    0x8,
    Blue =      0x9,
    Orange =    0xA,
};

struct Decoration {
    Type type;
    Color color;
    int polyshape = 0;
    // For triangles only
    int count = 0;
};

struct Cell {
    enum class Dot {NONE, BLACK, BLUE, YELLOW, INVISIBLE};
    Dot dot = Dot::NONE;
    enum class Gap {NONE, BREAK, FULL};
    Gap gap = Gap::NONE;
    // Line color
    enum class Color {NONE, BLACK, BLUE, YELLOW};
    Color color = Color::NONE;

    std::shared_ptr<Decoration> decoration = nullptr;

    bool start = false;
    enum class Dir {NONE, LEFT, RIGHT, UP, DOWN};
    Dir end = Dir::NONE;

    inline static Cell Undefined() {
        Cell c;
        c.undefined = true;
        return c;
    }
    bool undefined = false;
};

struct Negation {};
struct Pos {
    Pos() = default; // Required for use in std::maps
    Pos(int x_, int y_) : x(x_), y(y_) {}
    Pos(const std::tuple<int, int>& xy) : x(std::get<0>(xy)), y(std::get<1>(xy)) {}
    bool operator==(Pos other) {return x == other.x && y == other.y;}
    int x = 0;
    int y = 0;
};

class Puzzle {
public:
    int height = 0;
    int width = 0;
    bool hasDecorations = false;

    enum Symmetry {
        NONE = 0,
        X = 1,
        Y = 2,
        XY = 3
    };
    Symmetry symmetry = Symmetry::NONE;
    bool pillar = false;

    bool valid = false;
    std::vector<Negation> negations;
    std::vector<Pos> invalidElements;

    std::vector<Pos> sequence;

    Cell GetCell(int x, int y) const;
    Cell::Color GetLine(int x, int y) const;
    void NewGrid(int newWidth, int newHeight);
    Pos GetSymmetricalPos(int x, int y) const;

// private:
    std::vector<std::vector<Cell>> grid;

// private:
    int Mod(int x) const;
    bool SafeCell(int x, int y) const;
};
