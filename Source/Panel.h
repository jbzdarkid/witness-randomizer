#pragma once
#include "json.hpp"
#include "Memory.h"

class Decoration
{
public:
	enum Shape {
		Stone =		0x100,
		Star =		0x200,
		Poly =		0x400,
		Eraser =	0x500,
		Triangle =	0x600,
	};
	enum Color {
		Black = 0x1,
		White = 0x2,
		Red =	0x3,
		Blue =	0x4,
		Green = 0x5,
	};

	static nlohmann::json to_json(int decoration) {
		nlohmann::json json = {};
		int shape = decoration & 0x00000F00;
		if (shape == Shape::Stone)		json["type"] = "square";
		if (shape == Shape::Star)		json["type"] = "star";
		if (shape == Shape::Poly)		json["type"] = "poly";
		if (shape == Shape::Eraser)		json["type"] = "eraser";
		if (shape == Shape::Triangle)	json["type"] = "triangle";

		int color = decoration & 0x0000000F;
		if (color == Color::Black)	json["color"] = "black";
		if (color == Color::White)	json["color"] = "white";
		if (color == Color::Red)	json["color"] = "red";
		if (color == Color::Blue)	json["color"] = "blue";
		if (color == Color::Green)	json["color"] = "green";

		if (json.empty()) return false;
		return json;
	}
};

enum IntersectionFlags {
	IS_ENDPOINT = 0x1,
	IS_STARTPOINT = 0x2,
	IS_GAP = 0x10000,
	HAS_DOT = 0x20,
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
		if (_dir == LEFT) json["dir"] = "left";
		if (_dir == RIGHT) json["dir"] = "right";
		if (_dir == UP) json["dir"] = "up";
		if (_dir == DOWN) json["dir"] = "down";
		return json;
	}

private:
	int _x, _y;
	Direction _dir;
};

class Panel
{
public:
	explicit Panel(int id);
	// explicit Panel(nlohmann::json json);

	void Write(int id);
	nlohmann::json Serialize();

	void Random();

	enum Style {
		SYMMETRICAL = 0x2,
		IS_2COLOR = 0x10,
		HAS_DOTS = 0x4,
		HAS_STARS = 0x40,
		HAS_STONES = 0x100,
		HAS_ERASERS = 0x1000,
		HAS_SHAPERS = 0x2000,
	};

private:
	void ReadIntersections(int id);
	void WriteIntersections(int id);
	void ReadDecorations(int id);
	void WriteDecorations(int id);

	// TODO: Reflection data
	// TODO: Decoration colors

	Memory _memory = Memory("witness64_d3d11.exe");

	int _width, _height;

	std::vector<std::vector<int>> _grid;
	std::vector<Endpoint> _endpoints;
	std::vector<std::pair<int ,int>> _startpoints;
	int _style;
};