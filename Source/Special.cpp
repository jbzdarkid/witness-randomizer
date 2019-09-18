#include "Special.h"

void Special::generateReflectionDotPuzzle(int id1, int id2, std::vector<std::pair<int, int>> symbols, Panel::Symmetry symmetry)
{
	_generator->setFlagOnce(Generate::Config::DisableWrite);
	_generator->generate(id1, symbols);
	std::shared_ptr<Panel> puzzle = _generator->_panel;
	std::shared_ptr<Panel> flippedPuzzle = std::make_shared<Panel>(id2);
	for (int x = 0; x < puzzle->_width; x++) {
		for (int y = 0; y < puzzle->_height; y++) {
			Point sp = puzzle->get_sym_point(x, y, symmetry);
			if (puzzle->_grid[x][y] & Decoration::Dot) {
				int symbol = (sp.first & 1) == 1 ? Decoration::Dot_Row : (sp.second & 1) == 1 ? Decoration::Dot_Column : Decoration::Dot_Intersection;
				flippedPuzzle->_grid[sp.first][sp.second] = symbol | IntersectionFlags::DOT_IS_INVISIBLE;
			}
			else if (puzzle->_grid[x][y] & Decoration::Gap) {
				int symbol = (sp.first & 1) == 1 ? Decoration::Gap_Row : Decoration::Gap_Column;
				flippedPuzzle->_grid[sp.first][sp.second] = symbol;
			}
			else flippedPuzzle->_grid[sp.first][sp.second] = puzzle->_grid[x][y];
		}
	}
	flippedPuzzle->_startpoints.clear();
	for (Point p : puzzle->_startpoints) {
		flippedPuzzle->_startpoints.push_back(puzzle->get_sym_point(p, symmetry));
	}
	flippedPuzzle->_endpoints.clear();
	for (Endpoint p : puzzle->_endpoints) {
		Point sp = puzzle->get_sym_point(p.GetX(), p.GetY(), symmetry);
		flippedPuzzle->_endpoints.push_back(Endpoint(sp.first, sp.second, get_sym_dir(p.GetDir(), symmetry),
			IntersectionFlags::ENDPOINT | (p.GetDir() == Endpoint::Direction::UP || p.GetDir() == Endpoint::Direction::DOWN ? IntersectionFlags::COLUMN : IntersectionFlags::ROW)));
	}
	_generator->write(id1);
	flippedPuzzle->Write(id2);
}

void Special::generateAntiPuzzle(int id)
{
	while (true) {
		_generator->setFlagOnce(Generate::Config::DisableWrite);
		_generator->generate(id, Decoration::Poly | Decoration::Can_Rotate, 2);
		std::set<Point> open = _generator->_gridpos;
		std::vector<int> symbols;
		for (int x = 1; x < _generator->_panel->_width; x += 2) {
			for (int y = 1; y < _generator->_panel->_height; y += 2) {
				if (_generator->get_symbol_type(_generator->get(x, y)) == Decoration::Poly) {
					symbols.push_back(_generator->get(x, y));
					_generator->set(x, y, 0);
					for (Point p : _generator->get_region(Point(x, y))) {
						open.erase(p);
					}
				}
			}
		}
		if (open.size() == 0) continue;
		std::set<Point> region = _generator->get_region(*open.begin());
		if (region.size() != open.size() || open.size() < symbols.size() + 1) continue;
		for (int s : symbols) {
			Point p = _generator->pick_random(open);
			_generator->set(p, s | Decoration::Negative);
			open.erase(p);
		}
		_generator->set(_generator->pick_random(open), Decoration::Poly | 0xffff0000); //Full block
		_generator->write(id);
		_generator->resetConfig();
		return;
	}
}

void Special::generateColorFilterPuzzle(int id, std::vector<std::pair<int, int>> symbols, Color filter)
{
	_generator->setFlagOnce(Generate::Config::DisableWrite);
	_generator->generate(id, symbols);
	std::vector<Color> availableColors = { {0, 0, 0, 1} };
	if (filter.r == 1) {
		for (int i = 0; i < availableColors.size(); i++) {
			Color c = availableColors[i];
			if (c.r == 0) availableColors.push_back({ 1, c.g, c.b, 1 });
		}
	}
	if (filter.g == 1) {
		for (int i = 0; i < availableColors.size(); i++) {
			Color c = availableColors[i];
			if (c.g == 0) availableColors.push_back({ c.r, 1, c.b, 1 });
		}
	}
	if (filter.b == 1) {
		for (int i = 0; i < availableColors.size(); i++) {
			Color c = availableColors[i];
			if (c.b == 0) availableColors.push_back({ c.r, c.g, 1, 1 });
		}
	}
	for (int i = 0; i < availableColors.size(); i++) { //Shuffle
		std::swap(availableColors[i], availableColors[rand() % availableColors.size()]);
	}
	std::vector<Color> symbolColors;
	for (int y = _generator->_panel->_height - 2; y>0; y -= 2) {
		for (int x = 1; x < _generator->_panel->_width - 1; x += 2) {
			if (_generator->get(x, y) == 0) symbolColors.push_back({ 0, 0, 0, 0 });
			else symbolColors.push_back(availableColors[_generator->get(x, y) & 0xf]);
		}
	}
	bool pass = false;
	while (!pass) {
		//Add random variation in remaining color channel(s)
		for (Color &c : symbolColors) {
			if (c.a == 0) continue;
			if (filter.r == 0) c.r = static_cast<float>(rand() % 2);
			if (filter.g == 0) c.g = static_cast<float>(rand() % 2);
			if (filter.b == 0) c.b = static_cast<float>(rand() % 2);
		}
		//Check for solvability
		std::map<Color, int> colorCounts;
		for (Color &c : symbolColors) {
			colorCounts[c]++;
		}
		for (auto pair : colorCounts) {
			if (pair.second % 2) {
				pass = true;
				break;
			}
		}
	}
	
	_generator->_panel->_memory->WriteArray<Color>(id, DECORATION_COLORS, symbolColors);
	_generator->write(id);
}

void Special::generateSoundDotPuzzle(int id, std::vector<int> dotSequence, bool writeSequence) {
	_generator->setFlagOnce(Generate::Config::DisableWrite);
	_generator->setFlagOnce(Generate::Config::BackupPath);
	_generator->setFlagOnce(Generate::Config::LongPath);
	if (id == 0x014B2) { //Have to force there to only be one correct sequence
		_generator->generate(id, Decoration::Dot_Intersection, static_cast<int>(dotSequence.size() - 1));
		while (_generator->get(6, 0) == Decoration::Dot_Intersection || _generator->get(8, 2) == Decoration::Dot_Intersection)
			_generator->generate(id, Decoration::Dot_Intersection, static_cast<int>(dotSequence.size() - 1));
		if (_generator->get(7, 0) == PATH) _generator->set(7, 0, Decoration::Dot_Row);
		else _generator->set(8, 1, Decoration::Dot_Column);
	}
	else _generator->generate(id, Decoration::Dot_Intersection, static_cast<int>(dotSequence.size()));
	Point p = *_generator->_starts.begin();
	std::set<Point> path = _generator->_path;
	int seqPos = 0;
	while (!_generator->_exits.count(p)) {
		path.erase(p);
		int sym = _generator->get(p);
		if (sym & Decoration::Dot) {
			_generator->set(p, sym | dotSequence[seqPos++]);
		}
		for (Point dir : Generate::_DIRECTIONS1) {
			Point newp = Point(p.first + dir.first, p.second + dir.second);
			if (path.count(newp)) {
				p = newp;
				break;
			}
		}
	}
	if (writeSequence) {
		for (int i = 0; i < dotSequence.size(); i++) {
			if (dotSequence[i] == DOT_SMALL) dotSequence[i] = 1;
			if (dotSequence[i] == DOT_MEDIUM) dotSequence[i] = 2;
			if (dotSequence[i] == DOT_LARGE) dotSequence[i] = 3;
		}
		_generator->_panel->_memory->WritePanelData<size_t>(id, DOT_SEQUENCE_LEN, { dotSequence.size() });
		_generator->_panel->_memory->WriteArray<int>(id, DOT_SEQUENCE, dotSequence, true);
	}
	_generator->write(id);
}

void Special::generateSoundDotReflectionPuzzle(int id, std::vector<int> dotSequence1, std::vector<int> dotSequence2, int numColored, bool writeSequence)
{
	_generator->setFlagOnce(Generate::Config::DisableWrite);
	_generator->setFlagOnce(Generate::Config::BackupPath);
	_generator->setFlagOnce(Generate::Config::LongPath);
	_generator->setSymmetry(Panel::Symmetry::Rotational);
	_generator->generate(id, Decoration::Dot_Intersection | Decoration::Color::Blue, static_cast<int>(dotSequence1.size()), Decoration::Dot_Intersection | Decoration::Color::Yellow, static_cast<int>(dotSequence2.size()));
	std::set<Point> path1 = _generator->_path1, path2 = _generator->_path2;
	Point p1, p2;
	std::set<Point> dots1, dots2;
	for (Point p : _generator->_starts) {
		if (_generator->_path1.count(p)) p1 = p;
		if (_generator->_path2.count(p)) p2 = p;
	}
	int seqPos = 0;
	while (!_generator->_exits.count(p1)) {
		path1.erase(p1);
		int sym = _generator->get(p1);
		if (sym & Decoration::Dot) {
			_generator->set(p1, sym | dotSequence1[seqPos++]);
			dots1.insert(p1);
		}
		for (Point dir : Generate::_DIRECTIONS1) {
			Point newp = Point(p1.first + dir.first, p1.second + dir.second);
			if (path1.count(newp)) {
				p1 = newp;
				break;
			}
		}
	}
	seqPos = 0;
	while (!_generator->_exits.count(p2)) {
		path2.erase(p2);
		int sym = _generator->get(p2);
		if (sym & Decoration::Dot) {
			_generator->set(p2, sym | dotSequence2[seqPos++]);
			dots2.insert(p2);
		}
		for (Point dir : Generate::_DIRECTIONS1) {
			Point newp = Point(p2.first + dir.first, p2.second + dir.second);
			if (path2.count(newp)) {
				p2 = newp;
				break;
			}
		}
	}
	for (int i = static_cast<int>(dotSequence1.size() + dotSequence2.size()); i > numColored; i--) {
		if (i % 2 == 0) { //Want to evenly distribute colors between blue/orange (approximately)
			Point p = pop_random(dots1);
			_generator->set(p, _generator->get(p) & ~DOT_IS_BLUE); //Remove color
		}
		else {
			Point p = pop_random(dots2);
			_generator->set(p, _generator->get(p) & ~DOT_IS_ORANGE); //Remove color
		}
	}
	if (writeSequence) {
		for (int i = 0; i < dotSequence1.size(); i++) {
			if (dotSequence1[i] == DOT_SMALL) dotSequence1[i] = 1;
			if (dotSequence1[i] == DOT_MEDIUM) dotSequence1[i] = 2;
			if (dotSequence1[i] == DOT_LARGE) dotSequence1[i] = 3;
		}
		_generator->_panel->_memory->WritePanelData<size_t>(id, DOT_SEQUENCE_LEN, { dotSequence1.size() });
		_generator->_panel->_memory->WriteArray<int>(id, DOT_SEQUENCE, dotSequence1, true);
		for (int i = 0; i < dotSequence2.size(); i++) {
			if (dotSequence2[i] == DOT_SMALL) dotSequence2[i] = 1;
			if (dotSequence2[i] == DOT_MEDIUM) dotSequence2[i] = 2;
			if (dotSequence2[i] == DOT_LARGE) dotSequence2[i] = 3;
		}
		_generator->_panel->_memory->WritePanelData<size_t>(id, DOT_SEQUENCE_LEN_REFLECTION, { dotSequence2.size() });
		_generator->_panel->_memory->WriteArray<int>(id, DOT_SEQUENCE_REFLECTION, dotSequence2, true);
	}
	_generator->write(id);
	_generator->setSymmetry(Panel::Symmetry::None);
}

void Special::generateRGBStonePuzzleN(int id)
{
	while (true) {
		_generator->setFlagOnce(Generate::Config::DisableWrite);
		_generator->generate(id);
		int amount = 16;
		std::set<Decoration::Color> used;
		std::vector<Decoration::Color> colors = { Decoration::Black, Decoration::White, Decoration::Red, Decoration::Green, Decoration::Blue, Decoration::Cyan, Decoration::Yellow, Decoration::Magenta };
		while (amount > 0) {
			Decoration::Color c = _generator->pick_random(colors);
			if (_generator->place_stones(c, 1)) {
				amount--;
				used.insert(c);
			}
		}
		if (used.size() < 5) continue;
		_generator->setFlagOnce(Generate::Config::WriteColors);
		_generator->write(id);
		return;
	}
}

void Special::generateRGBStarPuzzleN(int id)
{
	while (true) {
		_generator->setFlagOnce(Generate::Config::DisableWrite);
		_generator->generate(id);
		int amount = 16;
		std::set<Decoration::Color> used;
		std::set<Decoration::Color> colors = { Decoration::Black, Decoration::White, Decoration::Red, Decoration::Green, Decoration::Blue, Decoration::Cyan, Decoration::Yellow, Decoration::Magenta };
		while (amount > 0) {
			Decoration::Color c = _generator->pick_random(colors);
			if (_generator->place_stars(c, 2)) {
				amount -= 2;
				used.insert(c);
				if (used.size() == 4) colors = used;
			}
		}
		if (used.size() < 4)
			continue;
		_generator->setFlagOnce(Generate::Config::WriteColors);
		_generator->write(id);
		return;
	}
}

void Special::generateJungleVault(int id)
{
	//This panel won't render symbols off the grid, so all I can do is move the dots around

	//a: 4 9 16 23, b: 12, c: 1, ab: 3 5 6 10 11 15 17 18 20 21 22, ac: 14, bc: 2 19, all: 7 8 13
	std::vector<std::vector<int>> sols = {
		{ 0, 5, 10, 15, 20, 21, 16, 11, 6, 7, 8, 3, 4, 9, 14, 13, 18, 17, 22, 23, 24, 25 },
		{ 0, 5, 10, 15, 20, 21, 22, 17, 12, 11, 6, 7, 2, 3, 8, 13, 18, 19, 24, 25 },
		{ 0, 1, 2, 7, 8, 13, 14, 19, 24, 25 } };
	//std::vector<int> invalidSol = { 0, 5, 10, 15, 16, 11, 12, 13, 8, 3, 4, 9, 14, 19, 18, 23, 24, 25 };
	std::vector<std::vector<int>> dotPoints1 = { { 4, 9, 16, 23 }, { 2, 19 }, { 2, 19 } };
	std::vector<std::vector<int>> dotPoints2 = { { 7, 8, 13 }, { 3, 5, 6, 10, 11, 15, 17, 18, 20, 21, 22 }, { 14, 1 } };
	_generator->initPanel(id);
	_generator->clear();
	int sol = rand() % sols.size();
	auto[x1, y1] = _generator->_panel->loc_to_xy(_generator->pick_random(dotPoints1[sol]));
	auto[x2, y2] = _generator->_panel->loc_to_xy(_generator->pick_random(dotPoints2[sol]));
	_generator->set(x1, y1, Decoration::Dot_Intersection);
	_generator->set(x2, y2, Decoration::Dot_Intersection);
	_generator->_panel->_memory->WritePanelData<size_t>(id, SEQUENCE_LEN, { sols[sol].size() });
	_generator->_panel->_memory->WriteArray<int>(id, SEQUENCE, sols[sol], true);
	_generator->write(id);
}

void Special::generateApplePuzzle(int id, bool changeExit, bool flip)
{
	//Is there a way to move the apples? Might be impossible without OpenGL stuff.
	std::shared_ptr<Panel> panel = std::make_shared<Panel>();
	int numIntersections = panel->_memory->ReadPanelData<int>(id, NUM_DOTS);
	std::vector<int> intersectionFlags = panel->_memory->ReadArray<int>(id, DOT_FLAGS, numIntersections);
	std::vector<int> sequence = panel->_memory->ReadArray<int>(id, SEQUENCE, 6);
	int exit = sequence[5];
	std::vector<int> exits;
	if (changeExit) {
		for (int i = 15; i < 31; i++) {
			if (intersectionFlags[i] == 9) exits.push_back(i);
		}
		int newExit = pop_random(exits);
		intersectionFlags[newExit] = 1;
		intersectionFlags[exit] = 9; //Gets rid of old exit
		for (int i = 5; i > 1; i--) {
			sequence[i] = newExit;
			newExit = (newExit - 1) / 2;
		}
		int numConnections = panel->_memory->ReadPanelData<int>(id, NUM_CONNECTIONS);
		std::vector<int> connections_a = panel->_memory->ReadArray<int>(id, DOT_CONNECTION_A, numConnections);
		std::vector<int> connections_b = panel->_memory->ReadArray<int>(id, DOT_CONNECTION_B, numConnections);
		for (int i = 0; i < numConnections; i++) {
			if (connections_b[i] == exit) {
				connections_a[i] = sequence[4];
				connections_b[i] = sequence[5];
			}
		}
		panel->_memory->WriteArray<int>(id, DOT_FLAGS, intersectionFlags);
		panel->_memory->WriteArray<int>(id, SEQUENCE, sequence, true);
		panel->_memory->WriteArray<int>(id, DOT_CONNECTION_A, connections_a);
		panel->_memory->WriteArray<int>(id, DOT_CONNECTION_B, connections_b);
		panel->_memory->WritePanelData<int>(id, NEEDS_REDRAW, { 1 });
	}
	if (flip) {
		std::vector<float> intersections = panel->_memory->ReadArray<float>(id, DOT_POSITIONS, numIntersections * 2);
		for (int i = 0; i < intersections.size(); i += 2) {
			intersections[i] = 1 - intersections[i];
		}
		panel->_memory->WriteArray<float>(id, DOT_POSITIONS, intersections);
		panel->_memory->WritePanelData<int>(id, NEEDS_REDRAW, { 1 });
	}
}

void Special::generateKeepLaserPuzzle(int id, std::set<Point> path1, std::set<Point> path2, std::set<Point> path3, std::set<Point> path4, std::vector<std::pair<int, int>> symbols)
{
	_generator->resetConfig();
	_generator->setGridSize(10, 11);
	_generator->_starts = { { 0, 2 },{ 20, 10 },{ 8, 14 },{ 0, 22 },{ 20, 22 } };
	_generator->_exits = { { 0, 18 },{ 10, 22 },{ 0, 0 },{ 20, 0 },{ 0, 12 } };
	_generator->initPanel(id);
	_generator->clear();
	std::vector<Point> gaps = { { 17, 20 },{ 16, 19 },{ 20, 17 },{ 15, 14 },{ 15, 16 },{ 18, 13 },{ 20, 13 }, //Yellow Puzzle Walls
	{ 10, 11 },{ 12, 11 },{ 14, 11 },{ 16, 11 },{ 18, 11 },{ 11, 0 },{ 11, 2 },{ 11, 4 },{ 11, 8 },{ 11, 10 },{ 14, 1 },{ 16, 1 },{ 18, 1 },{ 20, 1 }, //Pink Puzzle Walls
	{ 2, 1 },{ 4, 1 },{ 6, 1 },{ 8, 1 },{ 0, 11 },{ 2, 11 },{ 4, 11 },{ 6, 11 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 9, 8 },{ 9, 10 },{ 9, 12 }, //Green Puzzle Walls
	{ 0, 13 },{ 2, 13 },{ 4, 13 },{ 6, 13 },{ 9, 14 },{ 9, 16 },{ 9, 20 },{ 9, 22 }, //Blue Puzzle Walls
	};
	std::vector<Point> pathPoints = { { 14, 13 },{ 14, 12 },{ 15, 12 },{ 16, 12 },{ 17, 12 },{ 18, 12 },{ 19, 12 },{ 20, 12 },{ 20, 11 },
	{ 11, 6 },{ 10, 6 },{ 10, 5 },{ 10, 4 },{ 10, 3 },{ 10, 2 },{ 10, 1 },{ 10, 0 },{ 9, 0 },{ 8, 0 },{ 7, 0 },{ 6, 0 },{ 5, 0 },{ 4, 0 },{ 3, 0 },
	{ 2, 0 },{ 1, 0 },{ 0, 0 },{ 0, 1 },{ 8, 11 },{ 8, 12 },{ 8, 13 } };
	std::vector<Point> pathPoints2 = { { 9, 18 },{ 10, 18 },{ 10, 19 },{ 10, 20 },{ 10, 21 },{ 10, 22 } }; //For exiting out the right side of the last puzzle
	for (Point p : path1) _generator->set_path(Point(p.first + 12, p.second + 14));
	for (Point p : path2) _generator->set_path(Point(p.first + 12, p.second + 2));
	for (Point p : path3) _generator->set_path(Point(8 - p.first, 8 - p.second + 2));
	for (Point p : path4) _generator->set_path(Point(p.first, p.second + 14));
	for (Point p : pathPoints) _generator->set_path(p);
	if (path4.count(Point({ 8, 4 }))) for (Point p : pathPoints2) _generator->set_path(p);
	for (Point p : gaps) _generator->set(p, p.first % 2 == 0 ? Decoration::Gap_Column : Decoration::Gap_Row);

	Generate::PuzzleSymbols psymbols(symbols);

	while (!_generator->place_all_symbols(psymbols)) {
		for (int x = 0; x < _generator->_panel->_width; x++)
			for (int y = 0; y < _generator->_panel->_height; y++)
				if (_generator->get(x, y) != PATH && (_generator->get(x, y) & 0x1fffff) != Decoration::Gap)
					_generator->set(x, y, 0);
		_generator->_openpos = _generator->_gridpos;
	}

	_generator->write(id);
}

void Special::generateMountaintop(int id)
{
	/*std::vector<std::vector<Point>> perspectiveU = {
	{ { 1, 2 },{ 1, 4 },{ 3, 0 },{ 3, 2 },{ 7, 0 },{ 7, 2 },{ 7, 4 },{ 8, 1 },{ 8, 3 },{ 8, 5 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } },
	{ { 0, 3 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{ 3, 2 },{ 6, 1 },{ 7, 0 },{ 7, 2 },{ 7, 4 },{ 8, 1 },{ 8, 3 },{ 8, 5 },{ 9, 2 },{ 9, 4 },{ 9, 6 } },
	{ { 0, 3 },{ 0, 5 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{ 3, 2 },{ 3, 4 },{ 4, 3 },{ 4, 5 },{ 5, 4 },{ 6, 3 },{ 6, 5 },{ 7, 4 },{ 8, 5 },{ 9, 6 } },
	{ { 0, 3 },{ 0, 5 },{ 0, 7 },{ 1, 2 },{ 1, 4 },{ 1, 6 },{ 1, 8 },{ 2, 3 },{ 2, 5 },{ 2, 7 },{ 3, 4 },{ 3, 6 },{ 4, 5 },{ 4, 7 },{ 5, 6 },{ 6, 5 },{ 6, 7 },{ 7, 4 },{ 7, 6 },{ 8, 7 } },
	{ { 0, 7 },{ 1, 6 },{ 1, 8 },{ 2, 5 },{ 3, 4 },{ 7, 4 },{ 7, 6 },{ 7, 8 },{ 8, 5 },{ 8, 7 },{ 8, 9 },{ 9, 6 },{ 9, 8 },{ 10, 7 } },
	{ { 0, 7 },{ 1, 6 },{ 1, 8 },{ 2, 5 },{ 2, 7 },{ 2, 9 },{ 3, 4 },{ 3, 6 },{ 3, 8 },{ 3, 10 },{ 7, 4 },{ 7, 6 },{ 8, 5 },{ 8, 7 },{ 9, 6 },{ 9, 8 },{ 10, 7 } },
	{ { 2, 5 },{ 4, 3 },{ 4, 5 },{ 6, 3 },{ 6, 5 },{ 7, 4 },{ 8, 3 },{ 8, 5 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } },
	{ { 1, 4 },{ 2, 3 },{ 2, 5 },{ 3, 4 },{ 4, 3 },{ 5, 2 },{ 6, 1 },{ 6, 3 },{ 7, 0 },{ 7, 2 },{ 7, 4 },{ 8, 1 },{ 8, 3 },{ 8, 5 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } },
	};
	std::vector<std::vector<Point>> perspectiveL = {
	{ { 0, 3 },{ 0, 5 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{ 3, 2 },{ 5, 2 },{ 6, 1 },{ 6, 3 },{ 7, 0 },{ 7, 2 },{ 7, 4 },{ 8, 3 },{ 8, 5 },{ 9, 4 },{ 9, 6 } },
	{ { 0, 3 },{ 0, 5 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{ 3, 0 },{ 3, 2 },{ 4, 1 },{ 4, 3 },{ 5, 2 },{ 5, 4 },{ 6, 3 },{ 6, 5 },{ 7, 4 },{ 8, 5 },{ 9, 6 } },
	{ { 0, 3 },{ 0, 5 },{ 0, 7 },{ 1, 2 },{ 1, 4 },{ 1, 6 },{ 1, 8 },{ 2, 3 },{ 2, 5 },{ 2, 7 },{ 3, 4 },{ 3, 6 },{ 4, 5 },{ 4, 7 },{ 5, 6 },{ 6, 5 },{ 6, 7 },{ 7, 4 },{ 7, 6 },{ 8, 7 } },
	{ { 0, 7 },{ 1, 6 },{ 2, 5 },{ 3, 4 },{ 6, 9 },{ 7, 4 },{ 7, 6 },{ 7, 8 },{ 7, 10 },{ 8, 7 },{ 8, 9 },{ 9, 8 } },
	{ { 0, 7 },{ 1, 6 },{ 1, 8 },{ 2, 5 },{ 3, 4 },{ 7, 4 },{ 7, 6 },{ 7, 8 },{ 8, 5 },{ 8, 7 },{ 8, 9 },{ 9, 6 },{ 9, 8 },{ 10, 7 } },
	{ { 2, 5 },{ 4, 3 },{ 4, 5 },{ 6, 3 },{ 6, 5 },{ 7, 4 },{ 8, 3 },{ 8, 5 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } },
	{ { 1, 2 },{ 1, 4 },{ 2, 3 },{ 3, 0 },{ 3, 2 },{ 4, 1 },{ 4, 3 },{ 5, 2 },{ 7, 4 },{ 8, 3 },{ 8, 5 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } }
	};
	std::vector<std::vector<Point>> perspectiveC = {
	{ { 0, 3 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{ 3, 2 },{ 6, 1 },{ 7, 0 },{ 7, 2 },{ 7, 4 },{ 8, 1 },{ 8, 3 },{ 8, 5 },{ 9, 2 },{ 9, 4 },{ 9, 6 } },
	{ { 0, 7 },{ 1, 6 },{ 2, 5 },{ 3, 4 },{ 6, 9 },{ 7, 4 },{ 7, 6 },{ 7, 8 },{ 7, 10 },{ 8, 7 },{ 8, 9 },{ 9, 8 } }
	};*/
	std::vector<std::vector<Point>> perspectiveU = {
	{ { 1, 2 },{ 1, 4 },{ 3, 0 },{ 3, 2 },{ 3, 4 },{ 3, 6 },{ 5, 8 },{ 6, 7 },{ 7, 0 },{ 7, 2 },{ 7, 4 },{ 8, 1 },{ 8, 3 },{ 8, 5 },{ 8, 9 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } },
	{ { 0, 3 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{ 3, 2 },{4, 7}, { 6, 1 },{6, 7}, { 7, 0 },{ 7, 2 },{ 7, 4 },{7, 6}, { 8, 1 },{ 8, 3 },{ 8, 5 },{ 9, 2 },{ 9, 4 },{ 9, 6 } },
	{ { 0, 3 },{ 0, 5 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{ 3, 2 },{ 3, 4 },{ 4, 3 },{ 4, 5 },{ 5, 4 },{ 6, 3 },{ 6, 5 },{ 7, 4 },{ 8, 5 },{8, 9},{9, 2},{ 9, 6 } }, 
	{ { 0, 3 },{ 0, 5 },{ 0, 7 },{ 1, 2 },{ 1, 4 },{ 1, 6 },{ 1, 8 },{ 2, 3 },{ 2, 5 },{ 2, 7 },{ 3, 4 },{ 3, 6 },{ 4, 5 },{ 4, 7 },{ 5, 6 },{ 6, 5 },{ 6, 7 },{ 7, 4 },{ 7, 6 },{ 8, 7 },{8, 9},{10, 5} }, 
	{ { 0, 7 },{ 1, 6 },{ 1, 8 },{ 2, 5 },{ 3, 4 },{4, 7}, {6, 7}, { 7, 4 },{ 7, 6 },{ 7, 8 },{ 8, 5 },{ 8, 7 },{ 8, 9 },{ 9, 6 },{ 9, 8 },{ 10, 7 } }, 
	//{ { 0, 7 },{ 1, 6 },{ 1, 8 },{ 2, 5 },{ 2, 7 },{ 2, 9 },{ 3, 4 },{ 3, 6 },{ 3, 8 },{ 3, 10 },{ 7, 4 },{ 7, 6 },{ 8, 5 },{ 8, 7 },{ 9, 6 },{ 9, 8 },{ 10, 7 } }, 
	{ {1, 2},{2, 1}, { 2, 5 },{ 4, 3 },{ 4, 5 },{ 6, 3 },{ 6, 5 },{6, 9}, { 7, 4 },{7, 8}, { 8, 3 },{ 8, 5 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } },
	{ {0, 7}, { 1, 4 },{2, 1}, { 2, 3 },{ 2, 5 },{ 3, 4 },{3, 6}, { 4, 3 },{ 5, 2 },{ 6, 1 },{ 6, 3 },{ 7, 0 },{ 7, 2 },{ 7, 4 },{ 8, 1 },{ 8, 3 },{ 8, 5 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } },
	};
	std::vector<std::vector<Point>> perspectiveL = {
	{ { 0, 3 },{ 0, 5 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{2, 9}, { 3, 2 },{ 5, 2 },{ 6, 1 },{ 6, 3 },{ 7, 0 },{ 7, 2 },{ 7, 4 },{ 8, 3 },{ 8, 5 },{ 9, 4 },{ 9, 6 } }, 
	{ { 0, 3 },{ 0, 5 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{ 3, 0 },{ 3, 2 },{ 4, 1 },{ 4, 3 },{ 5, 2 },{ 5, 4 },{ 6, 3 },{ 6, 5 },{ 7, 4 },{8, 1}, { 8, 5 },{ 9, 6 }, {10, 3} }, 
	{ { 0, 3 },{ 0, 5 },{ 0, 7 },{ 1, 2 },{ 1, 4 },{ 1, 6 },{ 1, 8 },{ 2, 3 },{ 2, 5 },{ 2, 7 },{ 3, 4 },{ 3, 6 },{ 4, 5 },{ 4, 7 },{ 5, 6 },{ 6, 5 },{ 6, 7 },{ 7, 4 },{ 7, 6 },{8, 1}, { 8, 7 }, {10, 5} }, 
	{ { 0, 7 },{ 1, 6 },{ 2, 5 },{2, 9}, { 3, 4 },{4, 7},{5, 8}, { 6, 9 },{ 7, 4 },{ 7, 6 },{ 7, 8 },{ 7, 10 },{ 8, 7 },{ 8, 9 },{ 9, 8 } }, 
	//{ { 0, 7 },{ 1, 6 },{ 1, 8 },{ 2, 5 },{ 3, 4 },{ 7, 4 },{ 7, 6 },{ 7, 8 },{ 8, 5 },{ 8, 7 },{ 8, 9 },{ 9, 6 },{ 9, 8 },{ 10, 7 } }, 
	{ {0, 3},{0, 7},{1, 2},{1, 8}, { 2, 5 },{2, 9}, { 4, 3 },{ 4, 5 },{ 6, 3 },{ 6, 5 },{ 7, 4 },{ 8, 3 },{ 8, 5 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } }, 
	{ { 1, 2 },{ 1, 4 },{ 2, 3 },{ 3, 0 },{ 3, 2 },{ 4, 1 },{ 4, 3 },{4, 5}, { 5, 2 },{5, 6}, {6, 7}, { 7, 4 },{ 8, 3 },{ 8, 5 },{8, 7}, { 9, 2 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } }
	};
	std::vector<std::vector<Point>> perspectiveC = {
	{ { 0, 3 },{0, 7}, { 1, 2 },{ 1, 4 },{1, 8}, { 2, 1 },{ 2, 3 },{ 3, 2 },{ 6, 1 },{ 7, 0 },{ 7, 2 },{ 7, 4 },{ 8, 1 },{ 8, 3 },{ 8, 5 },{ 9, 2 },{ 9, 4 },{ 9, 6 } },
	{ { 0, 7 },{ 1, 6 },{ 2, 5 },{ 3, 4 },{4, 7},{5, 2}, {5, 4}, {5, 6}, { 6, 9 },{ 7, 4 },{ 7, 6 },{ 7, 8 },{ 7, 10 },{ 8, 7 },{ 8, 9 },{ 9, 8 } }
	};

	std::vector<std::shared_ptr<Generate>> gens;
	for (int i = 0; i < 3; i++) gens.push_back(std::make_shared<Generate>());
	for (std::shared_ptr<Generate> g : gens) {
		g->setGridSize(5, 5);
		g->setSymbol(Decoration::Gap, 5, 0);
		g->setSymbol(Decoration::Gap, 5, 10);
		g->setFlag(Generate::Config::PreserveStructure);
		g->setFlag(Generate::DisableWrite);
		g->setFlag(Generate::ShortPath);
	}
	gens[0]->_starts = { { 2, 0 } }; gens[1]->_starts = { { 2, 10 } }; gens[2]->_starts = { { 10, 4 },{ 10, 6 } };
	gens[0]->_exits = { { 8, 10 } }; gens[1]->_exits = { { 8, 0 } }; gens[2]->_exits = { { 0, 4 },{ 0, 6 } };
	gens[0]->setObstructions(perspectiveU); gens[1]->setObstructions(perspectiveL); gens[2]->setObstructions(perspectiveC);

	_generator->setFlagOnce(Generate::SplitStones);
	_generator->generateMulti(id, gens, { { Decoration::Stone | Decoration::Color::Black, 2 },{ Decoration::Stone | Decoration::Color::White, 1, },
		{ Decoration::Star | Decoration::Color::Black, 1, },{ Decoration::Star | Decoration::Color::White, 1 } });

	gens[0]->_panel->WriteDecorations();
	gens[0]->_panel->_memory->WritePanelData<int>(id, STYLE_FLAGS, { gens[0]->_panel->_style });
	gens[0]->_panel->_memory->WritePanelData<int>(id, NEEDS_REDRAW, { 1 });
}

void Special::setTarget(int puzzle, int target)
{
	std::shared_ptr<Panel> panel = std::make_shared<Panel>();
	panel->_memory->WritePanelData<int>(puzzle, TARGET, { target + 1 });
}

void Special::clearTarget(int puzzle)
{
	std::shared_ptr<Panel> panel = std::make_shared<Panel>();
	panel->_memory->WritePanelData<int>(puzzle, TARGET, { 0 });
}

void Special::setTargetAndDeactivate(int puzzle, int target)
{
	std::shared_ptr<Panel> panel = std::make_shared<Panel>();
	panel->_memory->WritePanelData<float>(target, POWER, { 0.0, 0.0 });
	panel->_memory->WritePanelData<int>(puzzle, TARGET, { target + 1 });
}