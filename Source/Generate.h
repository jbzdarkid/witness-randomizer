#pragma once
#include "Panel.h"
#include <stdlib.h>
#include <time.h>
#include <set>
#include <algorithm>

typedef std::pair<int, int> Point;
typedef std::set<Point> Shape;

class Generate
{
public:
	Generate() {
		_width = _height = _colored = 0;
		_symmetry = Panel::Symmetry::None;
	}
	void generate(int id, int symbol, int amount);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5);
	void generateMaze(int id, int width, int height, bool fullGaps);
	std::vector<std::vector<int>> getLongestPath(int length);
	std::vector<std::vector<int>> getRandomPath(int minLength, int maxLength);
	void readPanel(std::shared_ptr<Panel> panel);
	void setPath(std::vector<std::vector<int>> path);	
	void setGridSize(int width, int height);
	void setSymmetry(Panel::Symmetry symmetry);
	void setStartLocation(int x, int y);
	void setExitLocation(int x, int y);
	void resetConfig();

private:
	std::shared_ptr<Panel> _panel;
	std::vector<std::vector<int>> _cpath;
	int _width, _height, _colored;
	Panel::Symmetry _symmetry;
	std::vector<Point> _starts, _exits;
	std::set<Point> _gridpos;
	std::set<Point> _openpos;
	bool _fullGaps, _bisect;

	std::pair<int, int> to_vertex(int x, int y);
	std::pair<int, int> to_grid(int x, int y);
	std::pair<int, int> to_vertex(Point pos);
	std::pair<int, int> to_grid(Point pos);
	int get(Point pos) { return _panel->_grid[pos.first][pos.second]; }
	void set(Point pos, int val) { _panel->_grid[pos.first][pos.second] = val; }
	int get(int x, int y) { return _panel->_grid[x][y]; }
	void set(int x, int y, int val) { _panel->_grid[x][y] = val; }
	int get_symbol_type(int flags) { return flags & 0x700; }
	Point get_sym_point(Point pos) {
		return _panel->get_sym_point(pos);
	}
	void clear() {
		for (int x = 0; x < _panel->_width; x++) {
			for (int y = 0; y < _panel->_height; y++) {
				_panel->_grid[x][y] = 0;
			}
		}
		_panel->_style &= ~0x2ff8; //Remove all element flags
	}
	template <class T> T pick_random(std::vector<T>& vec) {
		return vec[rand() % vec.size()];
	}
	template <class T> T pick_random(std::set<T>& set) {
		auto it = set.begin();
		std::advance(it, rand() % set.size());
		return *it;
	}
	bool on_edge(Point p) {
		return (p.first == 0 || p.first + 1 == _panel->_width || p.second == 0 || p.second + 1 == _panel->_height);
	}
	bool off_edge(Point p) {
		return (p.first < 0 || p.first >= _panel->_width || p.second < 0 || p.second >= _panel->_height);
	}
	static std::vector<Point> _DIRECTIONS1;
	static std::vector<Point> _8DIRECTIONS1;
	static std::vector<Point> _DIRECTIONS2;
	static std::vector<Point> _8DIRECTIONS2;
	void generate(int id, std::vector<std::pair<int, int>> symbols);
	void generate_path();
	void generate_path(int minLength);
	void generate_path_regions(int minRegions);
	void generate_longest_path();
	std::set<Point> get_region(Point pos);
	std::vector<int> get_symbols_in_region(Point pos);
	std::vector<int> get_symbols_in_region(std::set<Point> region);
	bool place_start(int amount);
	bool place_exit(int amount);
	bool can_place_gap(Point pos);
	bool place_gaps(int amount);
	bool can_place_dot(Point pos);
	bool place_dots(int amount, int numColored, bool intersectionOnly);
	bool place_stones(int color, int amount);
	Shape generate_shape(std::set<Point>& region, std::set<Point>& bufferRegion, Point pos, int maxSize, bool disconnect);
	Shape generate_shape(std::set<Point>& region, Point pos, int maxSize, bool disconnect) {
		std::set<Point> buffer;
		return generate_shape(region, buffer, pos, maxSize, disconnect);
	}
	int make_shape_symbol(Shape shape, bool rotated, bool negative, int rotation);
	int make_shape_symbol(Shape shape, bool rotated, bool negative) {
		return make_shape_symbol(shape, rotated, negative, -1);
	}
	bool place_shapes(std::vector<int> colors, std::vector<int> negativeColors, int amount, int numRotated, int numNegative, bool disconnect);
	bool place_stars(int color, int amount);
	bool place_triangles(int color, int amount);
	bool place_eraser(int color, int toErase); //No "amount" variable because multiple erasers do not work consistently in the Witness
};

