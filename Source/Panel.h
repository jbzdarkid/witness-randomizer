#pragma once
#include "json.hpp"
#include "RandomizerCore.h"

class Decoration
{
public:
	enum Shape {
		Stone = 100,
		Star = 200,
		Poly = 400,
		Eraser = 500,
		Triangle = 600,
	};

	Decoration() = default;
	explicit Decoration(int shape);

	int GetValue();

//private:
	int _shape;
};

class Intersection
{
public:
	enum Flags {
		IS_ENDPOINT = 0x1,
		IS_STARTPOINT = 0x2,
		IS_GAP = 0x10000,
		HAS_DOT = 0x8,
		DOT_IS_BLUE = 0x100,
		DOT_IS_ORANGE = 0x200,
		DOT_IS_INVISIBLE = 0x1000,
	};

	Intersection() = default;
	explicit Intersection(float x, float y, int flags);

	int GetValue();

// private:
	float _x, _y;
	int _flags;
};

class Panel
{
public:
	explicit Panel(int id);
	void Write(int id);

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

	std::vector<std::vector<Decoration>> _decorations;
	std::vector<std::vector<Intersection>> _intersections;
	std::vector<Intersection> _endpoints;
	std::vector<Intersection> _gaps;
	int _style;
};