#pragma once

struct Decoration {
    enum Shape : int {
        Exit =              0x600001,
        Start =             0x600002,
        Stone =             0x100,
        Star =              0x200,
        Poly =              0x400,
        Eraser =            0x500,
        Triangle =          0x600,
        Triangle1 =         0x10600,
        Triangle2 =         0x20600,
        Triangle3 =         0x30600,
        Triangle4 =         0x40600,
        Arrow =             0x700,
        Arrow1 =            0x1700,
        Arrow2 =            0x2700,
        Arrow3 =            0x3700,
        Can_Rotate =        0x1000,
        Negative =          0x2000,
        Gap =               0x100000,
        Gap_Row =           0x300000,
        Gap_Column =        0x500000,
        Dot =               0x20,
        Dot_Row =           0x240020,
        Dot_Column =        0x440020,
        Dot_Intersection =  0x600020,
        Empty =             0xA00,
    };
    enum Color : int {
        None =      0x0,
        Black =     0x1,
        White =     0x2,
        Red =       0x3,
        Purple =    0x4,
        Green =     0x5,
        Cyan =      0x6,
        Magenta =   0x7,
        Yellow =    0x8,
        Blue =      0x9,
        Orange =    0xA,
    };
};

enum IntersectionFlags : int {
    ENDPOINT =          0x1,
    STARTPOINT =        0x2,
    NO_POINT =          0x8,
    DOT =               0x20,
    DOT_IS_BLUE =       0x100,
    DOT_IS_ORANGE =     0x200,
    DOT_IS_INVISIBLE =  0x1000,
    DOT_SMALL =         0x2000,
    DOT_MEDIUM =        0x4000,
    DOT_LARGE =         0x8000,
    GAP =               0x100000,
    ROW =               0x200000,
    COLUMN =            0x400000,
    INTERSECTION =      ROW | COLUMN,
};

class Endpoint {
public:
    enum Direction {
        NONE =  0,
        LEFT =  1,
        RIGHT = 2,
        UP =    4,
        DOWN =  8,
        UP_LEFT = UP | LEFT,
        UP_RIGHT = UP | RIGHT,
        DOWN_LEFT = DOWN | LEFT,
        DOWN_RIGHT = DOWN | RIGHT,
    };
};
