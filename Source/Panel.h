#pragma once
#include "json.hpp"
#include "Memory.h"
#include <stdint.h>

typedef std::pair<int, int> Point;


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
		Triangle1 = 0x10600,
		Triangle2 = 0x20600,
		Triangle3 = 0x30600,
		Can_Rotate = 0x1000,
		Negative = 0x2000,
		Gap = 0x100000,
		Gap_Row = 0x300000,
		Gap_Column = 0x500000,
		Dot = 0x20,
		Dot_Row = 0x240020,
		Dot_Column = 0x440020,
		Dot_Intersection = 0x600020,
		Empty = 0xA00,
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
	OPEN = 0x3, //Puzzle loader flag - not to be written out
	PATH = 0x4, //Generator use only
	NO_POINT = 0x8, //Points that nothing connects to
	GAP = 0x100000,
	DOT = 0x20,
	DOT_IS_BLUE = 0x100,
	DOT_IS_ORANGE = 0x200,
	DOT_IS_INVISIBLE = 0x1000,
	DOT_SMALL = 0x2000,
	DOT_MEDIUM = 0x4000,
	DOT_LARGE = 0x8000,
};

class Endpoint {
public:
	enum Direction {
		LEFT,
		RIGHT,
		UP,
		DOWN,
		DIAGONAL
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
	Direction GetDir() {return _dir;}
	int GetFlags() { return _flags; }
	void SetDir(Direction dir) {_dir = dir;}

private:
	int _x, _y, _flags;
	Direction _dir;
};

struct Color {
	float r;
	float g;
	float b;
	float a;
	friend bool operator <(const Color& lhs, const Color& rhs) {return lhs.r * 8 + lhs.g * 4 + lhs.b * 2 + lhs.a > rhs.r * 8 + rhs.g * 4 + rhs.b * 2 + rhs.a;}
};

class Panel
{
public:
	Panel();
	Panel(int id);

	void Read();
	void Read(int id) { this->id = id; Read(); }
	void Write();
	void Write(int id) { this->id = id; Write(); }

	void SetSymbol(int x, int y, Decoration::Shape symbol, Decoration::Color color);
	void SetShape(int x, int y, int shape, bool rotate, bool negative, Decoration::Color color);
	void ClearSymbol(int x, int y);
	void SetGridSymbol(int x, int y, Decoration::Shape symbol, Decoration::Color color);
	void ClearGridSymbol(int x, int y);
	void Resize(int width, int height);

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
	};

	enum Symmetry { //NOTE - Not all of these are valid symmetries for certain puzzles
		None, Horizontal, Vertical, Rotational,
		RotateLeft, RotateRight, FlipXY, FlipNegXY, ParallelH, ParallelV, ParallelHFlip, ParallelVFlip
	};
	Symmetry symmetry;

	float pathWidth;
	bool writeColors, decorationsOnly;

private:

	void ReadAllData();
	void ReadIntersections();
	void WriteIntersections();
	void ReadDecorations();
	void WriteDecorations();

	Point get_sym_point(int x, int y, Symmetry symmetry)
	{
		switch (symmetry) {
		case None: return Point(x, y);
		case Symmetry::Horizontal: return Point(x, _height - 1 - y);
		case Symmetry::Vertical: return Point(_width - 1 - x, y);
		case Symmetry::Rotational: return Point(_width - 1 - x, _height - 1 - y);
		case Symmetry::RotateLeft: return Point(y, _width - 1 - x);
		case Symmetry::RotateRight: return Point(_height - 1 - y, x);
		case Symmetry::FlipXY: return Point(y, x);
		case Symmetry::FlipNegXY: return Point(_height - 1 - y, _width - 1 - x);
		case Symmetry::ParallelH: return Point(x, (y + (_height + 1) / 2) % _height);
		case Symmetry::ParallelV: return Point((x + (_width + 1) / 2) % _width, y);
		case Symmetry::ParallelHFlip: return Point(_width - 1 - x, (y + (_height + 1) / 2) % _height);
		case Symmetry::ParallelVFlip: return Point((x + (_width + 1) / 2) % _width, _height - 1 - y);
		}
		return Point(x, y);
	}

	Point get_sym_point(int x, int y) { return get_sym_point(x, y, symmetry); }
	Point get_sym_point(Point p) { return get_sym_point(p.first, p.second, symmetry); }
	Point get_sym_point(Point p, Symmetry symmetry) { return get_sym_point(p.first, p.second, symmetry); }
	int get_num_grid_points() { return (_width / 2 + 1) * (_height / 2 + 1);  }
	int get_num_grid_blocks() { return (_width / 2) * (_height / 2);  }
	int get_parity() { return (get_num_grid_points() + 1) % 2; }
	Color get_color_rgb(int color) {
		switch (color) {
		case Decoration::Color::Black: return { 0, 0, 0, 1 };
		case Decoration::Color::White: return { 1, 1, 1, 1 };
		case Decoration::Color::Red: return { 1, 0, 0, 1 };
		case Decoration::Color::Green: return { 0, 1, 0, 1 };
		case Decoration::Color::Blue: return { 0, 0, 1, 1 };
		case Decoration::Color::Cyan: return { 0, 1, 1, 1 };
		case Decoration::Color::Magenta: return { 1, 0, 1, 1 };
		case Decoration::Color::Yellow: return { 1, 1, 0, 1 };
		case Decoration::Color::Orange: return { 1, 0.5, 0, 1 };
		case Decoration::Color::Purple: return { 0.5, 0, 1, 1 };
		default: return { 0, 0, 0, 0 };
		}
	}

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

	std::vector<int> sym_data_v() {
		std::vector<int> data;
		for (int y = 0; y <= _height / 2; y++) {
			for (int x = _width / 2 ; x >= 0; x--) {
				data.push_back(y * (_width / 2 + 1) + x);
			}
		}
		return data;
	}

	std::vector<int> sym_data_h() {
		std::vector<int> data;
		for (int y = _height / 2; y >= 0; y--) {
			for (int x = 0; x <= _width / 2; x++) {
				data.push_back(y * (_width / 2 + 1) + x);
			}
		}
		return data;
	}

	std::vector<int> sym_data_r() {
		std::vector<int> data;
		for (int i = get_num_grid_points() - 1; i >= 0; i--) {
			data.push_back(i);
		}
		return data;
	}

	int locate_segment(int x, int y, std::vector<int>& connections_a, std::vector<int>& connections_b) {
		for (int i = 0; i < connections_a.size(); i++) {
			auto[x1, y1] = loc_to_xy(connections_a[i]);
			auto[x2, y2] = loc_to_xy(connections_b[i]);
			if ((x1 == x - 1 && x2 == x + 1 && y1 == y && y2 == y) ||
				(y1 == y - 1 && y2 == y + 1 && x1 == x && x2 == x)) {
				return i;
			}
		}
		return -1;
	}

	void break_segment(int x, int y, std::vector<int>& connections_a, std::vector<int>& connections_b, std::vector<float>& intersections, std::vector<int>& intersectionFlags) {
		int i = locate_segment(x, y, connections_a, connections_b);
		if (i == -1) {
			return;
		}
		int other_connection = connections_b[i];
		connections_b[i] = static_cast<int>(intersectionFlags.size());
		connections_a.push_back(static_cast<int>(intersectionFlags.size()));
		connections_b.push_back(other_connection);
		intersections.push_back(static_cast<float>(minx + x * unitWidth));
		intersections.push_back(static_cast<float>(miny + (_height - 1 - y) * unitHeight));
		intersectionFlags.push_back(_grid[x][y]);
	}

	void break_segment_gap(int x, int y, std::vector<int>& connections_a, std::vector<int>& connections_b, std::vector<float>& intersections, std::vector<int>& intersectionFlags) {
		int i = locate_segment(x, y, connections_a, connections_b);
		if (i == -1) {
			return;
		}
		int other_connection = connections_b[i];
		connections_b[i] = static_cast<int>(intersectionFlags.size() + 1);
		connections_a.push_back(other_connection);
		connections_b.push_back(static_cast<int>(intersectionFlags.size()));
		if (!(_grid[x][y] & IntersectionFlags::GAP)) {
			_grid[x][y] |= (x % 2 == 0 ? IntersectionFlags::COLUMN : IntersectionFlags::ROW);
			connections_a.push_back(static_cast<int>(intersectionFlags.size()));
			connections_b.push_back(static_cast<int>(intersectionFlags.size() + 1));
		}
		double xOffset = _grid[x][y] & IntersectionFlags::ROW ? 0.5 : 0;
		double yOffset = _grid[x][y] & IntersectionFlags::COLUMN ? 0.5 : 0;
		intersections.push_back(static_cast<float>(minx + (x + xOffset) * unitWidth));
		intersections.push_back(static_cast<float>(miny + (_height - 1 - y - yOffset) * unitHeight));
		intersections.push_back(static_cast<float>(minx + (x - xOffset) * unitWidth));
		intersections.push_back(static_cast<float>(miny + (_height - 1 - y + yOffset) * unitHeight));
		intersectionFlags.push_back(_grid[x][y]);
		intersectionFlags.push_back(_grid[x][y]);
	}

	std::shared_ptr<Memory> _memory;

	int _width, _height;

	std::vector<std::vector<int>> _grid;
	std::vector<Point> _startpoints;
	std::vector<Endpoint> _endpoints;
	float minx, miny, maxx, maxy, unitWidth, unitHeight;
	int _style;
	bool _resized;
	int id;

	friend class PanelExtractionTests;
	friend class Generate;
	friend class PuzzleList;
	friend class Special;
	friend class MultiGenerate;
};