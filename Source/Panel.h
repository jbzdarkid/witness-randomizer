#pragma once
#include "json.hpp"
#include "Memory.h"

enum IntersectionFlags {
    IS_ENDPOINT =       0x1,
    IS_STARTPOINT =     0x2,
    IS_GAP =            0x10000,
    HAS_DOT =           0x40020,
    DOT_IS_BLUE =       0x100,
    DOT_IS_ORANGE =     0x200,
    DOT_IS_INVISIBLE =  0x1000,
};

/*
    enum Style {
        SYMMETRICAL = 0x2,
        IS_2COLOR = 0x10,
        HAS_DOTS = 0x4,
        HAS_STARS = 0x40,
        HAS_STONES = 0x100,
        HAS_ERASERS = 0x1000,
        HAS_SHAPERS = 0x2000,
    };
*/

class Endpoint {
public:
    enum class Direction {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    Endpoint(int x, int y, Direction dir) {
        _x = x;
        _y = y;
        _dir = dir;
    }

    int GetX() {return _x;}
    void SetX(int x) {_x = x;}
    int GetY() {return _y;}
    void SetY(int y) {_y = y;}
    Direction GetDir() {return _dir;}
    void SetDir(Direction dir) {_dir = dir;}

    nlohmann::json to_json() {
        nlohmann::json json = {{"x", _x}, {"y", _y}};
        if (_dir == Direction::LEFT) json["dir"] = "left";
        if (_dir == Direction::RIGHT) json["dir"] = "right";
        if (_dir == Direction::UP) json["dir"] = "up";
        if (_dir == Direction::DOWN) json["dir"] = "down";
        return json;
    }

private:
    int _x, _y;
    Direction _dir;
};

class Panel {
public:
    Panel(const std::shared_ptr<Memory>& memory, int id);
    // explicit Panel(nlohmann::json json);

    void Write(int id);
    nlohmann::json Serialize();

private:
    // For testing
    Panel() = default;

    void ReadIntersections(int id);
    void WriteIntersections(int id);
    void ReadDecorations(int id);
    void WriteDecorations(int id);

    // TODO: Reflection data
    // TODO: Decoration colors

    std::tuple<int, int> loc_to_xy(int location) {
        int height2 = (_height - 1) / 2;
        int width2 = (_width + 1) / 2;

        int x = 2 * (location % width2);
        int y = 2 * (height2 - location / width2);
        return {x, y};
    }

    int xy_to_loc(int x, int y) {
        int height2 = (_height - 1) / 2;
        int width2 = (_width + 1) / 2;

        int rowsFromBottom = height2 - y/2;
        return rowsFromBottom * width2 + x/2;
    }

    std::tuple<int, int> dloc_to_xy(int location) {
        int height2 = (_height - 3) / 2;
        int width2 = (_width - 1) / 2;

        int x = 2 * (location % width2) + 1;
        int y = 2 * (height2 - location / width2) + 1;
        return {x, y};
    }

    int xy_to_dloc(int x, int y) {
        int height2 = (_height - 3) / 2;
        int width2 = (_width - 1) / 2;

        int rowsFromBottom = height2 - (y - 1)/2;
        return rowsFromBottom * width2 + (x - 1)/2;
    }

    std::shared_ptr<Memory> _memory;

    int _width, _height;

    std::vector<std::vector<int>> _grid;
    std::vector<Endpoint> _endpoints;
    std::vector<std::pair<int ,int>> _startpoints;
    int _style;

    friend class PanelExtractionTests;
};

// V2 stuff here
struct Decoration {
    enum class Type {STONE, STAR, POLY, ERASER, TRIANGLE, RPOLY, YLOP};
    Type type;
    // TODO: Color color;
    uint32_t polyshape;
    int count; // For triangles
};

struct Cell {
    bool start;
    enum class Dir {NONE, LEFT, RIGHT, UP, DOWN};
    Dir end;
    Decoration* decoration;
    enum class Dot {NONE, BLACK, BLUE, YELLOW, INVISIBLE};
    Dot dot;
    enum class Gap {NONE, BREAK, FULL};
    Gap gap;
};

struct Puzzle {
    int16_t height;
    int16_t width;
    Cell** grid;

    enum class Symmetry {NONE, X, Y, XY};
    Symmetry sym;
    bool pillar;
};

class PuzzleSerializer {
public:
    PuzzleSerializer(const std::shared_ptr<Memory>& memory);
    Puzzle ReadPuzzle(int panelId);
    void WritePuzzle(int panelId, const Puzzle& puzzle);

//private:
    enum Shape {
        Stone =      0x100,
        Star =       0x200,
        Poly =       0x400,
        Eraser =     0x500,
        Triangle =   0x600,
        RPoly =     0x1000,
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

    enum Flags {
        IS_ENDPOINT =           0x1,
        IS_STARTPOINT =         0x2,
        DOT_IS_BLUE =         0x100,
        DOT_IS_ORANGE =       0x200,
        DOT_IS_INVISIBLE =   0x1000,
        IS_GAP =            0x10000,
        HAS_DOT =           0x40020,
    };

    std::shared_ptr<Memory> _memory;
};

static nlohmann::json Decoration_to_json(int decoration) {
    nlohmann::json json = {};
    int shape = decoration & 0x00000F00;
    if (shape == PuzzleSerializer::Shape::Stone)      json["type"] = "square";
    if (shape == PuzzleSerializer::Shape::Star)       json["type"] = "star";
    if (shape == PuzzleSerializer::Shape::Poly)       json["type"] = "poly";
    if (shape == PuzzleSerializer::Shape::Eraser)     json["type"] = "eraser";
    if (shape == PuzzleSerializer::Shape::Triangle)   json["type"] = "triangle";

    int color = decoration & 0x0000000F;
    if (color == PuzzleSerializer::Color::Black)      json["color"] = "black";
    if (color == PuzzleSerializer::Color::White)      json["color"] = "white";
    if (color == PuzzleSerializer::Color::Gray)       json["color"] = "gray";
    if (color == PuzzleSerializer::Color::Blue)       json["color"] = "blue";
    if (color == PuzzleSerializer::Color::Green)      json["color"] = "green";

    if (json.empty()) return false;
    return json;
}
