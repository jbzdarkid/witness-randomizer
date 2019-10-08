#include "Special.h"
#include "MultiGenerate.h"

void Special::generateReflectionDotPuzzle(std::shared_ptr<Generate> generator, int id1, int id2, std::vector<std::pair<int, int>> symbols, Panel::Symmetry symmetry, bool split)
{
	generator->setFlagOnce(Generate::Config::DisableWrite);
	generator->generate(id1, symbols);
	std::shared_ptr<Panel> puzzle = generator->_panel;
	std::shared_ptr<Panel> flippedPuzzle = std::make_shared<Panel>(id2);
	std::vector<Point> dots;
	for (int x = 0; x < puzzle->_width; x++) {
		for (int y = 0; y < puzzle->_height; y++) {
			Point sp = puzzle->get_sym_point(x, y, symmetry);
			if (puzzle->_grid[x][y] & Decoration::Dot) {
				int symbol = (sp.first & 1) == 1 ? Decoration::Dot_Row : (sp.second & 1) == 1 ? Decoration::Dot_Column : Decoration::Dot_Intersection;
				flippedPuzzle->_grid[sp.first][sp.second] = symbol | IntersectionFlags::DOT_IS_INVISIBLE;
				dots.push_back({ x, y });
			}
			else if (puzzle->_grid[x][y] & Decoration::Gap) {
				int symbol = (sp.first & 1) == 1 ? Decoration::Gap_Row : Decoration::Gap_Column;
				flippedPuzzle->_grid[sp.first][sp.second] = symbol;
			}
			else flippedPuzzle->_grid[sp.first][sp.second] = puzzle->_grid[x][y];
		}
	}
	if (split) {
		int size = static_cast<int>(dots.size());
		while (dots.size() > size / 2 + rand() % 2) {
			Point dot = pop_random(dots);
			Point sp = puzzle->get_sym_point(dot.first, dot.second, symmetry);
			puzzle->_grid[dot.first][dot.second] |= IntersectionFlags::DOT_IS_INVISIBLE;
			flippedPuzzle->_grid[sp.first][sp.second] &= ~IntersectionFlags::DOT_IS_INVISIBLE;
		}
		if (rand() % 2) {
			Point dot = pop_random(dots);
			Point sp = puzzle->get_sym_point(dot.first, dot.second, symmetry);
			flippedPuzzle->_grid[sp.first][sp.second] &= ~IntersectionFlags::DOT_IS_INVISIBLE;
		}
		Color color = flippedPuzzle->_memory->ReadPanelData<Color>(id2, SUCCESS_COLOR_A);
		flippedPuzzle->_memory->WritePanelData<Color>(id2, PATTERN_POINT_COLOR, { color });
	}
	flippedPuzzle->_startpoints.clear();
	for (Point p : puzzle->_startpoints) {
		flippedPuzzle->_startpoints.push_back(puzzle->get_sym_point(p, symmetry));
	}
	flippedPuzzle->_endpoints.clear();
	for (Endpoint p : puzzle->_endpoints) {
		Point sp = puzzle->get_sym_point(p.GetX(), p.GetY(), symmetry);
		flippedPuzzle->_endpoints.push_back(Endpoint(sp.first, sp.second, generator->_panel->get_sym_dir(p.GetDir(), symmetry),
			IntersectionFlags::ENDPOINT | (p.GetDir() == Endpoint::Direction::UP || p.GetDir() == Endpoint::Direction::DOWN ? IntersectionFlags::COLUMN : IntersectionFlags::ROW)));
	}
	if (id1 == 0x00A5B && split) {
		if (checkDotSolvability(puzzle, flippedPuzzle, symmetry)) {
			generateReflectionDotPuzzle(generator, id1, id2, symbols, symmetry, split);
			return;
		}
	}
	generator->write(id1);
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

void Special::generateColorFilterPuzzle(int id, Point size, std::vector<std::pair<int, int>> symbols, Color filter)
{
	_generator->setFlagOnce(Generate::Config::DisableWrite);
	_generator->setGridSize(size.first, size.second);
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
			else symbolColors.push_back(availableColors[(_generator->get(x, y) & 0xf) - 1]);
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
	_generator->resetConfig();
}

void Special::generateSoundDotPuzzle(int id1, int id2, std::vector<int> dotSequence, bool writeSequence) {
	_generator->setFlag(Generate::Config::DisableReset);
	generateSoundDotPuzzle(id1, { 5, 5 }, dotSequence, writeSequence);
	_generator->write(id2);
	WritePanelData<Color>(id2, PATTERN_POINT_COLOR, { ReadPanelData<Color>(id2, SUCCESS_COLOR_A) });
	_generator->resetConfig();
}

void Special::generateSoundDotPuzzle(int id, Point size, std::vector<int> dotSequence, bool writeSequence) {
	_generator->setFlagOnce(Generate::Config::DisableWrite);
	_generator->setFlagOnce(Generate::Config::BackupPath);
	_generator->setFlagOnce(Generate::Config::LongPath);
	_generator->setGridSize(size.first, size.second);
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
			Point newp = p + dir;
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

void Special::generateSoundDotReflectionPuzzle(int id, Point size, std::vector<int> dotSequence1, std::vector<int> dotSequence2, int numColored, bool writeSequence)
{
	_generator->setFlagOnce(Generate::Config::DisableWrite);
	_generator->setFlagOnce(Generate::Config::BackupPath);
	_generator->setFlagOnce(Generate::Config::LongPath);
	_generator->setSymmetry(Panel::Symmetry::Rotational);
	_generator->setGridSize(size.first, size.second);
	if (id != 0x00AFB) {
		WritePanelData<Color>(id, SUCCESS_COLOR_B, { ReadPanelData<Color>(id, SUCCESS_COLOR_A) });
		_generator->setSymbol(Decoration::Start, 0, _generator->_height - 1); _generator->setSymbol(Decoration::Start, _generator->_width - 1, 0);
		_generator->setSymbol(Decoration::Exit, 0, 0); _generator->setSymbol(Decoration::Exit, _generator->_width - 1, _generator->_height - 1);
	}
	if (id == 0x00C3F || id == 0x00C41) {
		_generator->generate(id, Decoration::Dot_Intersection | Decoration::Color::Blue, static_cast<int>(dotSequence1.size()), Decoration::Dot_Intersection | Decoration::Color::Yellow, static_cast<int>(dotSequence2.size() - 1));
	}
	else if (id == 0x014B2) {
		_generator->generate(id, Decoration::Dot_Intersection | Decoration::Color::Blue, static_cast<int>(dotSequence1.size() - 1), Decoration::Dot_Intersection | Decoration::Color::Yellow, static_cast<int>(dotSequence2.size() - 1));
	}
	else _generator->generate(id, Decoration::Dot_Intersection | Decoration::Color::Blue, static_cast<int>(dotSequence1.size()), Decoration::Dot_Intersection | Decoration::Color::Yellow, static_cast<int>(dotSequence2.size()));
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
			Point newp = p1 + dir;
			if (path1.count(newp)) {
				p1 = newp;
				break;
			}
		}
	}
	if (id == 0x014B2) {
		_generator->set(p1, dotSequence1[seqPos] | Decoration::Dot_Intersection | IntersectionFlags::DOT_IS_BLUE);
		numColored++;
	}
	seqPos = 0;
	if (id == 0x00C3F || id == 0x00C41 || id == 0x014B2) {
		_generator->set(p2, Decoration::Dot_Intersection | IntersectionFlags::DOT_IS_ORANGE);
		_generator->setFlagOnce(Generate::Config::Write2Color);
		numColored++;
	}
	while (!_generator->_exits.count(p2)) {
		path2.erase(p2);
		int sym = _generator->get(p2);
		if (sym & Decoration::Dot) {
			_generator->set(p2, sym | dotSequence2[seqPos++]);
			if (!_generator->_starts.count(p2)) dots2.insert(p2);
		}
		for (Point dir : Generate::_DIRECTIONS1) {
			Point newp = p2 + dir;
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

void Special::generateRGBStonePuzzleH(int id) {
	while (true) {
		_generator->setFlagOnce(Generate::Config::DisableWrite);
		_generator->generate(id);
		int amount = 16;
		std::set<Decoration::Color> used;
		std::vector<Decoration::Color> colors = { Decoration::Black, Decoration::Red, Decoration::Green, Decoration::Blue, Decoration::Magenta, Decoration::Yellow };
		while (amount > 0) {
			Decoration::Color c = _generator->pick_random(colors);
			if (_generator->place_stones(c, 1)) {
				amount--;
				used.insert(c);
			}
		}
		if (used.size() < 5) continue;
		if (!_generator->place_erasers({ Decoration::Color::White }, { Decoration::Stone | pick_random(colors) })) {
			continue;
		}
		Point eraserPos;
		for (int x = 1; x < _generator->_panel->_width; x += 2) {
			for (int y = 1; y < _generator->_panel->_height; y += 2) {
				if (_generator->get_symbol_type(_generator->get(x, y)) == Decoration::Eraser) {
					eraserPos = { x, y };
					break;
				}
			}
		}
		int count = 0;
		for (Point dir : _generator->_DIRECTIONS2) {
			if (!_generator->off_edge(eraserPos + dir) && _generator->get(eraserPos + dir) == 0) count++;
		}
		if (count < 2) continue;
		_generator->setFlagOnce(Generate::Config::WriteColors);
		_generator->write(id);
		return;
	}
}

void Special::generateRGBDotPuzzleH(int id) {
	WritePanelData<Color>(id, PATTERN_POINT_COLOR, { {1, 0, 0, 1} });
	WritePanelData<Color>(id, PATTERN_POINT_COLOR_A, { { 0, 1, 1, 1 } });
	WritePanelData<Color>(id, PATTERN_POINT_COLOR_B, { { 1, 1, 0, 1 } });
	WritePanelData<Color>(id, ACTIVE_COLOR, { { 0, 1, 1, 1 } });
	WritePanelData<Color>(id, REFLECTION_PATH_COLOR, { { 1, 1, 0, 1 } });
	_generator->setGridSize(7, 7);
	_generator->setSymmetry(Panel::Symmetry::Rotational);
	_generator->setSymbol(Decoration::Exit, 0, 14); _generator->setSymbol(Decoration::Exit, 14, 0);
	_generator->setSymbol(Decoration::Exit, 0, 0); _generator->setSymbol(Decoration::Exit, 14, 14);
	_generator->generate(id, Decoration::Dot_Intersection | Decoration::Color::Cyan, 2, Decoration::Dot_Intersection | Decoration::Color::Yellow, 4, Decoration::Dot_Intersection, 6,
		Decoration::Triangle | Decoration::Color::Orange, 4, Decoration::Start, 4);
	_generator->resetConfig();
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

	PuzzleSymbols psymbols(symbols);

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
	std::vector<std::vector<Point>> perspectiveU = {
	{ { 1, 2 },{ 1, 4 },{ 3, 0 },{ 3, 2 },{ 3, 4 },{ 3, 6 },{ 5, 8 },{ 6, 7 },{ 7, 0 },{ 7, 2 },{ 7, 4 },{ 8, 1 },{ 8, 3 },{ 8, 5 },{ 8, 9 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } },
	{ { 0, 3 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{ 3, 2 },{4, 7}, { 6, 1 },{6, 7}, { 7, 0 },{ 7, 2 },{ 7, 4 },{7, 6}, { 8, 1 },{ 8, 3 },{ 8, 5 },{ 9, 2 },{ 9, 4 },{ 9, 6 } },
	{ { 0, 3 },{ 0, 5 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{ 3, 2 },{ 3, 4 },{ 4, 3 },{ 4, 5 },{ 5, 4 },{ 6, 3 },{ 6, 5 },{ 7, 4 },{ 8, 5 },{8, 9},{9, 2},{ 9, 6 } }, 
	{ { 0, 3 },{ 0, 5 },{ 0, 7 },{ 1, 2 },{ 1, 4 },{ 1, 6 },{ 1, 8 },{ 2, 3 },{ 2, 5 },{ 2, 7 },{ 3, 4 },{ 3, 6 },{ 4, 5 },{ 4, 7 },{ 5, 6 },{ 6, 5 },{ 6, 7 },{ 7, 4 },{ 7, 6 },{ 8, 7 },{8, 9},{10, 5} }, 
	{ { 0, 7 },{ 1, 6 },{ 1, 8 },{ 2, 5 },{ 3, 4 },{4, 7}, {6, 7}, { 7, 4 },{ 7, 6 },{ 7, 8 },{ 8, 5 },{ 8, 7 },{ 8, 9 },{ 9, 6 },{ 9, 8 },{ 10, 7 } }, 
	{ {1, 2},{2, 1}, { 2, 5 },{ 4, 3 },{ 4, 5 },{ 6, 3 },{ 6, 5 },{6, 9}, { 7, 4 },{7, 8}, { 8, 3 },{ 8, 5 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } },
	{ {0, 7}, { 1, 4 },{2, 1}, { 2, 3 },{ 2, 5 },{ 3, 4 },{3, 6}, { 4, 3 },{ 5, 2 },{ 6, 1 },{ 6, 3 },{ 7, 0 },{ 7, 2 },{ 7, 4 },{ 8, 1 },{ 8, 3 },{ 8, 5 },{ 9, 4 },{ 9, 6 },{ 10, 3 },{ 10, 5 },{ 10, 7 } },
	};
	std::vector<std::vector<Point>> perspectiveL = {
	{ { 0, 3 },{ 0, 5 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{2, 9}, { 3, 2 },{ 5, 2 },{ 6, 1 },{ 6, 3 },{ 7, 0 },{ 7, 2 },{ 7, 4 },{ 8, 3 },{ 8, 5 },{ 9, 4 },{ 9, 6 } }, 
	{ { 0, 3 },{ 0, 5 },{ 1, 2 },{ 1, 4 },{ 2, 1 },{ 2, 3 },{ 3, 0 },{ 3, 2 },{ 4, 1 },{ 4, 3 },{ 5, 2 },{ 5, 4 },{ 6, 3 },{ 6, 5 },{ 7, 4 },{8, 1}, { 8, 5 },{ 9, 6 }, {10, 3} }, 
	{ { 0, 3 },{ 0, 5 },{ 0, 7 },{ 1, 2 },{ 1, 4 },{ 1, 6 },{ 1, 8 },{ 2, 3 },{ 2, 5 },{ 2, 7 },{ 3, 4 },{ 3, 6 },{ 4, 5 },{ 4, 7 },{ 5, 6 },{ 6, 5 },{ 6, 7 },{ 7, 4 },{ 7, 6 },{8, 1}, { 8, 7 }, {10, 5} }, 
	{ { 0, 7 },{ 1, 6 },{ 2, 5 },{2, 9}, { 3, 4 },{4, 7},{5, 8}, { 6, 9 },{ 7, 4 },{ 7, 6 },{ 7, 8 },{ 7, 10 },{ 8, 7 },{ 8, 9 },{ 9, 8 } }, 
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
		g->setFlag(Generate::ShortPath);
		g->setFlag(Generate::DecorationsOnly);
	}
	gens[0]->_starts = { { 2, 0 } }; gens[1]->_starts = { { 2, 10 } }; gens[2]->_starts = { { 10, 4 },{ 10, 6 } };
	gens[0]->_exits = { { 8, 10 } }; gens[1]->_exits = { { 8, 0 } }; gens[2]->_exits = { { 0, 4 },{ 0, 6 } };
	gens[0]->setObstructions(perspectiveU); gens[1]->setObstructions(perspectiveL); gens[2]->setObstructions(perspectiveC);

	_generator->setFlagOnce(Generate::SplitStones);
	_generator->generateMulti(id, gens, { { Decoration::Stone | Decoration::Color::Black, 2 },{ Decoration::Stone | Decoration::Color::White, 1, },
		{ Decoration::Star | Decoration::Color::Black, 1, },{ Decoration::Star | Decoration::Color::White, 1 } });
}

void Special::generateMultiPuzzle(std::vector<int> ids, std::vector<std::vector<std::pair<int, int>>> symbolVec) {
	_generator->setFlagOnce(Generate::Config::DisableWrite);
	_generator->generate(ids[0]);
	std::vector<PuzzleSymbols> symbols;
	for (auto sym : symbolVec) symbols.push_back(PuzzleSymbols(sym));
	std::vector<Generate> gens;
	for (int i = 0; i < ids.size(); i++) gens.push_back(Generate());
	for (int i = 0; i < ids.size(); i++) {
		gens[i].setFlag(Generate::Config::DisableWrite);
		gens[i].setFlag(Generate::WriteColors);
		if (symbols[i].getNum(Decoration::Poly) > 1) gens[i].setFlag(Generate::RequireCombineShapes);
	}
	while (!generateMultiPuzzle(ids, gens, symbols, _generator->_path)) {
		_generator->generate(ids[0]);
	}
	for (int i = 0; i < ids.size(); i++) gens[i].write(ids[i]);
}

bool Special::generateMultiPuzzle(std::vector<int> ids, std::vector<Generate>& gens, std::vector<PuzzleSymbols> symbols, std::set<Point> path) {
	for (int i = 0; i < ids.size(); i++) {
		gens[i]._custom_grid.clear();
		gens[i].setPath(path);
		int fails = 0;
		while (!gens[i].generate(ids[i], symbols[i])) {
			if (fails++ > 20)
				return false;
		}
	}
	std::vector<std::string> solution; //For debugging only
	for (int y = 0; y < 11; y++) {
		std::string row;
		for (int x = 0; x < 11; x++) {
			if (path.count(Point(x, y))) {
				row += "xx";
			}
			else row += "    ";
		}
		solution.push_back(row);
	}
	return true;
}

void Special::generate2Bridge(int id1, int id2)
{
	std::vector<std::shared_ptr<Generate>> gens;
	for (int i = 0; i < 3; i++) gens.push_back(std::make_shared<Generate>());
	for (std::shared_ptr<Generate> g : gens) {
		g->setFlag(Generate::Config::DisableWrite);
		g->setFlag(Generate::Config::DisableReset);
		g->setFlag(Generate::Config::DecorationsOnly);
		g->setFlag(Generate::Config::ShortPath);
		g->setFlag(Generate::Config::WriteColors);
	}
	while (!generate2Bridge(id1, id2, gens));

	gens[1]->write(id1);
	gens[1]->write(id2);
}

bool Special::generate2Bridge(int id1, int id2, std::vector<std::shared_ptr<Generate>> gens)
{
	for (int i = 0; i < gens.size(); i++) {
		gens[i]->_custom_grid.clear();
		gens[i]->setPath(std::set<Point>());
		std::vector<Point> walls = { { 12, 1 },{ 12, 3 },{ 3, 8 },{ 9, 8 } };
		for (Point p : walls) gens[i]->setSymbol(Decoration::Gap, p.first, p.second);
		if (i % 2 == 0) {
			gens[i]->setObstructions({ { 5, 8 },{ 6, 7 },{ 7, 8 } });
		}
		else {
			gens[i]->setObstructions({ { 5, 0 },{ 6, 1 },{ 7, 0 } });
		}
	}

	int steps = 2;

	gens[0]->_exits = { { 12, 8 } };
	gens[1]->_exits = { { 0, 0 } };

	PuzzleSymbols symbols({ {Decoration::Poly | Decoration::Can_Rotate | Decoration::Color::Yellow, 1}, {Decoration::Star | Decoration::Color::Yellow, 1} });
	int fails = 0;
	while (!gens[0]->generate(id1, symbols)) {
		if (fails++ > 20)
			return false;
	}

	gens[1]->setPath(gens[0]->_path);
	gens[1]->customPath.clear();
	gens[1]->_custom_grid = gens[0]->_panel->_grid;
	symbols = PuzzleSymbols({ { Decoration::Poly | Decoration::Can_Rotate | Decoration::Color::Yellow, 1 },{ Decoration::Star | Decoration::Color::Yellow, 3 } });
	fails = 0;
	while (!gens[1]->generate(id2, symbols)) {
		if (fails++ > 20) {
			return false;
		}
	}

	if (gens[0]->get(11, 1) != 0 || gens[1]->get(11, 1) != 0 || gens[1]->get(11, 3) != 0 || gens[1]->get(11, 3) != 0)
		return false;
	
	//Make sure both shapes weren't blocked off by the same path
	int shapeCount = 0;
	for (int x = 1; x < gens[1]->_panel->_width; x += 2) {
		for (int y = 1; y < gens[1]->_panel->_height; y += 2) {
			if (gens[1]->get_symbol_type(gens[1]->get(x, y)) == Decoration::Poly && gens[1]->get_region(Point(x, y)).size() == gens[0]->get_region(Point(x, y)).size())
				if (++shapeCount == 2) return false;
		}
	}

	for (int x = 1; x < gens[1]->_panel->_width; x += 2) {
		for (int y = 1; y < gens[1]->_panel->_height; y += 2) {
			if (gens[1]->get_symbol_type(gens[1]->get(x, y)) == Decoration::Star) {
				if (!gens[1]->get_region(Point(x, y)).count({ 9, 7 }))
					continue;
				gens[1]->set(x, y, Decoration::Eraser | Decoration::Color::White);
				return true;
			}
		}
	}

	return false;
}

bool checkShape(std::set<Point> shape, int direction) {
	//Make sure it is not off the grid
	for (Point p : shape) if (p.first < 0 || p.first > 7 || p.second < 0 || p.second > 7)
		return false;
	//Make sure it is drawable
	std::set<Point> points1, points2;
	if (direction == 0) {
		points1 = { { 1, 7 },{ 3, 7 },{ 5, 7 },{ 7, 7 },{ 7, 5 },{ 7, 3 },{ 7, 1 } };
		points2 = { { 1, 7 },{ 1, 5 },{ 1, 3 },{ 1, 1 },{ 3, 1 },{ 5, 1 },{ 7, 1 } };
	}
	else {
		points1 = { { 7, 7 },{ 5, 7 },{ 3, 7 },{ 1, 7 },{ 1, 5 },{ 1, 3 },{ 1, 1 } };
		points2 = { { 7, 7 },{ 7, 5 },{ 7, 3 },{ 7, 1 },{ 5, 1 },{ 3, 1 },{ 1, 1 } };
	}
	int count = 0;
	bool consecutive = false;
	for (Point p : points1) {
		if (shape.count(p)) {
			if (!consecutive) {
				count++;
				consecutive = true;
			}
		}
		else consecutive = false;
	}
	if (count == 1)
		return true;
	count = 0;
	consecutive = false;
	for (Point p : points2) {
		if (shape.count(p)) {
			if (!consecutive) {
				count++;
				consecutive = true;
			}
		}
		else consecutive = false;
	}
	if (count == 1)
		return true;
	return false;
}

void Special::generateMountainFloor(std::vector<int> ids, int idfloor)
{
	_generator->resetConfig();
	std::vector<Point> floorPos = { { 3, 3 },{ 7, 3 },{ 3, 7 },{ 7, 7 } };
	_generator->openPos = std::set<Point>(floorPos.begin(), floorPos.end());
	_generator->setFlag(Generate::Config::DisableWrite);
	//Make sure no duplicated symbols
	std::set<int> sym;
	do {
		_generator->generate(idfloor, Decoration::Poly, 4);
		sym.clear();
		for (Point p : floorPos) sym.insert(_generator->get(p));
	} while (sym.size() < 4);

	int rotateIndex = rand() % 4;
	for (int i = 0; i < 4; i++) {
		int symbol = _generator->get(floorPos[i]);
		//Convert to shape
		Shape shape;
		for (int j = 0; j < 16; j++) {
			if (symbol & (1 << (j + 16))) {
				shape.insert(Point((j % 4) * 2 + 1, 8 - ((j / 4) * 2 + 1)));
			}
		}
		//Translate randomly
		Shape newShape;
		do {
			Point shift = Point((rand() % 4) * 2, -(rand() % 4) * 2);
			newShape.clear();
			for (Point p : shape) newShape.insert(p + shift);
		} while (!checkShape(newShape, i % 2));
		if (i == rotateIndex) {
			symbol = _generator->make_shape_symbol(newShape, true, false);
			if (symbol == 0) {
				symbol = _generator->get(floorPos[i]);
				rotateIndex++;
			}
		}

		Generate gen;
		for (Point p : newShape) {
			for (Point dir : Generate::_DIRECTIONS2) {
				if (!newShape.count(p + dir)) {
					gen.setSymbol(PATH, p.first + dir.first / 2, p.second + dir.second / 2);
				}
			}
		}
		gen.setPath({ {0, 0} }); //Just to stop it from trying to make a path
		gen.setFlag(Generate::Config::DecorationsOnly);
		gen.setFlag(Generate::Config::DisableWrite);
		gen.generate(ids[i], Decoration::Poly | (i == rotateIndex ? Decoration::Can_Rotate : 0), 1, Decoration::Eraser | Decoration::Color::Green, 1);
		std::set<Point> covered;
		int decoyShape;
		for (int x = 1; x <= 7; x += 2)
			for (int y = 1; y <= 7; y += 2)
				if (gen.get(x, y) != 0) {
					covered.insert(Point(x, y));
					if (gen.get_symbol_type(gen.get(x, y)) == Decoration::Poly) decoyShape = gen.get(x, y);
				}
		for (Point p : covered) newShape.erase(p);
		if (newShape.size() == 0 || decoyShape == symbol) {
			i--;
			continue;
		}
		Point pos = pick_random(newShape);
		gen.setVal(symbol, pos.first, pos.second);
		gen.write(ids[i]);
	}
	_generator->resetVars();
	_generator->resetConfig();
}

void Special::generatePivotPanel(int id, Point gridSize, std::vector<std::pair<int, int>> symbolVec) {
	int width = gridSize.first * 2 + 1, height = gridSize.second * 2 + 1;
	std::vector<std::shared_ptr<Generate>> gens;
	for (int i = 0; i < 3; i++) gens.push_back(std::make_shared<Generate>());
	for (std::shared_ptr<Generate> gen : gens) {
		gen->seed(rand());
		gen->setSymbol(Decoration::Start, width / 2, height - 1);
		gen->setGridSize(gridSize.first, gridSize.second);
		gen->setFlag(Generate::Config::DisableWrite);
	}
	gens[0]->setSymbol(Decoration::Exit, 0, height / 2);
	gens[1]->setSymbol(Decoration::Exit, width - 1, height / 2);
	gens[2]->setSymbol(Decoration::Exit, width / 2, 0);
	_generator->generateMulti(id, gens, symbolVec);
	gens[0]->_panel->_endpoints.clear();
	gens[0]->_panel->SetGridSymbol(0, height / 2, Decoration::Exit, Decoration::Color::None);
	gens[0]->_panel->SetGridSymbol(width - 1, height / 2, Decoration::Exit, Decoration::Color::None);
	gens[0]->_panel->SetGridSymbol(width / 2, 0, Decoration::Exit, Decoration::Color::None);
	gens[0]->write(id);
	int style = ReadPanelData<int>(id, STYLE_FLAGS);
	WritePanelData<int>(id, STYLE_FLAGS, { style | Panel::Style::IS_PIVOTABLE });
}

void Special::modifyGate(int id)
{
	std::shared_ptr<Panel> panel = std::make_shared<Panel>();
	int numIntersections = panel->_memory->ReadPanelData<int>(id, NUM_DOTS);
	std::vector<float> intersections = panel->_memory->ReadArray<float>(id, DOT_POSITIONS, numIntersections * 2);
	std::vector<int> intersectionFlags = panel->_memory->ReadArray<int>(id, DOT_FLAGS, numIntersections);
	if (intersectionFlags[24] == 0) return;
	int numConnections = panel->_memory->ReadPanelData<int>(id, NUM_CONNECTIONS);
	std::vector<int> connections_a = panel->_memory->ReadArray<int>(id, DOT_CONNECTION_A, numConnections);
	std::vector<int> connections_b = panel->_memory->ReadArray<int>(id, DOT_CONNECTION_B, numConnections);
	int style = panel->_memory->ReadPanelData<int>(id, STYLE_FLAGS);
	intersectionFlags[6] |= Decoration::Start;
	intersectionFlags[18] |= Decoration::Start;
	intersectionFlags[11] = Decoration::Dot_Intersection;
	intersectionFlags[24] = 0;
	intersectionFlags.push_back(0x400001);
	intersections.push_back(0.5f);
	intersections.push_back(1 - intersections[51]);
	connections_a.push_back(24);
	connections_b.push_back(numIntersections);
	std::vector<int> symData;
	for (int i = 0; i < numIntersections + 1; i++) {
		bool pushed = false;
		for (int j = 0; j < numIntersections + 1; j++) {
			if (std::round(intersections[i * 2] * 30) == std::round(30 - intersections[j * 2] * 30) &&
				std::round(intersections[i * 2 + 1] * 30) == std::round(30 - intersections[j * 2 + 1] * 30)) {
				symData.push_back(j);
				pushed = true;
				break;
			}
		}
		if (!pushed) symData.push_back(0);
	}
	panel->_memory->WriteArray<int>(id, DOT_FLAGS, intersectionFlags);
	panel->_memory->WriteArray<float>(id, DOT_POSITIONS, intersections);
	panel->_memory->WriteArray<int>(id, DOT_CONNECTION_A, connections_a);
	panel->_memory->WriteArray<int>(id, DOT_CONNECTION_B, connections_b);
	panel->_memory->WritePanelData<int>(id, NUM_DOTS, { numIntersections + 1 });
	panel->_memory->WritePanelData<int>(id, NUM_CONNECTIONS, { numConnections + 1 });
	panel->_memory->WriteArray<int>(id, REFLECTION_DATA, symData);
	Color successColor = panel->_memory->ReadPanelData<Color>(id, SUCCESS_COLOR_A);
	panel->_memory->WritePanelData<Color>(id, SUCCESS_COLOR_B, { successColor });
	panel->_memory->WritePanelData<Color>(id, PATTERN_POINT_COLOR, { { 0.1f, 0.1f, 0.1f, 1 } });
	panel->_memory->WritePanelData<int>(id, STYLE_FLAGS, { style | Panel::Style::HAS_DOTS });
	panel->_memory->WritePanelData<int>(id, NEEDS_REDRAW, { 1 });
}

void Special::addDecoyExits(std::shared_ptr<Generate> gen, int amount) {
	while (amount > 0) {
		Point pos;
		switch (rand() % 4) {
		case 0: pos = Point(0, rand() % gen->_height); break;
		case 1: pos = Point(gen->_width - 1, rand() % gen->_height); break;
		case 2: pos = Point(rand() % gen->_width, 0); break;
		case 3: pos = Point(rand() % gen->_width, gen->_height - 1); break;
		}
		if (pos.first % 2) pos.first--;
		if (pos.second % 2) pos.second--;
		if (gen->_exits.count(pos) || gen->_exits.count(gen->get_sym_point(pos)))
			continue;
		gen->_panel->SetGridSymbol(pos.first, pos.second, Decoration::Exit, Decoration::Color::None);
		gen->_exits.insert(pos);
		amount--;
	}
}

void Special::initSSGrid(std::shared_ptr<Generate> gen) {
	gen->setSymbol(Decoration::Start, 0, 0);  gen->setSymbol(Decoration::Start, 6, 0), gen->setSymbol(Decoration::Start, 8, 0), gen->setSymbol(Decoration::Start, 14, 0);
	gen->setSymbol(Decoration::Start, 0, 6);  gen->setSymbol(Decoration::Start, 6, 6), gen->setSymbol(Decoration::Start, 8, 6), gen->setSymbol(Decoration::Start, 14, 6);
	gen->setSymbol(Decoration::Start, 0, 8);  gen->setSymbol(Decoration::Start, 6, 8), gen->setSymbol(Decoration::Start, 8, 8), gen->setSymbol(Decoration::Start, 14, 8);
	gen->setSymbol(Decoration::Start, 0, 14);  gen->setSymbol(Decoration::Start, 6, 14), gen->setSymbol(Decoration::Start, 8, 14), gen->setSymbol(Decoration::Start, 14, 14);
	gen->setSymbol(Decoration::Exit, 2, 0);  gen->setSymbol(Decoration::Exit, 4, 0), gen->setSymbol(Decoration::Exit, 10, 0), gen->setSymbol(Decoration::Exit, 12, 0);
	gen->setSymbol(Decoration::Exit, 2, 14);  gen->setSymbol(Decoration::Exit, 4, 14), gen->setSymbol(Decoration::Exit, 10, 14), gen->setSymbol(Decoration::Exit, 12, 14);
	gen->setSymbol(Decoration::Exit, 0, 2);  gen->setSymbol(Decoration::Exit, 0, 4), gen->setSymbol(Decoration::Exit, 0, 10), gen->setSymbol(Decoration::Exit, 0, 12);
	gen->setSymbol(Decoration::Exit, 14, 2);  gen->setSymbol(Decoration::Exit, 14, 4), gen->setSymbol(Decoration::Exit, 14, 10), gen->setSymbol(Decoration::Exit, 14, 12);
}

void Special::generateSymmetryGate(int id) //TODO: Figure out why this one won't work until you click on it... 
{
	_generator->resetConfig();
	_generator->setFlag(Generate::Config::DisableWrite);
	_generator->setFlag(Generate::Config::WriteInvisible);
	_generator->setSymmetry(Panel::Symmetry::RotateRight);
	_generator->setSymbol(Decoration::Start, 0, 0);
	_generator->setSymbol(Decoration::Start, 0, 8);
	_generator->setSymbol(Decoration::Start, 8, 0);
	_generator->setSymbol(Decoration::Start, 8, 8);
	_generator->setSymbol(Decoration::Exit, 4, 0);
	_generator->setSymbol(Decoration::Exit, 4, 8);
	_generator->setSymbol(Decoration::Exit, 0, 4);
	_generator->setSymbol(Decoration::Exit, 8, 4);
	_generator->generate(id, Decoration::Triangle | Decoration::Color::Yellow, 4);
	std::vector<Point> breakPos = { {0, 3}, {2, 3}, {4, 3}, {6, 3}, {8, 3}, {0, 5}, {2, 5}, {4, 5}, {6, 5}, {8, 5} };
	for (Point p : breakPos) _generator->set(p, IntersectionFlags::COLUMN | 0x40000);
	breakPos = { { 3, 0 },{ 3, 2 },{ 3, 4 },{ 3, 6 },{ 3, 8 },{ 5, 0 },{ 5, 2 },{ 5, 4 },{ 5, 6 },{ 5, 8 } };
	for (Point p : breakPos) _generator->set(p, IntersectionFlags::ROW | 0x40000);
	//Expand the grid a little to prevent the collision detection from glitching out
	_generator->_panel->minx = 0.08f;
	_generator->_panel->maxx = 0.92f;
	_generator->_panel->miny = 0.08f;
	_generator->_panel->maxy = 0.92f;
	_generator->write(id);
	int numIntersections = ReadPanelData<int>(id, NUM_DOTS);
	std::vector<float> intersections = ReadArray<float>(id, DOT_POSITIONS, numIntersections * 2);
	std::vector<int> symData;
	for (int i = 0; i < numIntersections; i++) {
		bool pushed = false;
		for (int j = 0; j < numIntersections; j++) {
			int precision = 30;
			if (std::round(intersections[i * 2] * precision) == std::round(precision - intersections[j * 2 + 1] * precision) &&
				std::round(intersections[i * 2 + 1] * precision) == std::round(intersections[j * 2] * precision)) {
				symData.push_back(j);
				pushed = true;
				break;
			}
		}
		if (!pushed)
			symData.push_back(0);
	}
	WriteArray<int>(id, REFLECTION_DATA, symData);
}

bool Special::checkDotSolvability(std::shared_ptr<Panel> panel1, std::shared_ptr<Panel> panel2, Panel::Symmetry correctSym) {
	std::vector<Panel::Symmetry> sym = { Panel::Symmetry::FlipXY, Panel::Symmetry::FlipNegXY, Panel::Symmetry::RotateLeft, Panel::Symmetry::RotateRight };
	for (Panel::Symmetry s : sym) {
		if (s == correctSym) continue;
		bool found = false;
		//Check for three dots at a point
		for (int x = 0; x < panel1->_width; x += 2) {
			for (int y = 0; y < panel1->_height; y += 2) {
				int count = 0;
				for (Point dir : Generate::_DIRECTIONS1) {
					Point p = { x + dir.first, y + dir.second };
					Point sp = panel1->get_sym_point(p, s);
					if (p.first < 0 || p.second < 0 || p.first >= panel1->_width || p.second >= panel1->_height) continue;
					if (((panel1->_grid[p.first][p.second] & Decoration::Dot) && !(panel1->_grid[p.first][p.second] & IntersectionFlags::DOT_IS_INVISIBLE)) ||
						((panel2->_grid[sp.first][sp.second] & Decoration::Dot) && !(panel2->_grid[sp.first][sp.second] & IntersectionFlags::DOT_IS_INVISIBLE))) {
						if (++count == 3) {
							found = true;
							break;
						}
					}
				}
				if (found) break;
			}
			if (found) break;
		}
		//Check for four dots in a circle
		for (int x = 1; x < panel1->_width; x += 2) {
			for (int y = 1; y < panel1->_height; y += 2) {
				int count = 0;
				for (Point dir : Generate::_DIRECTIONS1) {
					Point p = { x + dir.first, y + dir.second };
					Point sp = panel1->get_sym_point(p, s);
					if (((panel1->_grid[p.first][p.second] & Decoration::Dot) && !(panel1->_grid[p.first][p.second] & IntersectionFlags::DOT_IS_INVISIBLE)) ||
						((panel2->_grid[sp.first][sp.second] & Decoration::Dot) && !(panel2->_grid[sp.first][sp.second] & IntersectionFlags::DOT_IS_INVISIBLE))) {
						if (++count == 4) {
							found = true;
							break;
						}
					}
				}
				if (found) break;
			}
			if (found) break;
		}
		if (!found) return true;
	}
	return false;
}


void Special::test() {
	
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