#include "Generate.h"

std::pair<int, int> operator+(const std::pair<int, int>& l, const std::pair<int, int>& r) {
	return { l.first + r.first, l.second + r.second };
}

std::vector<std::pair<int, int>> Generate::DIRECTIONS = { std::make_pair(0, 2), std::make_pair(0, -2), std::make_pair(2, 0), std::make_pair(-2, 0) };

void Generate::readPanel(std::shared_ptr<Panel> panel) {
	_panel = panel;
	_starts = _panel->_startpoints;
	for (Endpoint e : _panel->_endpoints) {
		_exits.push_back(std::make_pair(e.GetX(), e.GetY()));
	}
}

void Generate::generate_path()
{
	generate_path((_panel->_width / 2 + 1) * (_panel->_height / 2 + 1) * 2 / 3);
}

void Generate::generate_path(int minLength)
{
	clear();
	int fails = 0;
	int length = 1;
	std::pair<int, int> pos = _starts[rand() % _starts.size()];
	std::pair<int, int> exit = _exits[rand() % _exits.size()];
	_panel->_grid[pos.first][pos.second] = PATH;
	while (pos != exit) {
		std::pair<int, int> dir = DIRECTIONS[rand() % 4];
		std::pair<int, int> newPos = pos + dir;
		if (newPos.first < 0 || newPos.first >= _panel->_width || newPos.second < 0 || newPos.second >= _panel->_height) continue;
		if (get(newPos) == 0 && !(newPos == exit && length + 1 < minLength)) {
			set(newPos, PATH);
			set(pos + std::make_pair(dir.first / 2, dir.second / 2), PATH);
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