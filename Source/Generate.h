#pragma once
#include "Panel.h"

class Generate
{
public:
	void generate(int id, int width, int height, Decoration::Shape symbol, int amount);
	void generate(int id, int width, int height, Decoration::Shape symbol1, int amount1, Decoration::Shape symbol2, int amount2);
	void generate(int id, int width, int height, Decoration::Shape symbol1, int amount1, Decoration::Shape symbol2, int amount2, Decoration::Shape symbol3, int amount3);
	void generateMaze(int id, int width, int height, bool fullGaps);
	std::vector<std::vector<int>> getRandomPath();
	std::vector<std::vector<int>> getRandomPath(int minLength, int maxLength);
	void setPath(std::vector<std::vector<int>>);
	void set1Color();
	void set2Color();
	void set3Color();

private:
	Panel _panel;
	int _dual_color_amount;
	std::vector<std::vector<int>> _path;
	void generate(int id, int width, int height, std::vector<std::tuple<int, int>> symbols);
	void generate_path();
	void generate_path(int minLength, int maxLength);
	void generate_longest_path();
	std::vector<int> get_symbols_in_region(int x, int y);
	std::vector<std::tuple<int>> get_region(int x, int y);
	void place_start(int amount);
	void place_exit(int amount);
	void place_gaps(int amount);
	void place_dots(int amount, int num_colored);
	void place_stones(int color, int amount);
	void place_shapes(std::vector<int> colors, int amount, int numRotated, int numNegative, bool disconnect);
	void place_stars(int color, int amount);
	void place_triangles(int color, int amount);
	void place_eraser(int color);
};

