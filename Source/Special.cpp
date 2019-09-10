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

void Special::generateSoundDotPuzzle(int id, std::vector<int> dotSequence) {
	_generator->setFlagOnce(Generate::Config::DisableWrite);
	_generator->setFlagOnce(Generate::Config::BackupPath);
	_generator->generate(id, Decoration::Dot, static_cast<int>(dotSequence.size()));
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
	_generator->write(id);
}

void Special::generateRGBStonePuzzleN(int id)
{
	while (true) {
		_generator->setFlagOnce(Generate::Config::DisableWrite);
		if (!_generator->generate(id, { })) continue;
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
		if (!_generator->generate(id, {})) continue;
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

void Special::generateDualEraserPuzzle(int id, std::vector<std::pair<int, int>> symbols)
{

}



void Special::deactivateAndTarget(int targetPuzzle, int targetFrom)
{
	std::shared_ptr<Panel> panel = std::make_shared<Panel>();
	panel->_memory->WritePanelData<float>(targetPuzzle, POWER, { 0.0, 0.0 });
	panel->_memory->WritePanelData<int>(targetFrom, TARGET, { targetPuzzle + 1 });
	panel->_memory->WritePanelData<float>(0x0061A, OPEN_RATE, { 0.1f });
}
