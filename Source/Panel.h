#pragma once
#include <memory>
#include <vector>

class Memory;

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

struct Decoration {
    Shape type;
    Color color;
    int polyshape;
    // For triangles only
    int count;
};

struct Cell {
    bool start;
    enum class Dir {NONE, LEFT, RIGHT, UP, DOWN};
    Dir end;
    std::shared_ptr<Decoration> decoration;
    enum class Dot {NONE, BLACK, BLUE, YELLOW, INVISIBLE};
    Dot dot;
    enum class Gap {NONE, BREAK, FULL};
    Gap gap;
};

struct Puzzle {
    int16_t height;
    int16_t width;
    std::vector<std::vector<Cell>> grid;

    enum class Symmetry {NONE, X, Y, XY};
    Symmetry sym;
    bool pillar;
};

class PuzzleSerializer {
public:
    PuzzleSerializer(const std::shared_ptr<Memory>& memory);
    Puzzle ReadPuzzle(int id);
    void WritePuzzle(const Puzzle& p, int id);

private:
    // @Bug: Blue and orange are swapped?
    enum Flags {
        IS_ENDPOINT =           0x1,
        IS_STARTPOINT =         0x2,
        DOT_IS_BLUE =         0x100,
        DOT_IS_ORANGE =       0x200,
        DOT_IS_INVISIBLE =   0x1000,
        IS_GAP =            0x10000,
        HAS_DOT =           0x40020,
    };

    void ReadIntersections(Puzzle& p, int id);
    void ReadDecorations(Puzzle& p, int id);
    void WriteIntersections(const Puzzle& p, int id);
    void WriteDecorations(const Puzzle& p, int id);

    std::tuple<int, int> loc_to_xy(const Puzzle& p, int location);
    int xy_to_loc(const Puzzle& p, int x, int y);
    // Decoration location
    std::tuple<int, int> dloc_to_xy(const Puzzle& p, int location);
    int xy_to_dloc(const Puzzle& p, int x, int y);
    Cell::Dot FlagsToDot(int flags);

    std::shared_ptr<Memory> _memory;
};
