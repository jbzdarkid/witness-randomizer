#include "Special.h"

void Special::generateReflectionDotPuzzle(int id1, int id2, std::vector<std::pair<int, int>> symbols, Panel::Symmetry symmetry)
{
	_generator->setFlagOnce(Generate::Config::DisableWrite);
	while (!_generator->generate(id1, symbols));
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
	while (!_generator->generate(id, symbols));
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
	_generator->generate(id, Decoration::Dot_Intersection, static_cast<int>(dotSequence.size()));
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