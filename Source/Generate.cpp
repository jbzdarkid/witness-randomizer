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

void Generate::generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5) {
	generate(id, { std::make_pair(symbol1, amount1), std::make_pair(symbol2, amount2), std::make_pair(symbol3, amount3), std::make_pair(symbol4, amount4),  std::make_pair(symbol5, amount5) });
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
	_openpos = _gridpos;
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

	//For debugging only
	std::vector<std::string> solution;
	for (int y = 0; y < _panel->_height; y++) {
		std::string row;
		for (int x = 0; x < _panel->_width; x++) {
			if (_panel->_grid[x][y] == PATH) {
				row += "xx";
			}
			else row += "    ";
		}
		solution.push_back(row);
	}

	std::vector<std::pair<int, int>> backupSymbols = symbols; //Have to do this because eraser modifies symbols list
	
	int toErase = 0;
	for (std::pair<int, int> s : symbols) {
		if ((s.first & 0x700) == Decoration::Eraser) {
			while (toErase == 0) {
				int eraseIndex = rand() % symbols.size();
				auto symbol = symbols[eraseIndex];
				if (symbol.first != Decoration::Start && symbol.first != Decoration::Exit && !(symbol.first & Decoration::Gap) && (symbol.first & 0x700) != Decoration::Eraser) {
					toErase = symbol.first;
					symbols[eraseIndex].second--;
				}
			}
		}
	}

	int numShapes = 0, numRotate = 0, numNegative = 0;
	std::vector<int> colors, negativeColors;
	for (std::pair<int, int> s : symbols) {
		if ((s.first & 0x700) == Decoration::Poly) {
			for (int i = 0; i < s.second; i++) {
				if (s.first & Decoration::Can_Rotate) numRotate++;
				if (s.first & Decoration::Negative) {
					numNegative++;
					negativeColors.push_back(s.first & 0xf);
				}
				else {
					numShapes++;
					colors.push_back(s.first & 0xf);
				}
			}
		}
	}
	if (numShapes > 0 && !place_shapes(colors, negativeColors, numShapes, numRotate, numNegative, false)) {
		generate(id, backupSymbols); return;
	}

	_bisect = true;
	for (std::pair<int, int> s : symbols) {
		if ((s.first & 0x700) == Decoration::Stone && !place_stones(s.first & 0xf, s.second)) {
			generate(id, backupSymbols); return;
		}
	}
	for (std::pair<int, int> s : symbols) {
		if ((s.first & 0x700) == Decoration::Triangle && !place_triangles(s.first & 0xf, s.second)) {
			generate(id, backupSymbols); return;
		}
	}
	for (std::pair<int, int> s : symbols) {
		if ((s.first & 0x700) == Decoration::Star && !place_stars(s.first & 0xf, s.second)) {
			generate(id, backupSymbols); return;
		}
	}
	for (std::pair<int, int> s : symbols) {
		if ((s.first & 0x700) == Decoration::Eraser && !place_eraser(s.first & 0xf, toErase)) {
			generate(id, backupSymbols); return;
		}
	}
	for (std::pair<int, int> s : symbols) {
		if ((s.first & Decoration::Dot) && !place_dots(s.second, 0, s.first == Decoration::Dot_Intersection)) {
			generate(id, backupSymbols); return;
		}
	}
	for (std::pair<int, int> s : symbols) {
		if ((s.first & ~0xf) == Decoration::Gap && !place_gaps(s.second)) {
			generate(id, symbols); return;
		}
	}
	for (Point p : _starts) {
		_panel->_grid[p.first][p.second] |= STARTPOINT;
	}
	for (int y = 0; y < _panel->_height; y++) {
		for (int x = 0; x < _panel->_width; x++) {
			if (_panel->_grid[x][y] == PATH) {
				_panel->_grid[x][y] = 0;
			}
		}
	}
	_panel->Write(id);
}

void Generate::generate_path()
{
	//generate_path((_panel->_width / 2 + 1) * (_panel->_height / 2 + 1) * 2 / 3);
	generate_path_regions((_panel->_width / 2 + _panel->_height / 2) / 2 + 1);
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
			if (dir.first == 0 || dir.second == 0 || rand() % 2 > 0) //Add some variation
				return false;
		}
	}
	if (rand() % 10 > 0) {
		for (Point dir : DIRECTIONS) {
			Point p = pos + dir;
			if (!off_edge(p) && _panel->_grid[p.first][p.second] & DOT) {
				if (rand() % 2 > 0) //Add some variation
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
	std::set<Point> open = _openpos;
	std::set<Point> open2;
	while (amount > 0) {
		if (open.size() == 0) {
			if (open2.size() < amount)
				return false;
			Point pos = pick_random(open2);
			_panel->_grid[pos.first][pos.second] = Decoration::Stone | color;
			_openpos.erase(pos);
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
		_openpos.erase(pos);
		if (_bisect)
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
	_bisect = false;
	return true;
}

Shape Generate::generate_shape(std::set<Point>& region, std::set<Point>& bufferRegion, Point pos, int maxSize, bool disconnect)
{
	Shape shape;
	shape.insert(pos);
	if (!bufferRegion.erase(pos))
		region.erase(pos);
	while (shape.size() < maxSize && region.size() > 0) {
		pos = pick_random(shape);
		int i = 0;
		for (; i < 10; i++) {
			Point dir = pick_random(DIRECTIONS);
			Point p = pos + dir;
			if (region.count(p) && !shape.count(p)) {
				shape.insert(p);
				if (!bufferRegion.erase(p))
					region.erase(p);
				break;
			}
		}
		if (i == 10)
			return shape;
	}
	return shape;
}

int Generate::make_shape_symbol(Shape shape, bool rotated, bool negative, int rotation)
{
	int symbol = static_cast<int>(Decoration::Poly);
	if (rotated) {
		if (rotation == -1) {
			if (make_shape_symbol(shape, rotated, negative, 0) == make_shape_symbol(shape, rotated, negative, 1))
				return 0; //Check to make sure the shape is not the same when rotated
			rotation = rand() % 4;
		}
		symbol |= Decoration::Can_Rotate;
		Shape newShape;
		for (Point p : shape) {
			switch (rotation) {
			case 0: newShape.insert(p); break;
			case 1: newShape.insert(Point(p.second, -p.first)); break;
			case 2: newShape.insert(Point(-p.second, p.first)); break;
			case 3: newShape.insert(Point(-p.first, -p.second)); break;
			}
		}
		shape = newShape;
	}
	if (negative) symbol |= Decoration::Negative;
	int xmin = INT_MAX, xmax = INT_MIN, ymin = INT_MAX, ymax = INT_MIN;
	for (Point p : shape) {
		if (p.first < xmin) xmin = p.first;
		if (p.first > xmax) xmax = p.first;
		if (p.second < ymin) ymin = p.second;
		if (p.second > ymax) ymax = p.second;
	}
	if (xmax - xmin > 8 || ymax - ymin > 8)
		return 0; //Shapes cannot be more than 4 in width and height
	for (Point p : shape) {
		symbol |= (1 << ((p.first - xmin) / 2 + (ymax  - p.second) * 2)) << 16;
	}
	return symbol;
}

bool Generate::place_shapes(std::vector<int> colors, std::vector<int> negativeColors, int amount, int numRotated, int numNegative, bool disconnect)
{
	std::set<Point> open = _openpos;
	int targetArea = amount * 3, totalArea = 0;
	int colorIndex = rand() % colors.size();
	int colorIndexN = rand() % (negativeColors.size() + 1);
	while (amount > 0) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		std::set<Point> region = get_region(pos);
		std::set<Point> bufferRegion;
		std::set<Point> open2;
		for (Point p : region) {
			if (open.erase(p)) open2.insert(p);
		}
		std::vector<Shape> shapes;
		std::vector<Shape> shapesN;
		int numShapesN = min(rand() % (numNegative + 1), static_cast<int>(region.size()) / 3);
		if (amount == 1) numShapesN = numNegative;
		if (numShapesN) {
			std::set<Point> regionN = _gridpos;
			int maxSize = static_cast<int>(region.size()) - numShapesN * 3;
			if (maxSize == 0) maxSize = 1;
			for (int i = 0; i < numShapesN; i++) {
				pos = pick_random(region);
				for (int i = 0; i < 10; i++) {
					Point p = pos + pick_random(DIRECTIONS);
					if (regionN.count(p) && !region.count(p)) {
						pos = p;
						break;
					}
				}
				if (!regionN.count(pos)) return false;
				Shape shape = generate_shape(regionN, pos, min(rand() % 3 + 1, maxSize), false);
				shapesN.push_back(shape);
				for (Point p : shape) {
					if (region.count(p)) bufferRegion.insert(p);
					else region.insert(p);
				}
			}
		}
		int numShapes = static_cast<int>(region.size() + bufferRegion.size()) / 5 + 1;
		if (numShapes == 1 && bufferRegion.size() > 0) numShapes++;
		if (numShapes < amount && region.size() >= 5 && (rand() & 1) == 1)
			numShapes++; //Adds more variation to the shape sizes
		bool balance = false;
		if (numShapes > amount) {
			if (numNegative < 2) continue;
			//Make balancing shapes - Positive and negative will be switched so that code can be reused
			balance = true;
			std::set<Point> regionN = _gridpos;
			numShapes = max(2, rand() % numNegative + 1);			//Actually the negative shapes
			numShapesN = min(amount, numShapes * 2 / 5 + 1);		//Actually the positive shapes
			if (numShapesN >= numShapes * 3 || numShapesN * 5 <= numShapes) continue;
			shapes.clear();
			shapesN.clear();
			region.clear();
			bufferRegion.clear();
			for (int i = 0; i < numShapesN; i++) {
				Shape shape = generate_shape(regionN, pick_random(regionN), min(5, numShapes * 2 / numShapesN + rand() % 3 - 1), false);
				shapesN.push_back(shape);
				for (Point p : shape) {
					region.insert(p);
				}
			}
			//Let the rest of the algorithm create the cancelling shapes
		}
		
		if (open2.size() < numShapes + numShapesN) continue;
		if (numShapes == 1) {
			shapes.push_back(region);
			region.clear();
		}
		else for (; numShapes > 0; numShapes--) {
			if (region.size() == 0) break;
			shapes.push_back(generate_shape(region, bufferRegion, pick_random(region), balance ? rand() % 3 + 1 : 4, false));
		}
		//Take remaining area and try to stick it to existing shapes
		multibreak:
		while (region.size() > 0) {
			pos = pick_random(region);
			for (Shape& shape : shapes) {
				if (shape.size() >= 5 || shape.count(pos) > 0) continue;
				for (Point p : shape) {
					for (Point dir : DIRECTIONS) {
						if (pos + dir == p) {
							shape.insert(pos);
							if (!bufferRegion.erase(pos))
								region.erase(pos);
							goto multibreak;
						}
					}
				}
			}
			//Failed to cover entire region, need to pick a different region
			break;
		}
		if (region.size() > 0) continue;
		if (balance) { //Undo swap for balancing shapes
			std::vector<Shape> swap = shapes;
			shapes = shapesN;
			shapesN = swap;
		}
		numNegative -= static_cast<int>(shapesN.size());
		numShapes = static_cast<int>(shapes.size());
		for (Shape& shape : shapesN) {
			shapes.push_back(shape);
		}
		for (Shape& shape : shapes) {
			int symbol = make_shape_symbol(shape, (numRotated-- > 0), (numShapes-- <= 0));
			if (symbol == 0)
				return false;
			//Attempt not to put shapes adjacent
			Point pos;
			for (int i = 0; i < 10; i++) {
				pos = pick_random(open2);
				bool pass = true;
				for (Point dir : _8DIRECTIONS) {
					Point p = pos + dir;
					if (!off_edge(p) && _panel->_grid[p.first][p.second] & Decoration::Poly) {
						pass = false;
						break;
					}
				}
				if (pass) break;
			}
			if (symbol & Decoration::Negative) _panel->_grid[pos.first][pos.second] = symbol | negativeColors[(colorIndexN++) % negativeColors.size()];
			else {
				_panel->_grid[pos.first][pos.second] = symbol | colors[(colorIndex++) % colors.size()];
				totalArea += static_cast<int>(shape.size());
				amount--;
			}
			open2.erase(pos);
			_openpos.erase(pos);
		}
	}
	if (totalArea < targetArea || numNegative > 0)
		return false;
	return true;
}

bool Generate::place_stars(int color, int amount)
{
	std::set<Point> open = _openpos;
	while (amount > 0) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		std::set<Point> region = get_region(pos);
		std::vector<int> symbols = get_symbols_in_region(region);
		int count = 0;
		for (int s : symbols) {
			if ((s & 0xf) == color) {
				count++;
			}
		}
		std::set<Point> open2;
		for (Point p : region) {
			if (open.erase(p)) open2.insert(p);
		}
		if (count >= 2) continue;
		if (open2.size() + count < 2) continue;
		if (count == 0 && amount == 1) continue;
		_panel->_grid[pos.first][pos.second] = Decoration::Star | color;
		_openpos.erase(pos);
		amount--;
		if (count == 0) {
			open2.erase(pos);
			if (open2.size() == 0)
				return false;
			pos = pick_random(open2);
			_panel->_grid[pos.first][pos.second] = Decoration::Star | color;
			_openpos.erase(pos);
			amount--;
		}
	}
	return true;
}

bool Generate::place_triangles(int color, int amount)
{
	std::set<Point> open = _openpos;
	while (amount > 0) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		int count = 0;
		for (Point dir : DIRECTIONS) {
			Point p = Point(pos.first + dir.first / 2, pos.second + dir.second / 2);
			if (!off_edge(p) && _panel->_grid[p.first][p.second] == PATH) {
				count++;
			}
		}
		open.erase(pos);
		if (count == 0)
			continue;
		_panel->_grid[pos.first][pos.second] = Decoration::Triangle | color | (count << 16);
		_openpos.erase(pos);
		amount--;
	}
	return true;
}

bool Generate::place_eraser(int color, int toErase)
{
	std::set<Point> open = _openpos;
	while (open.size() > 0) {
		Point pos = pick_random(open);
		std::set<Point> region = get_region(pos);
		std::set<Point> open2;
		for (Point p : region) {
			if (open.erase(p)) open2.insert(p);
		}
		if (open2.size() < 2 && !(toErase & Decoration::Dot) || open2.size() < 1) continue;
		bool canPlace = false;
		if ((toErase & 0x700) == Decoration::Stone) {
			std::vector<int> symbols = get_symbols_in_region(region);
			for (int s : symbols) {
				if ((s & 0x700) == Decoration::Stone) {
					canPlace = (s & 0xf) != (toErase & 0xf);
					break;
				}
			}
		}
		else if ((toErase & 0x700) == Decoration::Star) {
			std::vector<int> symbols = get_symbols_in_region(region);
			int count = (color == (toErase & 0xf));
			for (int s : symbols) {
				if ((s & 0xf) == (toErase & 0xf)) {
					if (++count >= 2) {
						canPlace = true;
						break;
					}
				}
			}
			if (count == 0) canPlace = true;
		}
		else canPlace = true;
		if (!canPlace) continue;
		_panel->_grid[pos.first][pos.second] = Decoration::Eraser | color;
		_openpos.erase(pos);
		open2.erase(pos);
		if (!(toErase & Decoration::Dot)) {
			pos = pick_random(open2);
			_openpos.erase(pos);
		}
		if ((toErase & 0x700) == Decoration::Stone || (toErase & 0x700) == Decoration::Star) {
			_panel->_grid[pos.first][pos.second] = toErase;
		}
		else if (toErase & Decoration::Dot) {
			std::set<Point> openEdge;
			for (Point p : region) {
				for (Point dir : _8DIRECTIONS) {
					if (toErase == Decoration::Dot_Intersection && (dir.first == 0 || dir.second == 0)) continue;
					Point p2 = Point(pos.first + dir.first / 2, pos.second + dir.second / 2);
					if (_panel->_grid[p2.first][p2.second] == 0 && can_place_dot(p2)) {
						openEdge.insert(p2);
					}
				}
			}
			if (openEdge.size() == 0) return false;
			pos = pick_random(openEdge);
			toErase &= ~IntersectionFlags::INTERSECTION;
			if ((pos.first & 1) == 0) toErase |= IntersectionFlags::COLUMN;
			if ((pos.second & 1) == 0) toErase |= IntersectionFlags::ROW;
			_panel->_grid[pos.first][pos.second] = toErase;
		}
		else if ((toErase & 0x700) == Decoration::Poly) {
			int symbol = 0;
			while (symbol == 0) {
				std::set<Point> area = _gridpos;
				Shape shape = generate_shape(area, pick_random(area), rand() % 5 + 1, false);
				if (shape.size() == region.size()) continue;
				symbol = make_shape_symbol(shape, toErase & Decoration::Can_Rotate, toErase & Decoration::Negative);
			}
			_panel->_grid[pos.first][pos.second] = symbol | (toErase & 0xf);
		}
		else if ((toErase & 0x700) == Decoration::Triangle) {
			int count = 0;
			for (Point dir : DIRECTIONS) {
				Point p = Point(pos.first + dir.first / 2, pos.second + dir.second / 2);
				if (!off_edge(p) && _panel->_grid[p.first][p.second] == PATH) {
					count++;
				}
			}
			count = (count + (rand() & 1)) % 3 + 1;
			_panel->_grid[pos.first][pos.second] = toErase | (count << 16);
		}
		return true;
	}
	return false;
}
