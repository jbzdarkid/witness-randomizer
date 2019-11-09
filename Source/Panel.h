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
    int polyshape = 0;
    // For triangles only
    int count = 0;
};

struct Cell {
    bool start = false;
    enum class Dir {NONE, LEFT, RIGHT, UP, DOWN};
    Dir end = Dir::NONE;
    std::shared_ptr<Decoration> decoration = nullptr;
    enum class Dot {NONE, BLACK, BLUE, YELLOW, INVISIBLE};
    Dot dot = Dot::NONE;
    enum class Gap {NONE, BREAK, FULL};
    Gap gap = Gap::NONE;
};

struct Puzzle {
    int16_t height;
    int16_t width;
    bool hasDecorations = false;
    std::vector<std::vector<Cell>> grid;

    enum class Symmetry {NONE, X, Y, XY};
    Symmetry sym = Symmetry::NONE;
    bool pillar = false;
};

class PuzzleSerializer {
public:
    PuzzleSerializer(const std::shared_ptr<Memory>& memory);
    Puzzle ReadPuzzle(int id);
    void WritePuzzle(const Puzzle& p, int id);

private:
    // @Bug: Blue and orange are swapped?
    enum Flags {
        IS_ENDPOINT =            0x1,
        IS_STARTPOINT =          0x2,
        IS_FULL_GAP =            0x8,
        HAS_DOT =               0x20,
        DOT_IS_BLUE =          0x100,
        DOT_IS_ORANGE =        0x200,
        DOT_IS_INVISIBLE =    0x1000,
        HAS_NO_CONN =       0x100000,
        HAS_VERTI_CONN =    0x200000,
        HAS_HORIZ_CONN =    0x400000,
    };

    void ReadIntersections(Puzzle& p, int id);
    void ReadDecorations(Puzzle& p, int id);
    void WriteIntersections(const Puzzle& p, int id);
    void WriteDecorations(const Puzzle& p, int id);

    std::tuple<int, int> loc_to_xy(const Puzzle& p, int location) const;
    int xy_to_loc(const Puzzle& p, int x, int y) const;
    // Decoration location
    std::tuple<int, int> dloc_to_xy(const Puzzle& p, int location) const;
    int xy_to_dloc(const Puzzle& p, int x, int y) const;
    Cell::Dot FlagsToDot(int flags) const;
    // Iterate connection lists for another location which is connected to us; return that other location.
    int FindConnection(int i, const std::vector<int>& connections_a, const std::vector<int>& connections_b) const;

    std::shared_ptr<Memory> _memory;
};
