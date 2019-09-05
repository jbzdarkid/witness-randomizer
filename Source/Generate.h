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
		_width = _height = 0;
		_areaTotal = _genTotal = _totalPuzzles = _areaPuzzles = 0;
		_handle = NULL;
		_panel = NULL;
		_parity = -1;
		resetConfig();
	}
	void generate(int id, int symbol, int amount);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5);
	void generateMaze(int id);
	void generateMaze(int id, int numStarts, int numExits);
	void initPanel(std::shared_ptr<Panel> panel);
	void setPath(std::vector<std::vector<int>> path);	
	void setSymbol(Decoration::Shape symbol, int x, int y);
	void setGridSize(int width, int height);
	void setSymmetry(Panel::Symmetry symmetry);
	void write(int id);
	void resetConfig();
	void setLoadingHandle(HWND handle) { _handle = handle; }
	void setLoadingData(int totalPuzzles) { _totalPuzzles = totalPuzzles; _genTotal = 0; }
	void setLoadingData(std::wstring areaName, int numPuzzles) { _areaName = areaName; _areaPuzzles = numPuzzles; _areaTotal = 0; }

	float pathWidth;
	int config;
	Endpoint::Direction pivotDirection;
	enum Config { FullGaps = 0x1, StartEdgeOnly = 0x2, DisableWrite = 0x4, PreserveStructure = 0x8, MakeStonesUnsolvable = 0x10, FullAreaEraser = 0x20, DisconnectShapes = 0x40, ResetColors = 0x80,
		DisableCancelShapes = 0x100, RequireCancelShapes = 0x200, KeepPath = 0x400, DisableCombineShapes = 0x800, RequireCombineShapes = 0x1000, TreehouseLayout = 0x2000, DisableReset = 0x4000,
		AlternateColors = 0x8000, //Black -> Green, White -> Pink, Purple -> White
	};

private:
	std::shared_ptr<Panel> _panel;
	std::vector<std::vector<int>> _custom_grid;
	int _width, _height;
	Panel::Symmetry _symmetry;
	std::set<Point> _starts, _exits;
	std::set<Point> _gridpos, _openpos;
	std::set<Point> _path, _path1, _path2;
	bool _fullGaps, _bisect;
	int stoneTypes;

	int _parity;
	HWND _handle;
	int _areaTotal, _genTotal, _areaPuzzles, _totalPuzzles;
	std::wstring _areaName;

	int get(Point pos) { return _panel->_grid[pos.first][pos.second]; }
	void set(Point pos, int val) { _panel->_grid[pos.first][pos.second] = val; }
	int get(int x, int y) { return _panel->_grid[x][y]; }
	void set(int x, int y, int val) { _panel->_grid[x][y] = val; }
	int get_symbol_type(int flags) { return flags & 0x700; }
	void set_path(Point pos);
	Point get_sym_point(Point pos) { return _panel->get_sym_point(pos); }
	int get_parity(Point pos) { return (pos.first / 2 + pos.second / 2) % 2; }
	void clear();
	void reset();
	void init_treehouse_layout();
	template <class T> T pick_random(std::vector<T>& vec) { return vec[rand() % vec.size()]; }
	template <class T> T pick_random(std::set<T>& set) { auto it = set.begin(); std::advance(it, rand() % set.size()); return *it; }
	bool on_edge(Point p) { return (p.first == 0 || p.first + 1 == _panel->_width || p.second == 0 || p.second + 1 == _panel->_height); }
	bool off_edge(Point p) { return (p.first < 0 || p.first >= _panel->_width || p.second < 0 || p.second >= _panel->_height); }
	static std::vector<Point> _DIRECTIONS1, _8DIRECTIONS1, _DIRECTIONS2, _8DIRECTIONS2, _SHAPEDIRECTIONS, _DISCONNECT;
	bool generate_maze(int id, int numStarts, int numExits);
	bool generate(int id, std::vector<std::pair<int, int>> symbols); //************************************************************
	bool place_all_symbols(std::vector<std::pair<int, int>>& symbols);
	bool generate_path(std::vector<std::pair<int, int>>& symbols);
	bool generate_path_length(int minLength);
	bool generate_path_regions(int minRegions);
	bool generate_longest_path();
	void erase_path();
	std::set<Point> get_region(Point pos);
	std::vector<int> get_symbols_in_region(Point pos);
	std::vector<int> get_symbols_in_region(std::set<Point> region);
	bool place_start(int amount);
	bool place_exit(int amount);
	bool can_place_gap(Point pos);
	bool place_gaps(int amount);
	bool can_place_dot(Point pos);
	bool place_dots(int amount, int color, bool intersectionOnly);
	bool place_stones(int color, int amount);
	Shape generate_shape(std::set<Point>& region, std::set<Point>& bufferRegion, Point pos, int maxSize);
	Shape generate_shape(std::set<Point>& region, Point pos, int maxSize) { std::set<Point> buffer; return generate_shape(region, buffer, pos, maxSize); }
	int make_shape_symbol(Shape shape, bool rotated, bool negative, int rotation);
	int make_shape_symbol(Shape shape, bool rotated, bool negative) { return make_shape_symbol(shape, rotated, negative, -1); }
	bool place_shapes(std::vector<int> colors, std::vector<int> negativeColors, int amount, int numRotated, int numNegative);
	bool place_stars(int color, int amount);
	bool place_triangles(int color, int amount);
	bool place_eraser(int color, int toErase); //No "amount" variable because multiple erasers do not work consistently in the Witness

	friend class PuzzleList;
	friend class Special;
};

