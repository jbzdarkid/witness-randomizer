#include "Generate.h"
#include "Randomizer.h"

void Generate::generate(int id, int symbol, int amount) {
	PuzzleSymbols symbols({ std::make_pair(symbol, amount) });
	while (!generate(id, symbols));
}

void Generate::generate(int id, int symbol1, int amount1, int symbol2, int amount2) {
	PuzzleSymbols symbols({ std::make_pair(symbol1, amount1), std::make_pair(symbol2, amount2) });
	while (!generate(id, symbols));
}

void Generate::generate(int id, int symbol1, int amount1,  int symbol2, int amount2, int symbol3, int amount3) {
	PuzzleSymbols symbols({ std::make_pair(symbol1, amount1), std::make_pair(symbol2, amount2), std::make_pair(symbol3, amount3) });
	while (!generate(id, symbols));
}

void Generate::generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4) {
	PuzzleSymbols symbols({ std::make_pair(symbol1, amount1), std::make_pair(symbol2, amount2), std::make_pair(symbol3, amount3), std::make_pair(symbol4, amount4) });
	while (!generate(id, symbols));
}

void Generate::generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5) {
	PuzzleSymbols symbols({ std::make_pair(symbol1, amount1), std::make_pair(symbol2, amount2), std::make_pair(symbol3, amount3), std::make_pair(symbol4, amount4),  std::make_pair(symbol5, amount5) });
	while (!generate(id, symbols));
}

Point operator+(const Point& l, const Point& r) { return { l.first + r.first, l.second + r.second }; }

std::vector<Point> Generate::_DIRECTIONS1 = { Point(0, 1), Point(0, -1), Point(1, 0), Point(-1, 0) };
std::vector<Point> Generate::_8DIRECTIONS1 = { Point(0, 1), Point(0, -1), Point(1, 0), Point(-1, 0), Point(1, 1), Point(1, -1), Point(-1, -1), Point(-1, 1) };
std::vector<Point> Generate::_DIRECTIONS2 = { Point(0, 2), Point(0, -2), Point(2, 0), Point(-2, 0) };
std::vector<Point> Generate::_8DIRECTIONS2 = { Point(0, 2), Point(0, -2), Point(2, 0), Point(-2, 0), Point(2, 2), Point(2, -2), Point(-2, -2), Point(-2, 2) };
std::vector<Point> Generate::_DISCONNECT = { Point(0, 2), Point(0, -2), Point(2, 0), Point(-2, 0), Point(2, 2), Point(2, -2), Point(-2, -2), Point(-2, 2), 
	Point(0, 2), Point(0, -2), Point(2, 0), Point(-2, 0), Point(2, 2), Point(2, -2), Point(-2, -2), Point(-2, 2),
	Point(0, 4), Point(0, -4), Point(4, 0), Point(-4, 0),
};
std::vector<Point> Generate::_SHAPEDIRECTIONS = { };

void Generate::generateMaze(int id) {
	while (!generate_maze(id, 0, 0));
}

void Generate::generateMaze(int id, int numStarts, int numExits)
{
	while (!generate_maze(id, numStarts, numExits));
}

void Generate::initPanel(int id) {
	if (!_panel) {
		_panel = std::make_shared<Panel>(id);
	}
	if (_width > 0 && _height > 0 && (_width != _panel->_width || _height != _panel->_height)) {
		_panel->Resize(_width, _height);
	}
	if (hasFlag(Config::FixBackground)) {
		_panel->Resize(_panel->_width, _panel->_height); //This will force the panel to have to redraw the background
	}
	if (_custom_grid.size() > 0) {
		if (_custom_grid.size() < _panel->_width) {
			_custom_grid.resize(_panel->_width, std::vector<int>());
		}
		if (_custom_grid[_custom_grid.size() - 1].size() < _panel->_height) {
			for (auto& row : _custom_grid) {
				row.resize(_panel->_height, 0);
			}
		}
		_panel->_grid = _custom_grid;
	}
	if (_starts.size() == 0)
		_starts = std::set<Point>(_panel->_startpoints.begin(), _panel->_startpoints.end());
	else
		_panel->_startpoints = std::vector<Point>(_starts.begin(), _starts.end());
	if (_exits.size() == 0) {
		for (Endpoint e : _panel->_endpoints) {
			_exits.insert(Point(e.GetX(), e.GetY()));
		}
	}
	else {
		_panel->_endpoints.clear();
		for (Point e : _exits) {
			_panel->SetGridSymbol(e.first, e.second, Decoration::Exit, Decoration::Color::None);
		}
	}
	if (hasFlag(Config::TreehouseLayout)) {
		init_treehouse_layout();
	}
	_gridpos.clear();
	for (int x = 1; x < _panel->_width; x += 2) {
		for (int y = 1; y < _panel->_height; y += 2) {
			if (!(hasFlag(Config::PreserveStructure) && (get(x, y) & Decoration::Empty) == Decoration::Empty))
				_gridpos.insert(Point(x, y));
		}
	}
	_openpos = _gridpos;
	_fullGaps = hasFlag(Config::FullGaps);
	if (_symmetry) _panel->symmetry = _symmetry;
	if (pathWidth != 1) _panel->pathWidth = pathWidth;
}

void Generate::setSymbol(Decoration::Shape symbol, int x, int y)
{
	if (_custom_grid.size() < x + 1) {
		_custom_grid.resize(x + 1, std::vector<int>());
	}
	if (_custom_grid[x].size() < y + 1) {
		for (auto& row : _custom_grid) {
			row.resize(y + 1, 0);
		}
	}

	if (symbol == Decoration::Start) _starts.insert(Point(x, y));
	if (symbol == Decoration::Exit) _exits.insert(Point(x, y));
	else _custom_grid[x][y] = symbol; //Only exits are not set into the grid
}

void Generate::setGridSize(int width, int height) {
	if (width <= 0 || height <= 0) {
		_width = 0; _height = 0;
	}
	else {
		_width = width * 2 + 1; _height = height * 2 + 1;
	}
}

void Generate::setSymmetry(Panel::Symmetry symmetry)
{
	_symmetry = symmetry;
}

void Generate::write(int id)
{
	std::vector<std::vector<int>> backupGrid;
	if (hasFlag(Config::DisableReset)) backupGrid = _panel->_grid;

	erase_path();

	_areaTotal++;
	_genTotal++;
	if (_handle) {
		int total = (_totalPuzzles == 0 ? _areaPuzzles : _totalPuzzles);
		std::wstring text = _areaName + L": " + std::to_wstring(_areaTotal) + L"/" + std::to_wstring(_areaPuzzles) + L" (" + std::to_wstring(_genTotal * 100 / total) + L"%)";
		SetWindowText(_handle, text.c_str());
	}

	if (hasFlag(Config::ResetColors)) {
		_panel->_memory->WritePanelData<int>(id, PUSH_SYMBOL_COLORS, { 0 });
		_panel->_memory->WritePanelData<int>(id, DECORATION_COLORS, { 0 });
	}
	else if (hasFlag(Config::AlternateColors)) {
		_panel->_memory->WritePanelData<int>(id, PUSH_SYMBOL_COLORS, { 1 });
		_panel->_memory->WritePanelData<int>(id, DECORATION_COLORS, { 0 });
	}
	_panel->writeColors = hasFlag(Config::WriteColors);
	_panel->Write(id);
	
	if (hasFlag(Config::DisableReset)) _panel->_grid = backupGrid;
	else reset();

	if (_oneTimeAdd) {
		_config &= ~_oneTimeAdd;
		_oneTimeAdd = 0;
	}
	if (_oneTimeRemove) {
		_config |= _oneTimeRemove;
		_oneTimeRemove = 0;
	}
	srand(_seed);
	_seed = rand(); //Manually advance seed by 1 each generation to prevent seeds "funneling" from repeated fails
}

void Generate::resetConfig()
{
	setGridSize(0, 0);
	_symmetry = Panel::Symmetry::None;
	pathWidth = 1;
	if (hasFlag(Config::DisableReset)) {
		reset();
	}
	_config = 0;
	_oneTimeAdd = Config::None;
	_oneTimeRemove = Config::None;
}

//----------------------Private--------------------------

void Generate::set_path(Point pos)
{
	_panel->_grid[pos.first][pos.second] = PATH;
	_path.insert(pos);
	if (_panel->symmetry) {
		_path1.insert(pos);
		Point sp = get_sym_point(pos);
		_panel->_grid[sp.first][sp.second] = PATH;
		_path.insert(sp);
		_path2.insert(sp);
	}
}

void Generate::clear()
{
	for (int x = 0; x < _panel->_width; x++) {
		for (int y = 0; y < _panel->_height; y++) {
			if (hasFlag(Config::PreserveStructure) && (_panel->_grid[x][y] == OPEN || (_panel->_grid[x][y] & 0x60000f) == NO_POINT || (_panel->_grid[x][y] & Decoration::Empty) == Decoration::Empty)) continue;
			_panel->_grid[x][y] = 0;
		}
	}
	_panel->_style &= ~0x2ff8; //Remove all element flags
	_path.clear(); _path1.clear(); _path2.clear();
}

void Generate::reset() {
	_panel = NULL; //This is needed for the generator to read in the next panel
	_starts.clear();
	_exits.clear();
	_custom_grid.clear();
}

void Generate::init_treehouse_layout()
{
	bool pivot = _panel->_endpoints.size() > 2;
	_panel->_startpoints.clear();
	_panel->SetGridSymbol(_panel->_width / 2, _panel->_height - 1, Decoration::Start, Decoration::Color::None);
	_panel->_endpoints.clear();
	_panel->SetGridSymbol(_panel->_width / 2, 0, Decoration::Exit, Decoration::Color::None);
	if (pivot) {
		_panel->SetGridSymbol(_panel->_width - 1, _panel->_height / 2, Decoration::Exit, Decoration::Color::None);
		_panel->SetGridSymbol(0, _panel->_height / 2, Decoration::Exit, Decoration::Color::None);
	}
	_starts.clear();
	_exits.clear();
	if ((_panel->_width / 2) % 2 == 1) {
		_starts.insert(Point(_panel->_width / 2 - 1, _panel->_height - 1));
		_starts.insert(Point(_panel->_width / 2 + 1, _panel->_height - 1));
		_exits.insert(Point(_panel->_width / 2 - 1, 0));
		_exits.insert(Point(_panel->_width / 2 + 1, 0));
	}
	else {
		_starts.insert(Point(_panel->_width / 2, _panel->_height - 1));
		_exits.insert(Point(_panel->_width / 2, 0));
	}
	if (pivot && pivotDirection != Endpoint::Direction::UP) {
		_exits.clear();
		if ((_panel->_height / 2) % 2 == 1) {
			if (pivotDirection == Endpoint::Direction::RIGHT) {
				_exits.insert(Point(_panel->_width - 1, _panel->_height / 2 - 1));
				_exits.insert(Point(_panel->_width - 1, _panel->_height / 2 + 1));
			}
			if (pivotDirection == Endpoint::Direction::LEFT) {
				_exits.insert(Point(0, _panel->_height / 2 - 1));
				_exits.insert(Point(0, _panel->_height / 2 + 1));
			}
		}
		else {
			if (pivotDirection == Endpoint::Direction::RIGHT)
				_exits.insert(Point(_panel->_width - 1, _panel->_height / 2));
			if (pivotDirection == Endpoint::Direction::LEFT)
				_exits.insert(Point(0, _panel->_height / 2));
		}
	}
}

bool Generate::generate_maze(int id, int numStarts, int numExits)
{
	initPanel(id);

	if (numStarts > 0) place_start(numStarts);
	if (numExits > 0) place_exit(numExits);

	for (Point p : _starts)
		if (_exits.count(p))
			return false;

	clear();
	while (!generate_path_length(_panel->_width + _panel->_height)) clear();

	std::set<Point> extraStarts;
	for (Point pos : _starts) {
		if (!_path.count(pos)) {
			extraStarts.insert(pos);
		}
		set_path(pos);
	}
	if (extraStarts.size() != (_panel->symmetry ? _starts.size() / 2 - 1 : _starts.size() - 1))
		return false;

	std::set<Point> check;
	std::vector<Point> deadEndH, deadEndV;
	for (Point p : _path) {
		if (p.first % 2 == 0 && p.second % 2 == 0)
			check.insert(p);
	}
	while (check.size() > 0) {
		Point randomPos = (extraStarts.size() > 0 ? pick_random(extraStarts) : pick_random(check));
		Point pos = randomPos;
		for (int i = (extraStarts.size() > 0 ? 7 : rand() % 5); i >= 0; i--) {
			std::vector<Point> validDir;
			for (Point dir : _DIRECTIONS2) {
				if (!off_edge(pos + dir) && get(pos + dir) == 0) {
					validDir.push_back(dir);
				}
			}
			if (validDir.size() < 2) check.erase(pos);
			if (validDir.size() == 0) {
				if (extraStarts.size() > 0) {
					return false;
				}
				if (_fullGaps && !_exits.count(pos) && !_starts.count(pos)) {
					int countOpenRow = 0, countOpenColumn = 0;
					for (Point dir2 : _DIRECTIONS1) {
						if (!off_edge(pos + dir2) && get(pos + dir2) == PATH) {
							if (dir2.first == 0) countOpenColumn++;
							else countOpenRow++;
						}
					}
					if (countOpenRow + countOpenColumn == 1) {
						if (countOpenRow) deadEndH.push_back(pos);
						else deadEndV.push_back(pos);
					}
				}
				break;
			}
			Point dir = pick_random(validDir);
			Point newPos = pos + dir;
			set_path(newPos);
			set_path(Point(pos.first + dir.first / 2, pos.second + dir.second / 2));
			check.insert(newPos);
			pos = newPos;
		}
		if (extraStarts.size() > 0) extraStarts.erase(randomPos);
	}

	for (int y = 0; y < _panel->_height; y++) {
		for (int x = (y + 1) % 2; x < _panel->_width; x += 2) {
			if (get(x, y) != PATH) {
				set(x, y, _fullGaps ? OPEN : x % 2 == 0 ? Decoration::Gap_Column : Decoration::Gap_Row);
				if (_panel->symmetry) {
					Point sp = get_sym_point(Point(x, y));
					if (sp.first == x && sp.second == y || sp.first == x && x % 2 == 0 && abs(sp.second - y) <= 2 ||
						sp.second == y && y % 2 == 0 && abs(sp.first - x) <= 2 || abs(sp.first - x) == 1) {
						set(x, y, PATH);
					}
					else if (rand() % 2 == 0) {
						set(sp, PATH);
					}
					else {
						set(x, y, PATH);
						set(sp, _fullGaps ? OPEN : x % 2 == 0 ? Decoration::Gap_Column : Decoration::Gap_Row);
					}
				}
			}
		}
	}

	for (Point p : deadEndH) {
		set(p, Decoration::Gap_Row);
	}
	for (Point p : deadEndV) {
		set(p, Decoration::Gap_Column);
	}

	if (!hasFlag(Config::DisableWrite)) write(id);
	return true;
}

bool Generate::generate(int id, PuzzleSymbols symbols)
{
	initPanel(id);

	if (symbols.getNum(Decoration::Dot) >= _panel->get_num_grid_points() - 2)
		_parity = (_panel->get_parity() + (
			!symbols.any(Decoration::Start) ? get_parity(pick_random(_starts)) :
			!symbols.any(Decoration::Exit) ? get_parity(pick_random(_exits)) : rand() % 2)) % 2;
	else _parity = -1;

	if (symbols.any(Decoration::Start)) place_start(symbols.getNum(Decoration::Start));
	if (symbols.any(Decoration::Exit)) place_exit(symbols.getNum(Decoration::Exit));

	for (Point p : _starts)
		if (_exits.count(p))
			return false;

	if (hasFlag(Config::SplitErasers)) {
		if ((_panel->_width / 2) % 2 == 0) {
			_splitPoints = { Point(1, _panel->_height / 2), Point(_panel->_width - 2, _panel->_height / 2) };
		}
		else {
			_splitPoints = { Point(_panel->_width / 2, 1), Point(_panel->_width / 2, _panel->_height - 2) };
		}
	}

	int fails = 0;
	while (!generate_path(symbols)) {
		if (fails++ > 20) return false;
	}

	std::vector<std::string> solution; //For debugging only
	for (int y = 0; y < _panel->_height; y++) {
		std::string row;
		for (int x = 0; x < _panel->_width; x++) {
			if (get(x, y) == PATH) {
				row += "xx";
			}
			else row += "    ";
		}
		solution.push_back(row);
	}

	std::set<Point> backupPath, backupPath1, backupPath2;
	if (hasFlag(Config::BackupPath)) {
		backupPath = _path; backupPath1 = _path1; backupPath2 = _path2;
	}

	if (!place_all_symbols(symbols))
		return false;

	if (hasFlag(Config::BackupPath)) {
		_path = backupPath; _path1 = backupPath1; _path2 = backupPath2;
	}

	if (!hasFlag(Config::DisableWrite)) write(id);
	return true;
}

bool Generate::place_all_symbols(PuzzleSymbols & symbols)
{
	std::vector<int> eraseSymbols;
	std::vector<int> eraserColors;
	for (std::pair<int, int> s : symbols[Decoration::Eraser]) {
		for (int i = 0; i < s.second; i++) {
			eraserColors.push_back(s.first & 0xf);
			eraseSymbols.push_back(symbols.popRandomSymbol());
		}
	}

	_SHAPEDIRECTIONS = (hasFlag(Config::DisconnectShapes) ? _DISCONNECT : _DIRECTIONS2);
	int numShapes = 0, numRotate = 0, numNegative = 0;
	std::vector<int> colors, negativeColors;
	for (std::pair<int, int> s : symbols[Decoration::Poly]) {
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
	if (numShapes > 0 && !place_shapes(colors, negativeColors, numShapes, numRotate, numNegative)) return false;
	_stoneTypes = static_cast<int>(symbols[Decoration::Stone].size());
	_bisect = true;
	for (std::pair<int, int> s : symbols[Decoration::Stone]) if (!place_stones(s.first & 0xf, s.second))
		return false;
	for (std::pair<int, int> s : symbols[Decoration::Triangle]) if (!place_triangles(s.first & 0xf, s.second))
		return false;
	for (std::pair<int, int> s : symbols[Decoration::Star]) if (!place_stars(s.first & 0xf, s.second))
		return false;
	for (std::pair<int, int> s : symbols[Decoration::Eraser]) if (!place_erasers(eraserColors, eraseSymbols))
		return false;
	for (std::pair<int, int> s : symbols[Decoration::Dot]) if (!place_dots(s.second, (s.first & 0xf), (s.first & ~0xf) == Decoration::Dot_Intersection))
		return false;
	for (std::pair<int, int> s : symbols[Decoration::Gap]) if (!place_gaps(s.second))
		return false;
	return true;
}

bool Generate::generate_path(PuzzleSymbols & symbols)
{
	clear();

	if (hasFlag(Config::SplitErasers)) {
		if ((_panel->_width / 2) % 2 == 0) {
			set(0, _panel->_height / 2, Decoration::Gap_Column); set(_panel->_width - 1, _panel->_height / 2, Decoration::Gap_Column);
		}
		else {
			set(_panel->_width / 2, 0, Decoration::Gap_Row); set(_panel->_width / 2, _panel->_height - 1, Decoration::Gap_Row);
		}
	}

	if (hasFlag(Config::TreehouseLayout)) {
		if (_starts.size() == 2) {
			set(_panel->_width / 2, 0, PATH);
			set(_panel->_width / 2, _panel->_height - 1, PATH);
		}
		if (_exits.size() == 6) { //Pivot panel
			set(0, _panel->_height / 2, PATH);
			set(_panel->_width - 1, _panel->_height / 2, PATH);
		}
	}

	if (_parity != -1) {
		return generate_longest_path();
	}

	if (hasFlag(Config::LongPath) || symbols.style == Panel::Style::HAS_DOTS && !hasFlag(Config::PreserveStructure) &&
		!(_panel->symmetry == Panel::Symmetry::Vertical && (_panel->_width / 2) % 2 == 0 ||
		  _panel->symmetry == Panel::Symmetry::Horizontal && (_panel->_height / 2) % 2 == 0)) {
		return generate_path_length(_panel->get_num_grid_points() * 7 / 8);
	}

	if (symbols.style == Panel::Style::HAS_STONES && !hasFlag(Config::SplitErasers))
		return generate_path_regions((_panel->_width / 2 + _panel->_height / 2) / 2 + 1);

	if (symbols.style == Panel::Style::HAS_SHAPERS) {
		if (hasFlag(Config::DisableCombineShapes)) {
			return generate_path_regions(symbols.getNum(Decoration::Poly) + 1);
		}
		return generate_path_length(1);
	}

	return generate_path_length(_panel->get_num_grid_points() * 3 / 4);
}

bool Generate::generate_path_length(int minLength)
{
	int fails = 0;
	Point pos = pick_random(_starts);
	Point exit = pick_random(_exits);
	set_path(pos);
	while (pos != exit) {
		if (fails++ > 20) return false;
		Point dir = pick_random(_DIRECTIONS2);
		Point newPos = pos + dir;
		if (off_edge(newPos) || get(newPos) != 0 || get(pos.first + dir.first / 2, pos.second + dir.second / 2) != 0
			|| newPos == exit && _path.size() / 2 + 2 < minLength) continue;
		if (_panel->symmetry && (off_edge(get_sym_point(newPos)) || newPos == get_sym_point(newPos))) continue;
		set_path(newPos);
		set_path(pos + Point(dir.first / 2, dir.second / 2));
		pos = newPos;
		fails = 0;
	}
	return _path.size() / 2 + 1 >= minLength;
}

bool Generate::generate_path_regions(int minRegions)
{
	int fails = 0;
	int regions = 1;
	Point pos = pick_random(_starts);
	Point exit = pick_random(_exits);
	set_path(pos);
	while (pos != exit) {
		if (fails++ > 20)
			return false;
		Point dir = pick_random(_DIRECTIONS2);
		Point newPos = pos + dir;
		if (off_edge(newPos) || get(newPos) != 0 || get(pos.first + dir.first / 2, pos.second + dir.second / 2) != 0
			|| newPos == exit && regions < minRegions) continue;
		if (_panel->symmetry && (off_edge(get_sym_point(newPos)) || newPos == get_sym_point(newPos))) continue;
		set_path(newPos);
		set_path(pos + Point(dir.first / 2, dir.second / 2));
		if (!on_edge(newPos) && on_edge(pos)) {
			regions++;
			if (_panel->symmetry) regions++;
		}
		pos = newPos;
		fails = 0;
	}
	return regions >= minRegions;
}

bool Generate::generate_longest_path()
{
	Point pos = pick_random(_starts);
	Point exit = pick_random(_exits);
	if (get_parity(pos + exit) != _panel->get_parity()) return false;
	int fails = 0;
	int reqLength = _panel->get_num_grid_points();
	bool centerFlag = !on_edge(pos);
	set_path(pos);
	while (pos != exit) {
		if (fails++ > 20) return false;
		Point dir = pick_random(_DIRECTIONS2);
		for (Point checkDir : _DIRECTIONS2) {
			Point check = pos + checkDir;
			if (off_edge(check) || get(check) != 0) continue;
			if (check == exit) continue;
			int open = 0;
			for (Point checkDir2 : _DIRECTIONS2) {
				if (!off_edge(check + checkDir2) && get(check + checkDir2) == 0) {
					if (++open >= 2) break;
				}
			}
			if (open < 2) {
				dir = checkDir;
				break;
			}
		}
		Point newPos = pos + dir;
		if (off_edge(newPos) || get(newPos) != 0 || get(pos.first + dir.first / 2, pos.second + dir.second / 2) != 0
			|| newPos == exit && _path.size() / 2 + 2 < reqLength) continue;
		if (_panel->symmetry && (off_edge(get_sym_point(newPos)) || newPos == get_sym_point(newPos))) continue;
		if (on_edge(newPos) && (off_edge(newPos + dir) || get(newPos + dir) != 0)) {
			if (centerFlag && off_edge(newPos + dir)) {
				centerFlag = false;
			}
			else {
				int open = 0;
				for (Point checkDir : _DIRECTIONS2) {
					if (!off_edge(newPos + checkDir) && get(newPos + checkDir) == 0) {
						if (++open >= 2) break;
					}
				}
				if (open >= 2) continue;
			}
		}
		set_path(newPos);
		set_path(pos + Point(dir.first / 2, dir.second / 2));
		pos = newPos;
		fails = 0;
	}
	return _path.size() / 2 + 1 == reqLength;
}

void Generate::erase_path()
{
	for (int y = 0; y < _panel->_height; y++) {
		for (int x = 0; x < _panel->_width; x++) {
			if (get(x, y) == PATH) {
				set(x, y, 0);
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
		for (Point dir : _DIRECTIONS1) {
			Point p1 = p + dir;
			if (on_edge(p1)) continue;
			if (get(p1) == PATH || get(p1) == OPEN) continue;
			Point p2 = Point(p.first + dir.first * 2, p.second + dir.second * 2);
			if ((get(p2) & Decoration::Empty) == Decoration::Empty) continue;
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
		if (get(p)) symbols.push_back(get(p));
	}
	return symbols;
}

bool Generate::place_start(int amount)
{
	_starts.clear();
	_panel->_startpoints.clear();
	while (amount > 0) {
		Point pos = Point(rand() % (_panel->_width / 2 + 1) * 2, rand() % (_panel->_height / 2 + 1) * 2);
		if (hasFlag(Config::StartEdgeOnly))
		switch (rand() % 4) {
		case 0: pos.first = 0; break;
		case 1: pos.second = 0; break;
		case 2: pos.first = _panel->_width - 1; break;
		case 3: pos.second = _panel->_height - 1; break;
		}
		if (_parity != -1 && get_parity(pos) != (amount == 1 ? _parity : !_parity)) continue;
		if (_starts.count(pos) || _exits.count(pos)) continue;
		bool adjacent = false;
		for (Point dir : _8DIRECTIONS2) {
			if (!off_edge(pos + dir) && get(pos + dir) == Decoration::Start) {
				adjacent = true;
				break;
			}
		}
		if (adjacent) continue;
		_starts.insert(pos);
		_panel->SetGridSymbol(pos.first, pos.second, Decoration::Start, Decoration::Color::None);
		amount--;
		if (_panel->symmetry) {
			Point sp = get_sym_point(pos);
			_starts.insert(sp);
			_panel->SetGridSymbol(sp.first, sp.second, Decoration::Start, Decoration::Color::None);
		}
	}
	return true;
}

bool Generate::place_exit(int amount)
{
	_exits.clear();
	_panel->_endpoints.clear();
	while (amount > 0) {
		Point pos = Point(rand() % (_panel->_width / 2 + 1) * 2, rand() % (_panel->_height / 2 + 1) * 2);
		switch (rand() % 4) {
		case 0: pos.first = 0; break;
		case 1: pos.second = 0; break;
		case 2: pos.first = _panel->_width - 1; break;
		case 3: pos.second = _panel->_height - 1; break;
		}
		if (_parity != -1 && (get_parity(pos) + _panel->get_parity()) % 2 != (amount == 1 ? _parity : !_parity)) continue;
		if (_starts.count(pos) || _exits.count(pos)) continue;
		bool adjacent = false;
		for (Point dir : _8DIRECTIONS2) {
			if (!off_edge(pos + dir) && get(pos + dir) == Decoration::Exit) {
				adjacent = true;
				break;
			}
		}
		if (adjacent) continue;
		_exits.insert(pos);
		_panel->SetGridSymbol(pos.first, pos.second, Decoration::Exit, Decoration::Color::None);
		amount--;
		if (_panel->symmetry) {
			Point sp = get_sym_point(pos);
			_exits.insert(sp);
			_panel->SetGridSymbol(sp.first, sp.second, Decoration::Exit, Decoration::Color::None);
		}
	}
	return true;
}

bool Generate::can_place_gap(Point pos) {
	if (pos.first == 0 || pos.second == 0) {
		if (hasFlag(Config::FullGaps)) return false;
	}
	else if (rand() % 2 == 0) return false; //Encourages gaps on outside border
	if (hasFlag(Config::FullGaps)) { //Prevent dead ends
		std::vector<Point> checkPoints = (pos.first % 2 == 0 ? std::vector<Point>({ Point(pos.first, pos.second - 1), Point(pos.first, pos.second + 1) })
			: std::vector<Point>({ Point(pos.first - 1, pos.second), Point(pos.first + 1, pos.second) }));
		for (Point check : checkPoints) {
			int valid = 4;
			for (Point dir : _DIRECTIONS1) {
				Point p = check + dir;
				if (off_edge(p) || get(p) & GAP || get(p) == OPEN) {
					if (--valid <= 2) {
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool Generate::place_gaps(int amount) {
	std::set<Point> open;
	for (int y = 0; y < _panel->_height; y++) {
		for (int x = (y + 1) % 2; x < _panel->_width; x += 2) {
			if (get(x, y) == 0 && (!_fullGaps || !on_edge(Point(x, y)))) {
				open.insert(Point(x, y));
			}
		}
	}

	while (amount > 0) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		if (can_place_gap(pos)) {
			set(pos, _fullGaps ? OPEN : pos.first % 2 == 0 ? Decoration::Gap_Column : Decoration::Gap_Row);
			amount--;
		}
		open.erase(pos);
	}
	return true;
}

bool Generate::can_place_dot(Point pos) {
	if (get(pos) & DOT)
		return false;
	for (Point dir : _8DIRECTIONS1) {
		Point p = pos + dir;
		if (!off_edge(p) && get(p) & DOT) {
			//Don't allow horizontally adjacent dots
			if (dir.first == 0 || dir.second == 0)
				return false;
			//Allow diagonal placement some of the time
			if (rand() % 2 > 0)
				return false;
		}
	}
	if (rand() % 10 > 0) {
		for (Point dir : _DIRECTIONS2) {
			Point p = pos + dir;
			if (!off_edge(p) && get(p) & DOT) {
				if (rand() % 4 > 0) //Add some variation
					return false;
			}
		}
	}
	return true;
}

bool Generate::place_dots(int amount, int color, bool intersectionOnly) {
	if (_parity != -1) {
		for (int x = 0; x < _panel->_width; x += 2) {
			for (int y = 0; y < _panel->_height; y += 2) {
				if (_starts.size() == 1 && _starts.count(Point(x, y))) continue;
				if (_exits.size() == 1 && _exits.count(Point(x, y))) continue;
				set(x, y, Decoration::Dot_Intersection);
			}
		}
		return true;
	}

	if (color == Decoration::Color::Blue || color == Decoration::Color::Cyan)
		color = IntersectionFlags::DOT_IS_BLUE;
	else if (color == Decoration::Color::Yellow || color == Decoration::Color::Orange)
		color = IntersectionFlags::DOT_IS_ORANGE;
	else color = 0;

	std::set<Point>& open = (color == 0 ? _path : color == IntersectionFlags::DOT_IS_BLUE ? _path1 : _path2);
	for (Point p : _starts) open.erase(p);
	for (Point p : _exits) open.erase(p);
	std::set<Point> intersections;
	if (intersectionOnly) {
		for (Point p : open) {
			if (p.first % 2 == 0 && p.second % 2 == 0)
				intersections.insert(p);
		}
		open = intersections;
	}

	while (amount > 0) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		if (_panel->symmetry && !_path.count(pos)) {
			open.erase(pos);
			continue;
		}
		open.erase(pos);
		if (!can_place_dot(pos)) continue;
		int symbol = (pos.first & 1) == 1 ? Decoration::Dot_Row : (pos.second & 1) == 1 ? Decoration::Dot_Column : Decoration::Dot_Intersection;
		set(pos, symbol | color);
		for (Point dir : _DIRECTIONS1) {
			open.erase(pos + dir);
			_path.erase(pos + dir);
		}
		if (_panel->symmetry) {
			Point sp = get_sym_point(pos);
			symbol = (sp.first & 1) == 1 ? Decoration::Dot_Row : (sp.second & 1) == 1 ? Decoration::Dot_Column : Decoration::Dot_Intersection;
			set(sp, symbol & ~Decoration::Dot);
			open.erase(sp);
			_path.erase(sp);
			for (Point dir : _DIRECTIONS1) {
				if (color == IntersectionFlags::DOT_IS_BLUE) _path2.erase(sp + dir);
				else _path1.erase(sp + dir);
				open.erase(sp + dir);
				_path.erase(sp + dir);
			}
		}
		amount--;
		_path.erase(pos);
	}
	return true;
}

bool Generate::place_stones(int color, int amount) {
	std::set<Point> open = _openpos;
	std::set<Point> open2;
	int passCount = 0;
	while (amount > 0) {
		if (open.size() == 0) {
			if (open2.size() < amount || _bisect && passCount < (_panel->_width / 2 + _panel->_height / 2) / 4)
				return false;
			Point pos = pick_random(open2);
			set(pos, Decoration::Stone | color);
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
		if (!pass) {
			for (Point p : region) {
				open.erase(p);
			}
			continue;
		}
		if (_stoneTypes > 2) {
			open = region;
		}
		open.erase(pos);
		if (_panel->symmetry) {
			open.erase(get_sym_point(pos));
		}
		if (_stoneTypes == 2) {
			for (Point p : region) {
				if (open.erase(p)) open2.insert(p);
			}
			for (Point p : region) {
				for (Point dir : _8DIRECTIONS2) {
					Point pos2 = p + dir;
					if (open.count(pos2) && !region.count(pos2)) {
						for (Point P : get_region(pos2)) {
							open.erase(P);
						}
					}
				}
			}
		}
		set(pos, Decoration::Stone | color);
		_openpos.erase(pos);
		amount--;
		passCount++;
	}
	_bisect = false;
	_stoneTypes--;
	return true;
}

Shape Generate::generate_shape(std::set<Point>& region, std::set<Point>& bufferRegion, Point pos, int maxSize)
{
	Shape shape;
	shape.insert(pos);
	if (!bufferRegion.erase(pos))
		region.erase(pos);
	while (shape.size() < maxSize && region.size() > 0) {
		pos = pick_random(shape);
		int i = 0;
		for (; i < 10; i++) {
			Point dir = pick_random(_SHAPEDIRECTIONS);
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
	if (xmax - xmin > 6 || ymax - ymin > 6)
		return 0; //Shapes cannot be more than 4 in width and height
	for (Point p : shape) {
		symbol |= (1 << ((p.first - xmin) / 2 + (ymax  - p.second) * 2)) << 16;
	}
	return symbol;
}

bool Generate::place_shapes(std::vector<int> colors, std::vector<int> negativeColors, int amount, int numRotated, int numNegative)
{
	std::set<Point> open = _openpos;
	int shapeSize = hasFlag(Config::SmallShapes) ? 2 : 4;
	int targetArea = amount * shapeSize * 7 / 8;
	int totalArea = 0;
	int colorIndex = rand() % colors.size();
	int colorIndexN = rand() % (negativeColors.size() + 1);
	bool shapesCanceled = false, shapesCombined = false;
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
		if (region.size() + totalArea == _panel->get_num_grid_blocks()) continue;
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
					Point p = pos + pick_random(_SHAPEDIRECTIONS);
					if (regionN.count(p) && !region.count(p)) {
						pos = p;
						break;
					}
				}
				if (!regionN.count(pos)) return false;
				Shape shape = generate_shape(regionN, pos, min(rand() % 3 + 1, maxSize));
				shapesN.push_back(shape);
				for (Point p : shape) {
					if (region.count(p)) bufferRegion.insert(p);
					else region.insert(p);
				}
			}
		}
		int numShapes = static_cast<int>(region.size() + bufferRegion.size()) / (shapeSize + 1) + 1;
		if (numShapes == 1 && bufferRegion.size() > 0) numShapes++;
		if (numShapes < amount && region.size() > shapeSize && rand() % 2 == 1) numShapes++; //Adds more variation to the shape sizes
		if (hasFlag(Config::DisableCombineShapes) && numShapes != 1) continue;
		bool balance = false;
		if (numShapes > amount) {
			if (numNegative < 2 || hasFlag(Config::DisableCancelShapes)) continue;
			//Make balancing shapes - Positive and negative will be switched so that code can be reused
			balance = true;
			shapesCanceled = true;
			std::set<Point> regionN = _gridpos;
			numShapes = max(2, rand() % numNegative + 1);			//Actually the negative shapes
			numShapesN = min(amount, numShapes * 2 / 5 + 1);		//Actually the positive shapes
			if (numShapesN >= numShapes * 3 || numShapesN * 5 <= numShapes) continue;
			shapes.clear();
			shapesN.clear();
			region.clear();
			bufferRegion.clear();
			for (int i = 0; i < numShapesN; i++) {
				Shape shape = generate_shape(regionN, pick_random(regionN), min(5, numShapes * 2 / numShapesN + rand() % 3 - 1));
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
			shapes.push_back(generate_shape(region, bufferRegion, pick_random(region), balance ? rand() % 3 + 1 : shapeSize));
			shapesCombined = true;
		}
		//Take remaining area and try to stick it to existing shapes
		multibreak:
		while (region.size() > 0) {
			pos = pick_random(region);
			for (Shape& shape : shapes) {
				if (shape.size() > shapeSize || shape.count(pos) > 0) continue;
				for (Point p : shape) {
					for (Point dir : _DIRECTIONS2) {
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
			std::swap(shapes, shapesN);
		}
		numShapes = static_cast<int>(shapes.size());
		for (Shape& shape : shapesN) {
			shapes.push_back(shape);
		}
		if (hasFlag(Config::DisconnectShapes)) {
			//Make sure at least one shape is disconnected
			bool disconnect = false;
			for (Shape& shape : shapes) {
				if (shape.size() == 1) continue;
				disconnect = true;
				for (Point p : shape) {
					for (Point dir : _DIRECTIONS2) {
						if (shape.count(p + dir)) {
							disconnect = false;
							break;
						}
					}
					if (!disconnect) break;
				}
				if (disconnect) break;
			}
			if (!disconnect) continue;
		}	
		numNegative -= static_cast<int>(shapesN.size());
		for (Shape& shape : shapes) {
			int symbol = make_shape_symbol(shape, (numRotated-- > 0), (numShapes-- <= 0));
			if (symbol == 0)
				return false;
			//Attempt not to put shapes adjacent
			Point pos;
			for (int i = 0; i < 10; i++) {
				pos = pick_random(open2);
				bool pass = true;
				for (Point dir : _8DIRECTIONS2) {
					Point p = pos + dir;
					if (!off_edge(p) && get(p) & Decoration::Poly) {
						pass = false;
						break;
					}
				}
				if (pass) break;
			}
			if (symbol & Decoration::Negative) set(pos, symbol | negativeColors[(colorIndexN++) % negativeColors.size()]);
			else {
				set(pos, symbol | colors[(colorIndex++) % colors.size()]);
				totalArea += static_cast<int>(shape.size());
				amount--;
			}
			open2.erase(pos);
			_openpos.erase(pos);
		}
	}
	if (totalArea < targetArea || numNegative > 0 || hasFlag(Config::RequireCancelShapes) && !shapesCanceled || hasFlag(Config::RequireCombineShapes) && !shapesCombined)
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
		set(pos, Decoration::Star | color);
		_openpos.erase(pos);
		amount--;
		if (count == 0) {
			open2.erase(pos);
			if (open2.size() == 0)
				return false;
			pos = pick_random(open2);
			set(pos, Decoration::Star | color);
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
		for (Point dir : _DIRECTIONS1) {
			Point p = pos + dir;
			if (!off_edge(p) && get(p) == PATH) {
				count++;
			}
		}
		open.erase(pos);
		if (_panel->symmetry) {
			open.erase(get_sym_point(pos));
		}
		if (count == 0) continue;
		if (count == 2 && rand() % 2 == 0) //Prevent it from placing so many 2's
			continue;
		set(pos, Decoration::Triangle | color | (count << 16));
		_openpos.erase(pos);
		amount--;
	}
	return true;
}

bool Generate::place_erasers(std::vector<int> colors, std::vector<int> eraseSymbols)
{
	std::set<Point> open = _openpos;
	int amount = static_cast<int>(colors.size());
	int pointNum = amount;
	while (amount > 0) {
		if (open.size() == 0)
			return false;
		int toErase = eraseSymbols[amount - 1];
		int color = colors[amount - 1];
		Point pos = pick_random(open);
		if (hasFlag(Config::SplitErasers)) {
			if (pointNum < amount)
				return false;
			pos = _splitPoints[pointNum - 1];
			pointNum--;
		}
		open.erase(pos);
		std::set<Point> region = get_region(pos);
		std::set<Point> open2;
		for (Point p : region) {
			if (open.erase(p)) open2.insert(p);
		}
		if (hasFlag(Config::MakeStonesUnsolvable)) {
			std::set<Point> valid;
			std::set<Point> check = open2;
			std::set<Point> test;
			for (Point p : open2) {
				//Try to make a checkerboard pattern with the stones
				if (!off_edge(p + Point(2, 2)) && get(p + Point(2, 2)) == toErase && get(p + Point(0, 2)) != 0 && get(p + Point(0, 2)) != toErase && get(p + Point(2, 0)) != 0 && get(p + Point(2, 0)) != toErase ||
					!off_edge(p + Point(-2, 2)) && get(p + Point(-2, 2)) == toErase && get(p + Point(0, 2)) != 0 && get(p + Point(0, 2)) != toErase && get(p + Point(-2, 0)) != 0 && get(p + Point(-2, 0)) != toErase ||
					!off_edge(p + Point(2, -2)) && get(p + Point(2, -2)) == toErase && get(p + Point(0, -2)) != 0 && get(p + Point(0, -2)) != toErase && get(p + Point(2, 0)) != 0 && get(p + Point(2, 0)) != toErase ||
					!off_edge(p + Point(-2, -2)) && get(p + Point(-2, -2)) == toErase && get(p + Point(0, -2)) != 0 && get(p + Point(0, -2)) != toErase && get(p + Point(-2, 0)) != 0 && get(p + Point(-2, 0)) != toErase)
					valid.insert(p);
			}
			open2 = valid;
		}
		
		if (open2.size() == 0 && !(toErase & Decoration::Dot)) continue;
		bool canPlace = false;
		if (get_symbol_type(toErase) == Decoration::Stone) {
			std::vector<int> symbols = get_symbols_in_region(region);
			for (int s : symbols) {
				if (get_symbol_type(s) == Decoration::Stone) {
					canPlace = (s & 0xf) != (toErase & 0xf);
					break;
				}
			}
		}
		else if (get_symbol_type(toErase) == Decoration::Star) {
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
		set(pos, Decoration::Eraser | color);
		_openpos.erase(pos);
		amount--;
		if (!(toErase & Decoration::Dot)) {
			pos = pick_random(open2);
			_openpos.erase(pos);
		}
		if (get_symbol_type(toErase) == Decoration::Stone || get_symbol_type(toErase) == Decoration::Star) {
			set(pos, toErase);
		}
		else if (toErase & Decoration::Dot) {
			std::set<Point> openEdge;
			for (Point p : region) {
				for (Point dir : _8DIRECTIONS1) {
					if (toErase == Decoration::Dot_Intersection && (dir.first == 0 || dir.second == 0)) continue;
					Point p2 = pos + dir;
					if (get(p2) == 0 && can_place_dot(p2)) {
						openEdge.insert(p2);
					}
				}
			}
			if (openEdge.size() == 0)
				return false;
			pos = pick_random(openEdge);
			toErase &= ~IntersectionFlags::INTERSECTION;
			if ((toErase & 0xf) == Decoration::Color::Blue || (toErase & 0xf) == Decoration::Color::Cyan) toErase |= IntersectionFlags::DOT_IS_BLUE;
			if ((toErase & 0xf) == Decoration::Color::Yellow || (toErase & 0xf) == Decoration::Color::Orange) toErase |= IntersectionFlags::DOT_IS_ORANGE;
			toErase &= ~0x4000f;
			if ((pos.first & 1) == 0 && (pos.second & 1) == 0) toErase |= Decoration::Dot_Intersection;
			else if ((pos.second & 1) == 0) toErase |= Decoration::Dot_Row;
			set(pos, ((pos.first & 1) == 1 ? Decoration::Dot_Row : (pos.second & 1) == 1 ? Decoration::Dot_Column : Decoration::Dot_Intersection) | (toErase & 0xffff));
		}
		else if (get_symbol_type(toErase) == Decoration::Poly) {
			int symbol = 0;
			while (symbol == 0) {
				std::set<Point> area = _gridpos;
				Shape shape = generate_shape(area, pick_random(area), rand() % ((toErase & Decoration::Negative) || hasFlag(Config::SmallShapes) ? 3 : 5) + 1);
				if (shape.size() == region.size()) continue;
				symbol = make_shape_symbol(shape, toErase & Decoration::Can_Rotate, toErase & Decoration::Negative);
			}
			set(pos, symbol | (toErase & 0xf));
		}
		else if (get_symbol_type(toErase) == Decoration::Triangle) {
			int count = 0;
			for (Point dir : _DIRECTIONS1) {
				Point p = pos + dir;
				if (!off_edge(p) && get(p) == PATH) {
					count++;
				}
			}
			count = (count + (rand() & 1)) % 3 + 1;
			set(pos, toErase | (count << 16));
		}
	}
	return true;
}




//Isn't actually making it any faster for some reason, so I'm not using it
/*std::vector<Point> Generate::get_valid_edge_dir(Point lastPoint, Point exit)
{
	std::vector<Point> validDir;
	for (Point dir : _DIRECTIONS2) {
		Point pos = lastPoint + dir;
		if (!on_edge(pos)) continue;
		Point checkDir = dir;
		while (get(pos) != PATH) {
			Point newPos = pos + checkDir;
			if (newPos == exit) {
				validDir.push_back(dir);
				break;
			}
			if (off_edge(newPos)) {
				if (newPos.first == 0) checkDir = Point(2, 0); //Right
				if (newPos.second == 0) checkDir = Point(0, 2); //Up
				if (newPos.first == _panel->_width - 1) checkDir = Point(-2, 0); //Left
				if (newPos.second == _panel->_height - 1) checkDir = Point(0, -2); //Down
				newPos = pos + checkDir;
			}
			pos = newPos;
		}
	}
	return validDir;
}*/