#include "Generate.h"
#include "Randomizer.h"
#include "MultiGenerate.h"
#include "Special.h"

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

void Generate::generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5, int symbol6, int amount6) {
	PuzzleSymbols symbols({ std::make_pair(symbol1, amount1), std::make_pair(symbol2, amount2), std::make_pair(symbol3, amount3), std::make_pair(symbol4, amount4),  std::make_pair(symbol5, amount5), std::make_pair(symbol6, amount6) });
	while (!generate(id, symbols));
}

void Generate::generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5, int symbol6, int amount6, int symbol7, int amount7) {
	PuzzleSymbols symbols({ std::make_pair(symbol1, amount1), std::make_pair(symbol2, amount2), std::make_pair(symbol3, amount3), std::make_pair(symbol4, amount4),  std::make_pair(symbol5, amount5), std::make_pair(symbol6, amount6), std::make_pair(symbol7, amount7) });
	while (!generate(id, symbols));
}

void Generate::generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5, int symbol6, int amount6, int symbol7, int amount7, int symbol8, int amount8) {
	PuzzleSymbols symbols({ std::make_pair(symbol1, amount1), std::make_pair(symbol2, amount2), std::make_pair(symbol3, amount3), std::make_pair(symbol4, amount4),  std::make_pair(symbol5, amount5), std::make_pair(symbol6, amount6), std::make_pair(symbol7, amount7), std::make_pair(symbol8, amount8) });
	while (!generate(id, symbols));
}

void Generate::generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5, int symbol6, int amount6, int symbol7, int amount7, int symbol8, int amount8, int symbol9, int amount9) {
	PuzzleSymbols symbols({ std::make_pair(symbol1, amount1), std::make_pair(symbol2, amount2), std::make_pair(symbol3, amount3), std::make_pair(symbol4, amount4),  std::make_pair(symbol5, amount5), std::make_pair(symbol6, amount6), std::make_pair(symbol7, amount7), std::make_pair(symbol8, amount8), std::make_pair(symbol9, amount9) });
	while (!generate(id, symbols));
}

void Generate::generate(int id, std::vector<std::pair<int, int>> symbolVec)
{
	PuzzleSymbols symbols(symbolVec);
	while (!generate(id, symbols));
}

void Generate::generateMulti(int id, std::vector<std::shared_ptr<Generate>> gens, std::vector<std::pair<int, int>> symbolVec)
{
	MultiGenerate gen;
	gen.splitStones = hasFlag(Config::SplitStones);
	gen.generate(id, gens, symbolVec);
	incrementProgress();
}

void Generate::generateMulti(int id, int numSolutions, std::vector<std::pair<int, int>> symbolVec)
{
	MultiGenerate gen;
	gen.splitStones = hasFlag(Config::SplitStones);
	std::vector<std::shared_ptr<Generate>> gens;
	for (; numSolutions > 0; numSolutions--) gens.push_back(std::make_shared<Generate>());
	gen.generate(id, gens, symbolVec);
	incrementProgress();
}

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
		_panel->Resize(Point::pillarWidth ? _width - 1 : _width, _height);
	}
	if (hasFlag(Config::FixBackground)) {
		_panel->Resize(_panel->_width, _panel->_height); //This will force the panel to have to redraw the background
	}
	if (hasFlag(Config::TreehouseLayout)) {
		init_treehouse_layout();
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
		if (hasFlag(Config::PreserveStructure)) {
			for (int x = 0; x < _panel->_width; x++)
				for (int y = 0; y < _panel->_height; y++)
					if (_panel->_grid[x][y] == OPEN || (_panel->_grid[x][y] & 0x60000f) == NO_POINT || (_panel->_grid[x][y] & Decoration::Empty) == Decoration::Empty)
						_custom_grid[x][y] = _panel->_grid[x][y];
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
	_gridpos.clear();
	for (int x = 1; x < _panel->_width; x += 2) {
		for (int y = 1; y < _panel->_height; y += 2) {
			if (!(hasFlag(Config::PreserveStructure) && (get(x, y) & Decoration::Empty) == Decoration::Empty))
				_gridpos.insert(Point(x, y));
		}
	}
	if (openPos.size() > 0) _openpos = openPos;
	else _openpos = _gridpos;
	for (Point p : blockPos) _openpos.erase(p);
	for (Point p : _splitPoints) _openpos.erase(p);
	_fullGaps = hasFlag(Config::FullGaps);
	if (_symmetry || id == 0x00076 || id == 0x01D3F) _panel->symmetry = _symmetry;
	if (pathWidth != 1) _panel->pathWidth = pathWidth;
}

void Generate::setSymbol(Decoration::Shape symbol, int x, int y)
{
	if (_custom_grid.size() < x + 1) {
		_custom_grid.resize(x + 1, std::vector<int>());
		for (auto& row : _custom_grid) {
			row.resize(_custom_grid[0].size(), 0);
		}
	}
	for (auto& row : _custom_grid) {
		if (row.size() < y + 1) {
			row.resize(y + 1, 0);
		}
	}

	if (symbol == Decoration::Start) _starts.insert(Point(x, y));
	else if (symbol == Decoration::Exit) _exits.insert(Point(x, y));
	else _custom_grid[x][y] = symbol; //Starts and exits are not set into the grid
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
	if (_symmetry == Panel::Symmetry::ParallelV || _symmetry == Panel::Symmetry::ParallelVFlip) {
		std::vector<Point> points;
		for (int y = 0; y < _height; y += 2) points.push_back(Point(_width / 2, y));
		setObstructions(points);
	}
	if (_symmetry == Panel::Symmetry::ParallelH || _symmetry == Panel::Symmetry::ParallelHFlip) {
		std::vector<Point> points;
		for (int x = 0; x < _width; x += 2) points.push_back(Point(x, _height / 2));
		setObstructions(points);
	}
}

void Generate::write(int id)
{
	std::vector<std::vector<int>> backupGrid;
	if (hasFlag(Config::DisableReset)) backupGrid = _panel->_grid;

	erase_path();

	incrementProgress();

	if (hasFlag(Config::ResetColors)) {
		_panel->colorMode = 0;
	}
	else if (hasFlag(Config::AlternateColors)) {
		_panel->colorMode = 1;
	}
	else if (hasFlag(Config::WriteColors)) {
		_panel->colorMode = 2;
	}
	if (hasFlag(Config::Write2Color)) {
		Special::WritePanelData(id, PATTERN_POINT_COLOR_A, _panel->_memory->ReadPanelData<Color>(0x0007C, PATTERN_POINT_COLOR_A));
		Special::WritePanelData(id, PATTERN_POINT_COLOR_B, _panel->_memory->ReadPanelData<Color>(0x0007C, PATTERN_POINT_COLOR_B));
		Special::WritePanelData(id, REFLECTION_PATH_COLOR, _panel->_memory->ReadPanelData<Color>(0x0007C, PATTERN_POINT_COLOR_B));
		Special::WritePanelData(id, ACTIVE_COLOR, _panel->_memory->ReadPanelData<Color>(0x0007C, PATTERN_POINT_COLOR_A));
	}
	if (hasFlag(Config::WriteInvisible)) {
		Special::WritePanelData(id, REFLECTION_PATH_COLOR, _panel->_memory->ReadPanelData<Color>(0x00076, REFLECTION_PATH_COLOR));
	}
	if (hasFlag(Config::WriteDotColor))
		Special::WritePanelData(id, PATTERN_POINT_COLOR, { 0.1f, 0.1f, 0.1f, 1 });
	if (hasFlag(Config::WriteDotColor2)) {
		Color color = _panel->_memory->ReadPanelData<Color>(id, SUCCESS_COLOR_A);
		Special::WritePanelData(id, PATTERN_POINT_COLOR, color);
	}
	if (hasFlag(Config::ArrowRecolor)) {
		Special::WritePanelData(id, OUTER_BACKGROUND, { backgroundColor });
		if (arrowColor.a == 0)
			Special::WritePanelData(id, BACKGROUND_REGION_COLOR, { _panel->_memory->ReadPanelData<Color>(id, SUCCESS_COLOR_A) });
		Special::WritePanelData(id, BACKGROUND_REGION_COLOR, { arrowColor });
		Special::WritePanelData(id, OUTER_BACKGROUND_MODE, 1);
		if (successColor.a == 0) Special::WritePanelData(id, SUCCESS_COLOR_A, _panel->_memory->ReadPanelData<Color>(id, BACKGROUND_REGION_COLOR));
		else Special::WritePanelData(id, SUCCESS_COLOR_A, successColor);
		Special::WritePanelData(id, SUCCESS_COLOR_B, _panel->_memory->ReadPanelData<Color>(id, SUCCESS_COLOR_A));
		Special::WritePanelData(id, ACTIVE_COLOR, { 1, 1, 1, 1 });
		Special::WritePanelData(id, REFLECTION_PATH_COLOR, { 1, 1, 1, 1 });
	}

	_panel->decorationsOnly = hasFlag(Config::DecorationsOnly);
	_panel->Write(id);
	
	if (hasFlag(Config::DisableReset)) _panel->_grid = backupGrid;
	else resetVars();

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
		resetVars();
	}
	_config = 0;
	_oneTimeAdd = Config::None;
	_oneTimeRemove = Config::None;
}

void Generate::incrementProgress()
{
	_areaTotal++;
	_genTotal++;
	if (_handle) {
		int total = (_totalPuzzles == 0 ? _areaPuzzles : _totalPuzzles);
		if (total == 0) return;
		std::wstring text = _areaName + L": " + std::to_wstring(_areaTotal) + L"/" + std::to_wstring(_areaPuzzles) + L" (" + std::to_wstring(_genTotal * 100 / total) + L"%)";
		SetWindowText(_handle, text.c_str());
	}
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
	if (_custom_grid.size() > 0) {
		_panel->_grid = _custom_grid;
	}
	else for (int x = 0; x < _panel->_width; x++) {
		for (int y = 0; y < _panel->_height; y++) {
			if (hasFlag(Config::PreserveStructure) && (_panel->_grid[x][y] == OPEN || (_panel->_grid[x][y] & 0x60000f) == NO_POINT || (_panel->_grid[x][y] & Decoration::Empty) == Decoration::Empty)) continue;
			_panel->_grid[x][y] = 0;
		}
	}
	_panel->_style &= ~0x2ff8; //Remove all element flags
	_path.clear(); _path1.clear(); _path2.clear();
}

void Generate::resetVars() {
	_panel = NULL; //This is needed for the generator to read in the next panel
	_starts.clear();
	_exits.clear();
	_custom_grid.clear();
	hitPoints.clear();
	_obstructions.clear();
	openPos.clear();
	blockPos.clear();
	customPath.clear();
	_splitPoints.clear();
}

void Generate::init_treehouse_layout()
{
	bool pivot = _panel->_endpoints.size() > 2;
	setSymbol(Decoration::Start, _panel->_width / 2, _panel->_height - 1);
	setSymbol(Decoration::Exit, _panel->_width / 2, 0);
	if (pivot) {
		setSymbol(Decoration::Exit, _panel->_width - 1, _panel->_height / 2 );
		setSymbol(Decoration::Exit, 0, _panel->_height / 2);
	}
}

bool Generate::generate_maze(int id, int numStarts, int numExits)
{
	initPanel(id);

	if (numStarts > 0) place_start(numStarts);
	if (numExits > 0) place_exit(numExits);

	if (id == 0x00083 && _width == 15 && _height == 15) {
		clear();
		_panel->_endpoints.clear();
		_exits.clear();
		Point start = pick_random(_starts);
		_panel->SetGridSymbol(start.first, start.second, Decoration::Exit, Decoration::Color::None);
		Point sp = get_sym_point(start);
		_panel->SetGridSymbol(sp.first, sp.second, Decoration::Exit, Decoration::Color::None);
		set_path(start); set_path(sp);
	}
	else {
		for (Point p : _starts)
			if (_exits.count(p))
				return false;

		clear();
		while (!generate_path_length(_panel->_width + _panel->_height)) clear();
	}
	
	std::set<Point> path = _path; //Backup

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
			set_path(pos + dir / 2);
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

	_path = path;
	if (!hasFlag(Config::DisableWrite)) write(id);
	return true;
}

bool Generate::generate(int id, PuzzleSymbols symbols)
{
	initPanel(id);

	if (symbols.getNum(Decoration::Eraser) > 1 && !hasFlag(Config::CombineErasers) && id != 0x09DB5) {
		setSymbol(Decoration::Gap_Row, 1, 0);
		setSymbol(Decoration::Gap_Row, _panel->_width - 2, _panel->_height - 1);
		_splitPoints = { Point(1, 1), Point(_panel->_width - 2, _panel->_height - 2) };
		initPanel(id);
	}

	if (symbols.getNum(Decoration::Dot) >= _panel->get_num_grid_points() - 2)
		_parity = (_panel->get_parity() + (
			!symbols.any(Decoration::Start) ? get_parity(pick_random(_starts)) :
			!symbols.any(Decoration::Exit) ? get_parity(pick_random(_exits)) : rand() % 2)) % 2;
	else _parity = -1;

	if (symbols.any(Decoration::Start)) place_start(symbols.getNum(Decoration::Start));
	if (symbols.any(Decoration::Exit)) place_exit(symbols.getNum(Decoration::Exit));

	if (customPath.size() == 0) {
		int fails = 0;
		while (!generate_path(symbols)) {
			if (fails++ > 20) return false;
		}
	}
	else _path = customPath;

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

	if (!place_all_symbols(symbols))
		return false;

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
			if (!hasFlag(Config::CombineErasers)) eraseSymbols.push_back(hasFlag(Config::FalseParity) ? Decoration::Dot_Intersection : symbols.popRandomSymbol());
		}
	}
	if (hasFlag(Config::CombineErasers)) eraseSymbols.push_back(hasFlag(Config::FalseParity) ? Decoration::Dot_Intersection : symbols.popRandomSymbol());

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
	if (numShapes > 0 && !place_shapes(colors, negativeColors, numShapes, numRotate, numNegative))
		return false;
	_stoneTypes = static_cast<int>(symbols[Decoration::Stone].size());
	_bisect = true;
	for (std::pair<int, int> s : symbols[Decoration::Stone]) if (!place_stones(s.first & 0xf, s.second))
		return false;
	for (std::pair<int, int> s : symbols[Decoration::Triangle]) if (!place_triangles(s.first & 0xf, s.second, s.first >> 16))
		return false;
	for (std::pair<int, int> s : symbols[Decoration::Arrow]) if (!place_arrows(s.first & 0xf, s.second, s.first >> 12))
		return false;
	for (std::pair<int, int> s : symbols[Decoration::Star]) if (!place_stars(s.first & 0xf, s.second))
		return false;
	if (eraserColors.size() > 0 && !place_erasers(eraserColors, eraseSymbols))
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

	if (_obstructions.size() > 0) {
		std::vector<Point> walls = pick_random(_obstructions);
		for (Point p : walls) if (get(p) == 0) set(p, p.first % 2 == 0 ? Decoration::Gap_Column : Decoration::Gap_Row);
		bool result = (hasFlag(Config::ShortPath) ? generate_path_length(1) : _parity != -1 ? generate_longest_path() : 
			hitPoints.size() > 0 ? generate_special_path() : generate_path_length(_panel->get_num_grid_points() * 3 / 4));
		for (Point p : walls) if (get(p) & Decoration::Gap) set(p, 0);
		return result;
	}

	if (hitPoints.size() > 0) {
		return generate_special_path();
	}

	if (_parity != -1 || hasFlag(Generate::LongestPath)) {
		return generate_longest_path();
	}

	if (hasFlag(Config::ShortPath))
		return generate_path_length(1);

	if (_panel->symmetry == Panel::Symmetry::FlipXY || _panel->symmetry == Panel::Symmetry::FlipNegXY) {
		return generate_path_length(_panel->get_num_grid_points() * 3 / 4 - _panel->_width / 2);
	}

	if (hasFlag(Config::LongPath) || symbols.style == Panel::Style::HAS_DOTS && !hasFlag(Config::PreserveStructure) &&
		!(_panel->symmetry == Panel::Symmetry::Vertical && (_panel->_width / 2) % 2 == 0 ||
			_panel->symmetry == Panel::Symmetry::Horizontal && (_panel->_height / 2) % 2 == 0)) {
		return generate_path_length(_panel->get_num_grid_points() * 7 / 8);
	}

	if (symbols.style == Panel::Style::HAS_STONES && _splitPoints.size() == 0)
		return generate_path_regions(min(symbols.getNum(Decoration::Stone), (_panel->_width / 2 + _panel->_height / 2) / 2 + 1));

	if (symbols.style == Panel::Style::HAS_SHAPERS) {
		if (hasFlag(Config::SplitShapes)) {
			return generate_path_regions(symbols.getNum(Decoration::Poly) + 1);
		}
		return generate_path_length(1);
	}

	return generate_path_length(_panel->get_num_grid_points() * 3 / 4);
}

bool Generate::generate_path_length(int minLength)
{
	int fails = 0;
	Point pos = adjust_point(pick_random(_starts));
	Point exit = adjust_point(pick_random(_exits));
	if (off_edge(pos) || off_edge(exit))
		return false;
	set_path(pos);
	while (pos != exit) {
		if (fails++ > 20)
			return false;
		Point dir = pick_random(_DIRECTIONS2);
		Point newPos = pos + dir;
		if (off_edge(newPos) || get(newPos) != 0 || get(pos + dir / 2) != 0
			|| newPos == exit && _path.size() / 2 + 2 < minLength) continue;
		if (_panel->symmetry && (off_edge(get_sym_point(newPos)) || newPos == get_sym_point(newPos)))
			continue;
		set_path(newPos);
		set_path(pos + dir / 2);
		pos = newPos;
		fails = 0;
	}
	return _path.size() / 2 + 1 >= minLength;
}

bool Generate::generate_path_regions(int minRegions)
{
	int fails = 0;
	int regions = 1;
	Point pos = adjust_point(pick_random(_starts));
	Point exit = adjust_point(pick_random(_exits));
	if (off_edge(pos) || off_edge(exit)) return false;
	set_path(pos);
	while (pos != exit) {
		if (fails++ > 20)
			return false;
		Point dir = pick_random(_DIRECTIONS2);
		Point newPos = pos + dir;
		if (off_edge(newPos) || get(newPos) != 0 || get(pos + dir / 2) != 0
			|| newPos == exit && regions < minRegions)
			continue;
		if (_panel->symmetry && (off_edge(get_sym_point(newPos)) || newPos == get_sym_point(newPos))) continue;
		set_path(newPos);
		set_path(pos + dir / 2);
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
	Point pos = adjust_point(pick_random(_starts));
	Point exit = adjust_point(pick_random(_exits));
	if (off_edge(pos) || off_edge(exit)) return false;
	Point block(-10, -10);
	if (hasFlag(Config::FalseParity)) {
		if (get_parity(pos + exit) == _panel->get_parity())
			return false;
		block = Point(rand() % (_panel->_width / 2 + 1) * 2, rand() % (_panel->_height / 2 + 1) * 2);
		while (pos == block || exit == block) {
			block = Point(rand() % (_panel->_width / 2 + 1) * 2, rand() % (_panel->_height / 2 + 1) * 2);
		}
		set_path(block);
	}
	else if (get_parity(pos + exit) != _panel->get_parity())
		return false;
	int fails = 0;
	int reqLength = _panel->get_num_grid_points();
	bool centerFlag = !on_edge(pos);
	set_path(pos);
	while (pos != exit && !(_panel->symmetry && get_sym_point(pos) == exit)) {
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
		if (fails++ > 20)
			return false;
		Point dir = pick_random(_DIRECTIONS2);
		for (Point checkDir : _DIRECTIONS2) {
			Point check = pos + checkDir;
			if (off_edge(check) || get(check) != 0)
				continue;
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
		if (off_edge(newPos) || get(newPos) != 0 || get(pos + dir / 2) != 0
			|| newPos == exit && _path.size() / 2 + 3 < reqLength ||
			_panel->symmetry && get_sym_point(newPos) == exit && _path.size() / 2 + 3 < reqLength) continue;
		if (_panel->symmetry && (off_edge(get_sym_point(newPos)) || newPos == get_sym_point(newPos))) continue;
		if (on_edge(newPos) && Point::pillarWidth == 0 && _panel->symmetry != Panel::Symmetry::Horizontal && newPos + dir != block && (off_edge(newPos + dir) || get(newPos + dir) != 0)) {
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
		set_path(pos + dir / 2);
		pos = newPos;
		fails = 0;
	}
	if (!off_edge(block))
		set(block, 0);
	return _path.size() / 2 + 1 == reqLength;
}

bool Generate::generate_special_path()
{
	Point pos = adjust_point(pick_random(_starts));
	Point exit = adjust_point(pick_random(_exits));
	if (off_edge(pos) || off_edge(exit))
		return false;
	set_path(pos);
	for (Point p : hitPoints) {
		set(p, PATH);
	}
	int hitIndex = 0;
	int minLength = _panel->get_num_grid_points() * 3 / 4;
	while (pos != exit) {
		std::vector<Point> validDir;
		for (Point dir : _DIRECTIONS2) {
			Point newPos = pos + dir;
			if (off_edge(newPos)) continue;
			Point connectPos = pos + dir / 2;
			if (get(connectPos) == PATH && hitIndex < hitPoints.size() && connectPos == hitPoints[hitIndex]) {
				validDir = { dir };
				hitIndex++;
				break;
			}
			if (get(newPos) != 0 || get(connectPos) != 0 || newPos == exit && (hitIndex != hitPoints.size() || _path.size() / 2 + 2 < minLength)) continue;
			if (_panel->symmetry && newPos == get_sym_point(newPos)) continue;
			bool fail = false;
			for (Point dir : _DIRECTIONS1) {
				if (!off_edge(newPos + dir) && get(newPos + dir) == PATH && newPos + dir != hitPoints[hitIndex]) {
					fail = true;
					break;
				}
			}
			if (fail) continue;
			validDir.push_back(dir);
		}
		if (validDir.size() == 0)
			return false;
		Point dir = pick_random(validDir);
		set_path(pos + dir);
		set_path(pos + dir / 2);
		pos = pos + dir;
	}
	return hitIndex == hitPoints.size() && _path.size() >= minLength;
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

Point Generate::adjust_point(Point pos) {
	if (pos.first % 2 != 0) {
		if (get(pos) != 0) return { -10, -10 };
		//set_path(pos);
		return Point(pos.first - 1 + rand() % 2 * 2, pos.second);
	}
	if (pos.second % 2 != 0) {
		if (get(pos) != 0) return { -10, -10 };
		//set_path(pos);
		return Point(pos.first, pos.second - 1 + rand() % 2 * 2);
	}
	if (_panel->symmetry && _exits.count(pos) && !_exits.count(get_sym_point(pos))) return { -10, -10 };
	return pos;
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
			Point p2 = p + dir * 2;
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
		if (_panel->symmetry && pos == get_sym_point(pos)) continue;
		bool adjacent = false;
		for (Point dir : _8DIRECTIONS2) {
			if (!off_edge(pos + dir) && get(pos + dir) == Decoration::Start) {
				adjacent = true;
				break;
			}
		}
		if (adjacent && rand() % 10 > 0) continue;
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
		if (_panel->symmetry && pos == get_sym_point(pos)) continue;
		if (_panel->symmetry && get_sym_point(pos).first != 0 && get_sym_point(pos).second != 0) continue;
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
	if (_panel->symmetry && (get_sym_point(pos) == pos) || (get(get_sym_point(pos)) & Decoration::Gap)) return false;
	if ((_panel->symmetry == Panel::Symmetry::ParallelH || _panel->symmetry == Panel::Symmetry::ParallelHFlip) && pos.second == _panel->_height / 2) return false;
	if ((_panel->symmetry == Panel::Symmetry::ParallelV || _panel->symmetry == Panel::Symmetry::ParallelVFlip) && pos.first == _panel->_width / 2) return false;
	if (_panel->symmetry == Panel::Symmetry::FlipNegXY && (pos.first + pos.second == _width - 1 || pos.first + pos.second == _width + 1)) return false;
	if (_panel->symmetry == Panel::Symmetry::FlipXY && (pos.first - pos.second == 1 || pos.first - pos.second == -1)) return false;
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
	if (_panel->symmetry) {
		if (get_sym_point(pos) == pos) return false;
		Panel::Symmetry backupSym = _panel->symmetry;
		_panel->symmetry = Panel::Symmetry::None; //To prevent endless recursion
		if (!can_place_dot(get_sym_point(pos))) {
			_panel->symmetry = backupSym;
			return false;
		}
		_panel->symmetry = backupSym;
	}
	if (_panel->symmetry == Panel::Symmetry::RotateLeft && _path1.count(pos) && _path2.count(pos))
		return false;
	if (hasFlag(Config::DisableDotIntersection)) return true;
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
			if (!off_edge(p) && (get(p) & DOT)) {
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
				if (get(x, y) == 0) continue;
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

	std::set<Point> open = (color == 0 ? _path : color == IntersectionFlags::DOT_IS_BLUE ? _path1 : _path2);
	for (Point p : _starts) open.erase(p);
	for (Point p : _exits) open.erase(p);
	if (intersectionOnly) {
		std::set<Point> intersections;
		for (Point p : open) {
			if (p.first % 2 == 0 && p.second % 2 == 0)
				intersections.insert(p);
		}
		open = intersections;
	}
	if (hasFlag(Config::DisableDotIntersection)) {
		std::set<Point> intersections;
		for (Point p : open) {
			if (p.first % 2 != 0 || p.second % 2 != 0)
				intersections.insert(p);
		}
		open = intersections;
	}

	while (amount > 0) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		open.erase(pos);
		if (!can_place_dot(pos)) continue;
		int symbol = (pos.first & 1) == 1 ? Decoration::Dot_Row : (pos.second & 1) == 1 ? Decoration::Dot_Column : Decoration::Dot_Intersection;
		set(pos, symbol | color);
		for (Point dir : _DIRECTIONS1) {
			open.erase(pos + dir);
		}
		if (_panel->symmetry) {
			Point sp = get_sym_point(pos);
			symbol = (sp.first & 1) == 1 ? Decoration::Dot_Row : (sp.second & 1) == 1 ? Decoration::Dot_Column : Decoration::Dot_Intersection;
			if (symbol != Decoration::Dot_Intersection) set(sp, symbol & ~Decoration::Dot);
			open.erase(sp);
			for (Point dir : _DIRECTIONS1) {
				open.erase(sp + dir);
			}
		}
		amount--;
	}
	return true;
}

bool Generate::can_place_stone(std::set<Point>& region, int color)
{
	for (Point p : region) {
		int sym = get(p);
		if (get_symbol_type(sym) == Decoration::Stone) return (sym & 0xf) == color;
	}
	return true;
}

bool Generate::place_stones(int color, int amount) {
	std::set<Point> open = _openpos;
	std::set<Point> open2;
	int passCount = 0;
	int originalAmount = amount;
	while (amount > 0) {
		if (open.size() == 0) {
			if (open2.size() < amount || _bisect && passCount < min(originalAmount, (_panel->_width / 2 + _panel->_height / 2 + 2) / 4))
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
		if (!can_place_stone(region, color)) {
			for (Point p : region) {
				open.erase(p);
			}
			continue;
		}
		if (_stoneTypes > 2) {
			open.clear();
			for (Point p : region) {
				if (_openpos.count(p))
					open.insert(p);
			}
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

int Generate::make_shape_symbol(Shape shape, bool rotated, bool negative, int rotation, int depth)
{
	int symbol = static_cast<int>(Decoration::Poly);
	if (rotated) {
		if (rotation == -1) {
			if (make_shape_symbol(shape, rotated, negative, 0, depth + 1) == make_shape_symbol(shape, rotated, negative, 1, depth + 1))
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
	if (xmax - xmin > 6 || ymax - ymin > 6) { //Shapes cannot be more than 4 in width and height
		if (Point::pillarWidth == 0 || ymax - ymin > 6 || depth > Point::pillarWidth / 2) return 0;
		Shape newShape;
		for (Point p : shape) newShape.insert({ (p.first - xmax + Point::pillarWidth) % Point::pillarWidth, p.second });
		return make_shape_symbol(newShape, rotated, negative, rotation, depth + 1);
	}
	for (Point p : shape) {
		symbol |= (1 << ((p.first - xmin) / 2 + (ymax  - p.second) * 2)) << 16;
	}
	if (rand() % 4 > 0) {
		int type = symbol >> 16; //The generator makes the below type of symbol way too often. (2x2 square with another square attached)
		if (type == 0x0331 || type == 0x0332 || type == 0x0037 || type == 0x0067 || type == 0x0133 || type == 0x0233 || type == 0x0073 || type == 0x0076)
			return 0;
	}
	return symbol;
}

bool Generate::place_shapes(std::vector<int> colors, std::vector<int> negativeColors, int amount, int numRotated, int numNegative)
{
	std::set<Point> open = _openpos;
	int shapeSize = hasFlag(Config::SmallShapes) ? 2 : hasFlag(Config::BigShapes) ? 5 : 4;
	int targetArea = amount * shapeSize * 7 / 8;
	if (hasFlag(Generate::Config::MountainFloorH) && _panel->_width == 9) {
		targetArea = 0;
		removeFlag(Generate::Config::MountainFloorH);
	}
	int totalArea = 0;
	int colorIndex = rand() % colors.size();
	int colorIndexN = rand() % (negativeColors.size() + 1);
	bool shapesCanceled = false, shapesCombined = false;
	if (amount == 1) shapesCombined = true;
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
		if (region.size() <= shapeSize + 1 && bufferRegion.size() == 0 && rand() % 2 == 1) numShapes = 1;
		if (hasFlag(Config::MountainFloorH)) {
			if (region.size() < 20) continue;
			numShapes = 6;
		}
		if (hasFlag(Config::SplitShapes) && numShapes != 1) continue;
		if (hasFlag(Config::RequireCombineShapes) && numShapes == 1) continue;
		bool balance = false;
		if (numShapes > amount) {
			if (numNegative < 2 || hasFlag(Config::DisableCancelShapes)) continue;
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
				Shape shape = generate_shape(regionN, pick_random(regionN), min(shapeSize + 1, numShapes * 2 / numShapesN + rand() % 3 - 1));
				shapesN.push_back(shape);
				for (Point p : shape) {
					region.insert(p);
				}
			}
			shapesCanceled = true;
			//Let the rest of the algorithm create the cancelling shapes
		}
		if (!balance && numShapesN && (numShapesN + numShapes >= 7 || numShapesN > 1 && numRotated > 0))
			continue; //Trying to prevent the game's shape calculator from lagging too much
		if (!(hasFlag(Config::MountainFloorH) && _panel->_width == 11) && open2.size() < numShapes + numShapesN) continue;
		if (numShapes == 1) {
			shapes.push_back(region);
			region.clear();
		}
		else for (; numShapes > 0; numShapes--) {
			if (region.size() == 0) break;
			shapes.push_back(generate_shape(region, bufferRegion, pick_random(region), balance ? rand() % 3 + 1 : shapeSize));
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
		if (numShapes > 1) shapesCombined = true;
		numNegative -= static_cast<int>(shapesN.size());
		if (hasFlag(Generate::Config::MountainFloorH) && amount == 6) {
			if (!combine_shapes(shapes) || !combine_shapes(shapes))
				return false;
			amount -= 2;
		}
		for (Shape& shape : shapes) {
			int symbol = make_shape_symbol(shape, (numRotated-- > 0), (numShapes-- <= 0));
			if (symbol == 0)
				return false;
			//Attempt not to put shapes adjacent
			Point pos;
			for (int i = 0; i < 10; i++) {
				if (open2.size() == 0) return false;
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

int Generate::count_color(std::set<Point>& region, int color)
{
	int count = 0;
	for (Point p : region) {
		int sym = get(p);
		if (sym && (sym & 0xf) == color)
			if (count++ == 2) return count;
	}
	return count;
}

bool Generate::place_stars(int color, int amount)
{
	std::set<Point> open = _openpos;
	while (amount > 0) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		std::set<Point> region = get_region(pos);
		std::set<Point> open2;
		for (Point p : region) {
			if (open.erase(p)) open2.insert(p);
		}
		int count = count_color(region, color);
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

bool Generate::has_star(std::set<Point>& region, int color)
{
	for (Point p : region) {
		if (get(p) == (Decoration::Star | color)) return true;
	}
	return false;
}

bool Generate::place_triangles(int color, int amount, int targetCount)
{
	std::set<Point> open = _openpos;
	while (amount > 0) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		int count = count_sides(pos);
		open.erase(pos);
		if (_panel->symmetry) {
			open.erase(get_sym_point(pos));
		}
		if (count == 0 || targetCount && count != targetCount) continue;
		if (hasFlag(Config::TreehouseLayout)) {
			bool found = false;
			for (Point dir : _DIRECTIONS1) {
				if (_starts.count(pos + dir) || _exits.count(pos + dir)) {
					found = true;
					break;
				}
			}
			if (found) continue;
		}
		if (!targetCount && count == 2 && rand() % 2 == 0) //Prevent it from placing so many 2's
			continue;
		set(pos, Decoration::Triangle | color | (count << 16));
		_openpos.erase(pos);
		amount--;
	}
	return true;
}

int Generate::count_sides(Point pos)
{
	int count = 0;
	for (Point dir : _DIRECTIONS1) {
		Point p = pos + dir;
		if (!off_edge(p) && get(p) == PATH) {
			count++;
		}
	}
	return count;
}

bool Generate::place_arrows(int color, int amount, int targetCount)
{
	std::set<Point> open = _openpos;
	while (amount > 0) {
		if (open.size() == 0)
			return false;
		Point pos = pick_random(open);
		open.erase(pos);
		if (pos.first == _panel->_width / 2 || Point::pillarWidth > 0 && pos.first == _panel->_width / 2 - 1)
			continue; //Because of a glitch where arrows in the center column won't draw right
		int fails = 0;
		while (fails++ < 20) {
			int choice = rand() % 8;
			Point dir = _8DIRECTIONS2[choice];
			if (Point::pillarWidth > 0 && dir.second == 0) continue;
			int count = count_crossings(pos, dir);
			if (count == 0 || count > 3 || targetCount && count != targetCount) continue;
			if (dir.first < 0 && count == (pos.first + 1) / 2 || dir.first > 0 && count == (_panel->_width - pos.first) / 2 ||
				dir.second < 0 && count == (pos.second + 1) / 2 || dir.second > 0 && count == (_panel->_height - pos.second) / 2 && rand() % 10 > 0)
				continue;
			set(pos, Decoration::Arrow | color | (count << 12) | (choice << 16));
			_openpos.erase(pos);
			amount--;
			break;
		}
	}
	return true;
}

int Generate::count_crossings(Point pos, Point dir)
{
	pos = pos + dir / 2;
	int count = 0;
	while (!off_edge(pos)) {
		if (get(pos) == PATH) count++;
		pos = pos + dir;
	}
	return count;
}

bool Generate::place_erasers(std::vector<int> colors, std::vector<int> eraseSymbols)
{
	std::set<Point> open = _openpos;
	if (_panel->id == 0x288FC && hasFlag(Generate::Config::DisableWrite)) open.erase({ 5, 5 });
	int amount = static_cast<int>(colors.size());
	while (amount > 0) {
		if (open.size() == 0)
			return false;
		int toErase = hasFlag(Config::CombineErasers) ? eraseSymbols[0] : eraseSymbols[amount - 1];
		int color = colors[amount - 1];
		Point pos = pick_random(open);
		std::set<Point> region = get_region(pos);
		std::set<Point> open2;
		for (Point p : region) {
			if (open.erase(p)) open2.insert(p);
		}
		if (_splitPoints.size() > 0) {
			bool found = false;
			for (Point p : _splitPoints) {
				if (region.count(p)) {
					found = true;
					break;
				}
			}
			if (!found) continue;
		}
		if (_panel->id == 0x288FC && hasFlag(Generate::Config::DisableWrite) && !region.count({ 5, 5 })) continue;
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
		
		if ((open2.size() == 0 || _splitPoints.size() == 0 && open2.size() == 1 || hasFlag(Generate::CombineErasers) && open2.size() < colors.size() + 1) && !(toErase & Decoration::Dot)) continue;
		bool canPlace = false;
		if (get_symbol_type(toErase) == Decoration::Stone) {
			canPlace = !can_place_stone(region, (toErase & 0xf));
		}
		else if (get_symbol_type(toErase) == Decoration::Star) {
			canPlace = (count_color(region, (toErase & 0xf)) + (color == (toErase & 0xf) ? 1 : 0) != 1);
		}
		else canPlace = true;
		if (!canPlace) continue;

		if (get_symbol_type(toErase) == Decoration::Stone || get_symbol_type(toErase) == Decoration::Star) {
			set(pos, toErase);
		}
		else if (toErase & Decoration::Dot) {
			std::set<Point> openEdge;
			for (Point p : region) {
				for (Point dir : _8DIRECTIONS1) {
					if (toErase == Decoration::Dot_Intersection && (dir.first == 0 || dir.second == 0)) continue;
					Point p2 = p + dir;
					if (get(p2) == 0 && (hasFlag(Config::FalseParity) || can_place_dot(p2))) {
						openEdge.insert(p2);
					}
				}
			}
			if (openEdge.size() == 0)
				continue;
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
				int shapeSize;
				if ((toErase & Decoration::Negative) || hasFlag(Config::SmallShapes)) shapeSize = rand() % 3 + 1;
				else {
					shapeSize = rand() % 5 + 1;
					if (shapeSize < 3)
						shapeSize += rand() % 3;
				}
				Shape shape = generate_shape(area, pick_random(area), shapeSize);
				if (shape.size() == region.size()) continue;
				symbol = make_shape_symbol(shape, toErase & Decoration::Can_Rotate, toErase & Decoration::Negative);
			}
			set(pos, symbol | (toErase & 0xf));
		}
		else if (get_symbol_type(toErase) == Decoration::Triangle) {
			int count = count_sides(pos);
			if (count == 0) count = rand() % 3 + 1;
			else count = (count + (rand() & 1)) % 3 + 1;
			set(pos, toErase | (count << 16));
		}

		if (!(toErase & Decoration::Dot)) {
			_openpos.erase(pos);
			open2.erase(pos);
		}
		
		if (hasFlag(Config::CombineErasers)) {
			while (amount > 0) {
				pos = pick_random(open2);
				set(pos, Decoration::Eraser | colors[amount - 1]);
				_openpos.erase(pos);
				open2.erase(pos);
				amount--;
			}
		}
		else {
			if (_splitPoints.size() == 0) pos = pick_random(open2);
			else for (Point p : _splitPoints) if (region.count(p)) { pos = p; break; }
			if (_panel->id == 0x288FC && hasFlag(Generate::Config::DisableWrite)) pos = { 5, 5 };
			set(pos, Decoration::Eraser | color);
			_openpos.erase(pos);
			amount--;
		}
	}
	return true;
}

bool Generate::combine_shapes(std::vector<Shape>& shapes)
{
	for (int i = 0; i < shapes.size(); i++) {
		for (int j = 0; j < shapes.size(); j++) {
			if (i == j) continue;
			if (shapes[i].size() + shapes[j].size() <= 5) continue;
			if (shapes[i].size() > 5 || shapes[j].size() > 5) continue;
			//Look for adjacent points
			for (Point p1 : shapes[i]) {
				for (Point p2 : shapes[j]) {
					for (Point dir : _DIRECTIONS2) {
						if (p1 + dir == p2) {
							//Combine shapes
							for (Point p : shapes[i]) shapes[j].insert(p);
							//Make sure there are no holes
							std::set<Point> area = _gridpos;
							for (Point p : shapes[j]) area.erase(p);
							while (area.size() > 0) {
								std::set<Point> region = get_region(*area.begin());
								bool connected = false;
								for (Point p : region) {
									if (p.first == 1 || p.second == 1 || p.first == _panel->_width - 2 || p.second == _panel->_height - 2) {
										connected = true;
										break;
									}
								}
								if (!connected) return false;
								for (Point p : region) area.erase(p);
							}
							shapes.erase(shapes.begin() + i);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}
