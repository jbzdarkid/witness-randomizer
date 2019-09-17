#pragma once
#include "Generate.h"

class MultiGenerate
{
public:

	MultiGenerate();
	~MultiGenerate();

	std::vector<std::shared_ptr<Generate>> generators;

	void generate(int id, std::vector<std::shared_ptr<Generate>> gens, std::vector<std::pair<int, int>> symbolVec);

private:

	bool generate(int id, Generate::PuzzleSymbols symbols);
	bool place_all_symbols(Generate::PuzzleSymbols symbols);
	bool can_place_gap(Point pos);
	bool place_gaps(int amount);
	bool place_dots(int amount, bool intersectionOnly);
	bool place_stones(int color, int amount);
	bool place_stars(int color, int amount);
	bool can_place_triangle(Point pos);
	bool place_triangles(int color, int amount);

	template <class T> T pick_random(std::vector<T>& vec) { return vec[rand() % vec.size()]; }
	template <class T> T pick_random(std::set<T>& set) { auto it = set.begin(); std::advance(it, rand() % set.size()); return *it; }

};

