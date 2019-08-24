#include "Generate.h"
#include "Randomizer.h"

void Generate::generate(int id, int symbol, int amount) {
	generate(id, { std::make_pair(symbol, amount) });
}

void Generate::generate(int id, int symbol1, int amount1, int symbol2, int amount2) {
	generate(id, { std::make_pair(symbol1, amount1), std::make_pair(symbol2, amount2) });
}

void Generate::generate(int id, int symbol1, int amount1,  int symbol2, int amount2, int symbol3, int amount3) {
	generate(id, { std::make_pair(symbol1, amount1), std::make_pair(symbol2, amount2), std::make_pair(symbol3, amount3) });
}

void Generate::generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4) {
	generate(id, { std::make_pair(symbol1, amount1), std::make_pair(symbol2, amount2), std::make_pair(symbol3, amount3), std::make_pair(symbol4, amount4) });
}

Point operator+(const Point& l, const Point& r) {
	return { l.first + r.first, l.second + r.second };
}

std::vector<Point> Generate::DIRECTIONS = { Point(0, 2), Point(0, -2), Point(2, 0), Point(-2, 0) };
std::vector<Point> Generate::_8DIRECTIONS = { Point(0, 2), Point(0, -2), Point(2, 0), Point(-2, 0), Point(2, 2), Point(2, -2), Point(-2, -2), Point(-2, 2) };

void Generate::readPanel(std::shared_ptr<Panel> panel) {
	_panel = panel;
	_starts = _panel->_startpoints;
	_exits.clear();
	for (Endpoint e : _panel->_endpoints) {
		_exits.push_back(Point(e.GetX(), e.GetY()));
	}
	_gridpos.clear();
	for (int x = 1; x < panel->_width; x += 2) {
		for (int y = 1; y < panel->_height; y += 2) {
			_gridpos.insert(Point(x, y));
		}
	}
	_fullGaps = false;
	for (int x = 0; x < panel->_width; x++) {
		for (int y = 0; y < panel->_height; y++) {
			if (panel->_grid[x][y] == OPEN) {
				_fullGaps = true;
				return;
			}
		}
	}
}

void Generate::setGridSize(int width, int height) {
	_width = width * 2 + 1; _height = height * 2 + 1;
}

//----------------------Private--------------------------

void Generate::generate(int id, std::vector<std::pair<int, int>> symbols) {

	std::shared_ptr<Panel> _panel = std::make_shared<Panel>(id);

	if (_width > 0 && _height > 0 && (_width != _panel->_width || _height != _panel->_height)) {
		for (Point &s : _panel->_startpoints) {
			if (s.first == _panel->_width - 1) s.first = _width - 1;
			if (s.second == _panel->_height - 1) s.second = _height - 1;
		}
		for (Endpoint &e : _panel->_endpoints) {
			if (e.GetX() == _panel->_width - 1) e.SetX(_width - 1);
			if (e.GetY() == _panel->_height - 1) e.SetY(_height - 1);
		}
		_panel->_width = _width;
		_panel->_height = _height;
		_panel->_grid.resize(_width);
		for (auto& row : _panel->_grid) row.resize(_height);
	}
	readPanel(_panel);
	for (std::pair<int, int> s : symbols) {
		if (s.first == Decoration::Start)
			while (!place_start(s.second));
		if (s.first == Decoration::Exit)
			while (!place_exit(s.second));
	}
	generate_path();
	for (std::pair<int, int> s : symbols) {
		if ((s.first & Decoration::Stone) && !place_stones(s.first & 0xf, s.second)) {
			generate(id, symbols); return;
		}
	}
	for (std::pair<int, int> s : symbols) {
		if ((s.first & Decoration::Star) && !place_stars(s.first & 0xf, s.second)) {
			generate(id, symbols); return;
		}
	}
	for (std::pair<int, int> s : symbols) {
		if ((s.first & Decoration::Dot) && !place_dots(s.second, 0, s.first == Decoration::Dot_Intersection)) {
			generate(id, symbols); return;
		}
	}
	for (std::pair<int, int> s : symbols) {
		if ((s.first & Decoration::Gap) && !place_gaps(s.second)) {
			generate(id, symbols); return;
		}
	}
	for (int x = 0; x < _panel->_width; x++) {
		for (int y = 0; y < _panel->_height; y++) {
			if (_panel->_grid[x][y] == PATH)
				_panel->_grid[x][y] = 0;
		}
	}
	for (Point p : _starts) {
		_panel->_grid[p.first][p.second] |= STARTPOINT;
	}
	_panel->Write(id);
}

void Generate::generate_path()
{
	//generate_path((_panel->_width / 2 + 1) * (_panel->_height / 2 + 1) * 2 / 3);
	generate_path_regions((_panel->_width / 2 + _panel->_height / 2) / 2);
}

void Generate::generate_path(int minLength)
{
	clear();
	int fails = 0;
	int length = 1;
	Point pos = _starts[rand() % _starts.size()];
	Point exit = _exits[rand() % _exits.size()];
	_panel->_grid[pos.first][pos.second] = PATH;
	while (pos != exit) {
		Point dir = DIRECTIONS[rand() % 4];
		Point newPos = pos + dir;
		if (off_edge(newPos)) continue;
		if (get(newPos) == 0 && !(newPos == exit && length + 1 < minLength)) {
			set(newPos, PATH);
			set(pos + Point(dir.first / 2, dir.second / 2), PATH);
			pos = newPos;
			fails = 0;
			length++;
		}
		else {
			if (fails++ > 10) {
				generate_path(minLength);
				return;
			}
		}
	}
}

void Generate::generate_path_regions(int minRegions)
{
	clear();
	int fails = 0;
	int regions = 1;
	Point pos = _starts[rand() % _starts.size()];
	Point exit = _exits[rand() % _exits.size()];
	_panel->_grid[pos.first][pos.second] = PATH;
	while (pos != exit) {
		Point dir = DIRECTIONS[rand() % 4];
		Point newPos = pos + dir;
		if (off_edge(newPos)) continue;
		if (get(newPos) == 0 && !(newPos == exit && regions < minRegions)) {
			set(newPos, PATH);
			set(pos + Point(dir.first / 2, dir.second / 2), PATH);
			if (!on_edge(newPos) && on_edge(pos))
				regions++;
			pos = newPos;
			fails = 0;
		}
		else {
			if (fails++ > 10) {
				generate_path_regions(minRegions);
				return;
			}
		}
	}
}

std::set<Point> Generate::get_region(Point pos) {
	std::set<Point> region;
	std::vector<Point> check;
	check.push_back(pos);
	region.insert(pos);
	while (check.size() > 0) {
		Point p = check[check.size() - 1];
		check.pop_back();
		for (Point dir : DIRECTIONS) {
			Point p1 = p + Point(dir.first / 2, dir.second / 2);
			if (on_edge(p1)) continue;
			int flag = _panel->_grid[p1.first][p1.second];
			if (flag == PATH || flag == OPEN) continue;
			Point p2 = p + dir;
			if (region.insert(p2).second) {
				check.push_back(p2);
			}
		}
	}
	return region;
}

std::vector<int> Generate::get_symbols_in_region(Point pos) {
	return get_symbols_in_region(get_region(pos));
}

std::vector<int> Generate::get_symbols_in_region(std::set<Point> region) {
	std::vector<int> symbols;
	for (Point p : region) {
		int flag = _panel->_grid[p.first][p.second];
		if (flag) symbols.push_back(flag);
	}
	return symbols;
}

bool Generate::place_start(int amount)
{
	_starts.clear();
	_panel->_startpoints.clear();
	std::set<Point> open;
	for (int y = 0; y < _panel->_height; y += 2) {
		for (int x = 0; x < _panel->_width; x += 2) {
			if (std::find(_starts.begin(), _starts.end(), Point(x, y)) == _starts.end()) {
				open.insert(Point(x, y));
			}
		}
	}
	for (amount; amount > 0; amount--) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		_starts.push_back(pos);
		_panel->SetGridSymbol(pos.first, pos.second, Decoration::Start, Decoration::Color::None);
		open.erase(pos);
		for (Point dir : _8DIRECTIONS) {
			open.erase(pos + dir);
		}
	}
	return true;
}

bool Generate::place_exit(int amount)
{
	_exits.clear();
	_panel->_endpoints.clear();
	std::set<Point> open;
	for (int y = 0; y < _panel->_height; y += 2) {
		for (int x = 0; x < _panel->_width; x += 2) {
			if (on_edge(Point(x, y)) && std::find(_exits.begin(), _exits.end(), Point(x, y)) == _exits.end() && std::find(_starts.begin(), _starts.end(), Point(x, y)) == _starts.end()) {
				open.insert(Point(x, y));
			}
		}
	}
	for (amount; amount > 0; amount--) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		_exits.push_back(pos);
		_panel->SetGridSymbol(pos.first, pos.second, Decoration::Exit, Decoration::Color::None);
		open.erase(pos);
		for (Point dir : _8DIRECTIONS) {
			open.erase(pos + dir);
		}
	}
	return true;
}

bool Generate::can_place_gap(Point pos) {
	std::vector<Point> checkPoints = (pos.first % 2 == 0 ? std::vector<Point>({ Point(pos.first, pos.second - 1), Point(pos.first, pos.second + 1) })
		: std::vector<Point>({ Point(pos.first - 1, pos.second), Point(pos.first + 1, pos.second) }));
	for (Point check : checkPoints) {
		int valid = 4;
		for (Point dir : DIRECTIONS) {
			Point p = Point(check.first + dir.first / 2, check.second + dir.second / 2);
			if (off_edge(p) || _panel->_grid[p.first][p.second] & GAP || _panel->_grid[p.first][p.second] == OPEN) {
				if (--valid <= 2) {
					return false;
				}
			}
		}
	}
	return true;
}

bool Generate::place_gaps(int amount) {
	std::set<Point> open;
	for (int y = 0; y < _panel->_height; y++) {
		for (int x = y % 2 + 1; x < _panel->_width; x += 2) {
			if (_panel->_grid[x][y] == 0 && !on_edge(Point(x, y))) {
				open.insert(Point(x, y));
			}
		}
	}

	while (amount > 0) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		if (can_place_gap(pos)) {
			_panel->_grid[pos.first][pos.second] = (_fullGaps ? OPEN : pos.first % 2 == 0 ? Decoration::Gap_Column : Decoration::Gap_Row);
			amount--;
		}
		open.erase(pos);
	}
	return true;
}

bool Generate::can_place_dot(Point pos) {
	for (Point dir : _8DIRECTIONS) {
		Point p = Point(pos.first + dir.first / 2, pos.second + dir.second / 2);
		if (!off_edge(p) && _panel->_grid[p.first][p.second] & DOT) {
			return false;
		}
	}
	if (rand() % 10 > 0) {
		for (Point dir : DIRECTIONS) {
			Point p = pos + dir;
			if (!off_edge(p) && _panel->_grid[p.first][p.second] & DOT) {
				return false;
			}
		}
	}
	return true;
}

bool Generate::place_dots(int amount, int numColored, bool intersectionOnly) {
	std::set<Point> open;
	for (int x = 0; x < _panel->_width; x += (intersectionOnly ? 2 : 1)) {
		for (int y = 0; y < _panel->_height; y += (intersectionOnly ? 2 : 1)) {
			if (_panel->_grid[x][y] == PATH)
				open.insert(Point(x, y));
		}
	}

	for (Point p : _starts) open.erase(p);
	for (Point p : _exits) open.erase(p);

	while (amount > 0) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		if (can_place_dot(pos)) {
			_panel->_grid[pos.first][pos.second] = (pos.first % 2 == 1 ? Decoration::Dot_Row :
				pos.second % 2 == 1 ? Decoration::Dot_Column : Decoration::Dot_Intersection);
			amount--;
		}
		open.erase(pos);
	}
	return true;
}

bool Generate::place_stones(int color, int amount) {
	std::set<Point> open = _gridpos;
	std::set<Point> open2;
	while (amount > 0) {
		if (open.size() == 0) {
			if (open2.size() < amount)
				return false;
			Point pos = pick_random(open2);
			_panel->_grid[pos.first][pos.second] = Decoration::Stone | color;
			_gridpos.erase(pos);
			open2.erase(pos);
			amount--;
			continue;
		}
		Point pos = pick_random(open);
		std::set<Point> region = get_region(pos);
		std::vector<int> symbols = get_symbols_in_region(region);
		bool pass = true;
		for (int s : symbols) {
			if ((s & Decoration::Stone) && ((s & 0xf) != color)) {
				pass = false;
				break;
			}
		}
		for (Point p : region) {
			open.erase(p);
			if (pass && p != pos) open2.insert(p);
		}
		if (!pass) continue;
		_panel->_grid[pos.first][pos.second] = Decoration::Stone | color;
		_gridpos.erase(pos);
		if (_panel->_style == 0)
			for (Point p : region) {
				for (Point dir : _8DIRECTIONS) {
					Point pos2 = p + dir;
					if (open.count(pos2) && !region.count(pos2)) {
						for (Point P : get_region(pos2)) {
							open.erase(P);
						}
					}
				}
			}
		amount--;
	}
	_panel->_style |= Panel::Style::HAS_STONES;
	return true;
}

bool Generate::place_stars(int color, int amount)
{
	std::set<Point> open = _gridpos;
	std::set<Point> open2;
	while (amount > 0) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		std::set<Point> region = get_region(pos);
		std::vector<int> symbols = get_symbols_in_region(region);
		open2.clear();
		int count = 0;
		for (int s : symbols) {
			if ((s & 0xf) == color) {
				count++;
			}
		}
		for (Point p : region) {
			if (open.erase(p)) open2.insert(p);
		}
		if (count >= 2) continue;
		if (open2.size() + count < 2) continue;
		if (count == 0 && amount == 1) continue;
		_panel->_grid[pos.first][pos.second] = Decoration::Star | color;
		_gridpos.erase(pos);
		amount--;
		if (count == 0) {
			open2.erase(pos);
			if (open2.size() == 0)
				return false;
			pos = pick_random(open2);
			_panel->_grid[pos.first][pos.second] = Decoration::Star | color;
			_gridpos.erase(pos);
			amount--;
		}
	}
	_panel->_style |= Panel::Style::HAS_STARS;
	return true;
}
