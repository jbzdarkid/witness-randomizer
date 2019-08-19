#pragma once
#include "Panel.h"
#include <stdlib.h>
#include <time.h>

class Generate
{
public:
	Generate() {
		_width = _height = _colored = 0;
		_symmetry = Panel::Symmetry::None;
	}
	void generate(int id, Decoration::Shape symbol, int amount);
	void generate(int id, Decoration::Shape symbol1, int amount1, Decoration::Shape symbol2, int amount2);
	void generate(int id, Decoration::Shape symbol1, int amount1, Decoration::Shape symbol2, int amount2, Decoration::Shape symbol3, int amount3);
	void generateMaze(int id, int width, int height, bool fullGaps);
	std::vector<std::vector<int>> getLongestPath(int length);
	std::vector<std::vector<int>> getRandomPath(int minLength, int maxLength);
	void readPanel(std::shared_ptr<Panel> panel);
	void setPath(std::vector<std::vector<int>> path);	
	void setGridSize(int width, int height);
	void setSymmetry(Panel::Symmetry symmetry);
	void setNumColoredDots(int amount);
	void setStartLocation(int x, int y);
	void setExitLocation(int x, int y);
	void resetConfig();
	void test() {
		srand(static_cast<unsigned int>(time(NULL)));
		generate_path();
	}

private:
	std::shared_ptr<Panel> _panel;
	std::vector<std::vector<int>> _cpath;
	int _width, _height, _colored;
	Panel::Symmetry _symmetry;
	std::vector<std::pair<int, int>> _starts, _exits;
	std::tuple<int> to_vertex(int x, int y);
	std::tuple<int> to_grid(int x, int y);
	std::tuple<int> to_vertex(std::pair<int, int> pos);
	std::tuple<int> to_grid(std::pair<int, int> pos);
	int get(std::pair<int, int> pos) {
		return _panel->_grid[pos.first][pos.second];
	}
	void set(std::pair<int, int> pos, int val) {
		_panel->_grid[pos.first][pos.second] = val;
	}
	void clear() {
		for (int x = 0; x < _panel->_width; x++) {
			for (int y = 0; y < _panel->_height; y++) {
				_panel->_grid[x][y] = 0;
			}
		}
	}
	static std::vector<std::pair<int, int>> DIRECTIONS;
	void generate(int id, std::vector<std::pair<int, int>> symbols);
	void generate_path();
	void generate_path(int minLength);
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

