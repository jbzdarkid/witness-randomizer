#pragma once
#include "Generate.h"
#include "Random.h"

//Class for generating puzzles with multiple solutions.
class MultiGenerate
{
public:

	MultiGenerate() { splitStones = false; }
	~MultiGenerate() { }

	std::vector<std::shared_ptr<Generate>> generators;

	void generate(int id, std::vector<std::shared_ptr<Generate>> gens, std::vector<std::pair<int, int>> symbolVec);

	bool splitStones;

private:

	bool generate(int id, PuzzleSymbols symbols);
	bool place_all_symbols(PuzzleSymbols symbols);
	bool can_place_gap(Point pos);
	bool place_stones(int color, int amount);
	bool place_stars(int color, int amount);
	bool can_place_triangle(Point pos);
	bool place_triangles(int color, int amount);

	template <class T> T pick_random(std::vector<T>& vec) { return vec[Random::rand() % vec.size()]; }
	template <class T> T pick_random(std::set<T>& set) { auto it = set.begin(); std::advance(it, Random::rand() % set.size()); return *it; }

	friend class Special;
};

