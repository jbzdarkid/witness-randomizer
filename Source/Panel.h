#pragma once
#include "json.hpp"
#include "Memory.h"
#include <stdint.h>

class Decoration
{
public:
	enum Shape {
		Exit =		0x600001,
		Start =		0x600002,
		Stone =		0x100,
		Star =		0x200,
		Poly =		0x400,
		Eraser =	0x500,
		Triangle =	0x600,
		Can_Rotate = 0x1000,
		Negative = 0x2000,
		Gap_Row = 0x300000,
		Gap_Column = 0x500000,
		Dot_Row = 0x200020,
		Dot_Column = 0x400020,
		Dot_Intersection = 0x600020,
	};
	enum Color {
		None = 0,
		Black = 0x1,
		White = 0x2,
		Red =	0x3, //Doesn't work sadly
		Purple = 0x4,
		Green = 0x5,
		Cyan = 0x6,
		Magenta = 0x7,
		Yellow = 0x8,
		Blue = 0x9,
		Orange = 0xA,
		X = 0xF,
	};
};

enum IntersectionFlags {
	ROW = 0x200000,
	COLUMN = 0x400000,
	INTERSECTION = 0x600000,
	ENDPOINT = 0x1,
	STARTPOINT = 0x2,
	OPEN = 0x3,
	GAP = 0x100000,
	DOT = 0x20,
	DOT_IS_BLUE = 0x100,
	DOT_IS_ORANGE = 0x200,
	DOT_IS_INVISIBLE = 0x1000,
};

class Endpoint {
public:
	enum Direction {
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	Endpoint(int x, int y, Direction dir, int flags) {
		_x = x;
		_y = y;
		_dir = dir;
		_flags = flags;
	}

	int GetX() {return _x;}
	void SetX(int x) {_x = x;}
	int GetY() {return _y;}
	void SetY(int y) {_y = y;}
	float GetXD() { return _xd; }
	float GetYD() { return _yd; }
	Direction GetDir() {return _dir;}
	int GetFlags() { return _flags; }
	void SetDir(Direction dir) {_dir = dir;}

private:
	int _x, _y, _flags;
	float _xd, _yd;
	Direction _dir;
};

class Panel
{
public:
	Panel(int id);

	void Write(int id);
	nlohmann::json Serialize();

	void SetSymbol(int x, int y, Decoration::Shape symbol, Decoration::Color color);
	void SetShape(int x, int y, int shape, bool rotate, bool negative, Decoration::Color color);
	void ClearSymbol(int x, int y);
	void SetGridSymbol(int x, int y, Decoration::Shape symbol, Decoration::Color color);
	void ClearGridSymbol(int x, int y);
	void Random();

	enum Style {
		SYMMETRICAL = 0x2, //Not on the town symmetry puzzles? IDK why.
		NO_BLINK = 0x4,
		HAS_DOTS = 0x8,
		IS_2COLOR = 0x10,
		HAS_STARS = 0x40,
		HAS_TRIANGLES = 0x80,
		HAS_STONES = 0x100,
		HAS_ERASERS = 0x1000,
		HAS_SHAPERS = 0x2000,
		IS_PIVOTABLE = 0x8000,
		X200000 = 0x200000, //Town Dot+Shape, Dot+Eraser, Glass Door puzzle have this flag
		X2000000 = 0x2000000, //UTM Invisible dots
		x8000000 = 0x8000000, //Bunker elevator
		x20000000 = 0x20000000, //Jungle vault
	};

private:
	// For testing
	Panel() = default;

	void ReadAllData(int id);
	void ReadIntersections(int id);
	void WriteIntersections(int id);
	void ReadDecorations(int id);
	void WriteDecorations(int id);

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

	int num_grid_points() {
		return (_width / 2 + 1) * (_height / 2 + 1);
	}

	std::shared_ptr<Memory> _memory;

	int _width, _height;

	std::vector<std::vector<int>> _grid;
	std::vector<std::pair<int, int>> _startpoints;
	std::vector<Endpoint> _endpoints;
	float minx, miny, maxx, maxy;
	int _style;

	friend class PanelExtractionTests;
};