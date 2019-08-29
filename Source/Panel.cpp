#include "Panel.h"
#include "Random.h"
#include "Memory.h"
#include "Randomizer.h"
#include <sstream>

template <class T>
int find(const std::vector<T> &data, T search, size_t startIndex = 0) {
	for (size_t i=startIndex ; i<data.size(); i++) {
		if (data[i] == search) return static_cast<int>(i);
	}
	return -1;
}

struct Color {
	int r;
	int g;
	int b;
	int a;
};

Panel::Panel() {
	_memory = std::make_shared<Memory>("witness64_d3d11.exe");
}

Panel::Panel(int id) {
	_memory = std::make_shared<Memory>("witness64_d3d11.exe");
	Read(id);
}

void Panel::Read(int id) {
	_width = 2 * _memory->ReadPanelData<int>(id, GRID_SIZE_X) - 1;
	_height = 2 * _memory->ReadPanelData<int>(id, GRID_SIZE_Y) - 1;
	if (_width <= 0 || _height <= 0) {
		int numIntersections = _memory->ReadPanelData<int>(id, NUM_DOTS);
		_width = _height = static_cast<int>(std::round(sqrt(numIntersections))) * 2 - 1;
	}
	_grid.resize(_width);
	for (auto& row : _grid) row.resize(_height);
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _height; y++) {
			_grid[x][y] = 0;
		}
	}
	_startpoints.clear();
	_endpoints.clear();

	_style = _memory->ReadPanelData<int>(id, STYLE_FLAGS);
	ReadAllData(id);
	ReadIntersections(id);
	ReadDecorations(id);
}

void Panel::Write(int id) {
	WriteIntersections(id);
	WriteDecorations(id);
	
	//_memory->WritePanelData<int>(id, PUSH_SYMBOL_COLORS, { 0 });
	_memory->WritePanelData<int>(id, DECORATION_COLORS, { 0 });
	_memory->WritePanelData<int>(id, STYLE_FLAGS, { _style & ~NO_BLINK });
	_memory->WritePanelData<int>(id, GRID_SIZE_X, {(_width + 1)/2});
	_memory->WritePanelData<int>(id, GRID_SIZE_Y, {(_height + 1)/2});
	_memory->WritePanelData<int>(id, NEEDS_REDRAW, { 1 });
}

void Panel::SetSymbol(int x, int y, Decoration::Shape symbol, Decoration::Color color)
{
	int gridx = x * 2 + (symbol & IntersectionFlags::COLUMN ? 0 : 1);
	int gridy = y * 2 + (symbol & IntersectionFlags::ROW ? 0 : 1);
	if (symbol & IntersectionFlags::DOT) {
		if (color == Decoration::Color::Blue || color == Decoration::Color::Cyan)
			color = static_cast<Decoration::Color>(IntersectionFlags::DOT_IS_BLUE);
		else if (color == Decoration::Color::Orange || color == Decoration::Color::Yellow)
			color = static_cast<Decoration::Color>(IntersectionFlags::DOT_IS_ORANGE);
		else color = Decoration::Color::None;
		if (symmetry != Symmetry::None) {
			Point sp = get_sym_point(gridx, gridy);
			SetGridSymbol(sp.first, sp.second, static_cast<Decoration::Shape>(symbol & ~Decoration::Dot), Decoration::Color::None);
		}
	}
	else if (symbol & IntersectionFlags::ROW || symbol & IntersectionFlags::COLUMN)
		color = Decoration::Color::None;
	SetGridSymbol(gridx, gridy, symbol, color);
}

void Panel::SetShape(int x, int y, int shape, bool rotate, bool negative, Decoration::Color color)
{
	if (!shape) return;
	int symbol = Decoration::Shape::Poly;
	while (!(shape & 0xf)) shape >>= 4;
	while (!(shape & 0x1111)) shape >>= 1;
	shape <<= 16;
	if (rotate) shape |= Decoration::Shape::Can_Rotate;
	else shape &= ~Decoration::Shape::Can_Rotate;
	if (negative) shape |= Decoration::Shape::Negative;
	else shape &= ~Decoration::Shape::Negative;
	_grid[x * 2 + 1][y * 2 + 1] = symbol | shape | color;
}

void Panel::ClearSymbol(int x, int y)
{
	ClearGridSymbol(x * 2 + 1, y * 2 + 1);
}

void Panel::SetGridSymbol(int x, int y, Decoration::Shape symbol, Decoration::Color color)
{
	if (symbol == Decoration::Start) _startpoints.push_back({ x, y });
	if (symbol == Decoration::Exit) {
		Endpoint::Direction dir;
		if (y == 0) dir = Endpoint::Direction::UP;
		else if (y == _height - 1) dir = Endpoint::Direction::DOWN;
		else if (x == 0) dir = Endpoint::Direction::LEFT;
		else dir = Endpoint::Direction::RIGHT;
		_endpoints.push_back(Endpoint(x, y, dir, IntersectionFlags::ENDPOINT | 
			(dir == Endpoint::Direction::UP || dir == Endpoint::Direction::DOWN ?
				IntersectionFlags::COLUMN : IntersectionFlags::ROW)));
	}
	else _grid[x][y] = symbol | color;
}

void Panel::ClearGridSymbol(int x, int y)
{
	_grid[x][y] = 0;
}

void Panel::Random() {
	//
}

//Only for testing
void Panel::ReadAllData(int id) {
	Color pathColor = _memory->ReadPanelData<Color>(id, PATH_COLOR);
	Color rpathColor = _memory->ReadPanelData<Color>(id, REFLECTION_PATH_COLOR);
	std::vector<byte> pathColorb = _memory->ReadPanelData<byte>(id, PATH_COLOR, 16);
	std::vector<byte> rpathColorb = _memory->ReadPanelData<byte>(id, REFLECTION_PATH_COLOR, 16);
	Color successColor = _memory->ReadPanelData<Color>(id, SUCCESS_COLOR_A);
	Color strobeColor = _memory->ReadPanelData<Color>(id, STROBE_COLOR_A);
	Color errorColor = _memory->ReadPanelData<Color>(id, ERROR_COLOR);
	int numDecorations = _memory->ReadPanelData<int>(id, NUM_DECORATIONS);
	Color a = _memory->ReadPanelData<Color>(id, SYMBOL_A);
	Color b = _memory->ReadPanelData<Color>(id, SYMBOL_B);
	Color c = _memory->ReadPanelData<Color>(id, SYMBOL_C);
	Color d = _memory->ReadPanelData<Color>(id, SYMBOL_D);
	Color e = _memory->ReadPanelData<Color>(id, SYMBOL_E);
	Color ppColor = _memory->ReadPanelData<Color>(id, PATTERN_POINT_COLOR);
	Color ppColorA = _memory->ReadPanelData<Color>(id, PATTERN_POINT_COLOR_A);
	Color ppColorB = _memory->ReadPanelData<Color>(id, PATTERN_POINT_COLOR_B);
	int pushSymbolColors = _memory->ReadPanelData<int>(id, PUSH_SYMBOL_COLORS);
	int numColored = _memory->ReadPanelData<int>(id, NUM_COLORED_REGIONS);
	std::vector<int> colored = _memory->ReadArray<int>(id, COLORED_REGIONS, numColored);
	int numConnections = _memory->ReadPanelData<int>(id, NUM_CONNECTIONS);
	int numDots = _memory->ReadPanelData<int>(id, NUM_DOTS);
	int reflectionData = _memory->ReadPanelData<int>(id, REFLECTION_DATA);
	if (reflectionData) std::vector<int> datas = _memory->ReadArray<int>(id, REFLECTION_DATA, numDots);
	int style = _memory->ReadPanelData<int>(id, STYLE_FLAGS);
	int ptr1 = _memory->ReadPanelData<int>(id, DOT_CONNECTION_A);
	int ptr2 = _memory->ReadPanelData<int>(id, DOT_CONNECTION_B);
	std::vector<int> connections_a = _memory->ReadArray<int>(id, DOT_CONNECTION_A, numConnections);
	std::vector<int> connections_b = _memory->ReadArray<int>(id, DOT_CONNECTION_B, numConnections);
	int numIntersections = _memory->ReadPanelData<int>(id, NUM_DOTS);
	std::vector<float> intersections = _memory->ReadArray<float>(id, DOT_POSITIONS, numIntersections * 2);
	std::vector<int> intersectionFlags = _memory->ReadArray<int>(id, DOT_FLAGS, numIntersections);
	int test = _memory->ReadPanelData<int>(id, DECORATIONS);
	std::vector<int> decorations = _memory->ReadArray<int>(id, DECORATIONS, numDecorations);
	std::vector<int> decorationFlags = _memory->ReadArray<int>(id, DECORATION_FLAGS, numDecorations);
}

void Panel::ReadDecorations(int id) {
	int numDecorations = _memory->ReadPanelData<int>(id, NUM_DECORATIONS);
	std::vector<int> decorations = _memory->ReadArray<int>(id, DECORATIONS, numDecorations);
	std::vector<int> decorationFlags = _memory->ReadArray<int>(id, DECORATION_FLAGS, numDecorations);

	for (int i=0; i<numDecorations; i++) {
		auto [x, y] = dloc_to_xy(i);
		_grid[x][y] = decorations[i];
	}
}

void Panel::WriteDecorations(int id) {
	std::vector<int> decorations;
	bool any = false;
	_style &= ~0x2f40; //Remove all element flags
	for (int y=_height-2; y>0; y-=2) {
		for (int x=1; x<_width - 1; x+=2) {
			decorations.push_back(_grid[x][y]);
			if (_grid[x][y])
				any = true;
			if ((_grid[x][y] & Decoration::Shape::Stone) == Decoration::Shape::Stone) _style |= HAS_STONES;
			if ((_grid[x][y] & Decoration::Shape::Star) == Decoration::Shape::Star) _style |= HAS_STARS;
			if ((_grid[x][y] & Decoration::Shape::Poly) == Decoration::Shape::Poly) _style |= HAS_SHAPERS;
			if ((_grid[x][y] & Decoration::Shape::Eraser) == Decoration::Shape::Eraser) _style |= HAS_ERASERS;
			if ((_grid[x][y] & Decoration::Shape::Triangle) == Decoration::Shape::Triangle) _style |= HAS_TRIANGLES;
		}
	}
	int ptr = _memory->ReadPanelData<int>(id, DECORATIONS);
	if (!any) {
		_memory->WritePanelData<int>(id, NUM_DECORATIONS, { 0 });
	}
	else _memory->WritePanelData<int>(id, NUM_DECORATIONS, { static_cast<int>(decorations.size()) });
	if (any || ptr) {
		_memory->WriteArray<int>(id, DECORATIONS, decorations);
		std::vector<int> test = _memory->ReadArray<int>(id, DECORATIONS, static_cast<int>(decorations.size()));
		for (int i = 0; i < decorations.size(); i++) decorations[i] = 0;
		_memory->WriteArray<int>(id, DECORATION_FLAGS, decorations);
		std::vector<int> test2 = _memory->ReadArray<int>(id, DECORATION_FLAGS, static_cast<int>(decorations.size()));
	}
}

void Panel::ReadIntersections(int id) {
	int numIntersections = _memory->ReadPanelData<int>(id, NUM_DOTS);
	std::vector<float> intersections = _memory->ReadArray<float>(id, DOT_POSITIONS, numIntersections * 2);
	int num_grid_points = this->num_grid_points();
	minx = intersections[0]; miny = intersections[1];
	maxx = intersections[num_grid_points * 2 - 2]; maxy = intersections[num_grid_points * 2 - 1];
	if (minx > maxx) std::swap(minx, maxx);
	if (miny > maxy) std::swap(miny, maxy);
	float unitWidth = (maxx - minx) / (_width - 1);
	float unitHeight = (maxy - miny) / (_height - 1);
	std::vector<int> intersectionFlags = _memory->ReadArray<int>(id, DOT_FLAGS, numIntersections);
	std::vector<int> symmetryData = _memory->ReadPanelData<int>(id, REFLECTION_DATA) ? 
		_memory->ReadArray<int>(id, REFLECTION_DATA, numIntersections) : std::vector<int>();
	if (symmetryData.size() == 0) symmetry = Symmetry::None;
	else if (symmetryData[0] == num_grid_points - 1) symmetry = Symmetry::Rotational;
	else if (symmetryData[0] == _width / 2 && intersections[1] == intersections[3]) symmetry = Symmetry::Vertical;
	else symmetry = Symmetry::Horizontal;

	for (int i = 0; i < num_grid_points; i++) {
		//auto [x, y] = loc_to_xy(i);
		int x = static_cast<int>(std::round((intersections[i * 2] - minx) / unitWidth));
		int y = _height - 1 - static_cast<int>(std::round((intersections[i * 2 + 1] - miny) / unitHeight));
		_grid[x][y] = intersectionFlags[i];
		if (intersectionFlags[i] & IntersectionFlags::STARTPOINT) {
			_startpoints.push_back({x, y});
		}
	}

	for (int y = 0; y < _height; y++) {
		for (int x = 0; x < _width; x++) {
			if (x % 2 == y % 2) continue;
			_grid[x][y] = OPEN;
		}
	}
	int numConnections = _memory->ReadPanelData<int>(id, NUM_CONNECTIONS);
	std::vector<int> connections_a = _memory->ReadArray<int>(id, DOT_CONNECTION_A, numConnections);
	std::vector<int> connections_b = _memory->ReadArray<int>(id, DOT_CONNECTION_B, numConnections);
	//Remove non-existent connections
	std::vector<std::string> out;
	for (int i = 0; i < connections_a.size(); i++) {
		out.push_back("(" + std::to_string(connections_a[i]) + ", " + std::to_string(connections_b[i]) + ")");
		if (connections_a[i] >= num_grid_points || connections_b[i] >= num_grid_points) continue;
		int x = static_cast<int>(std::round((intersections[connections_a[i] * 2] - minx) / unitWidth));
		int y = _height - 1 - static_cast<int>(std::round((intersections[connections_a[i] * 2 + 1] - miny) / unitHeight));
		int x2 = static_cast<int>(std::round((intersections[connections_b[i] * 2] - minx) / unitWidth));
		int y2 = _height - 1 - static_cast<int>(std::round((intersections[connections_b[i] * 2 + 1] - miny) / unitHeight));
		_grid[(x + x2) / 2][(y + y2) / 2] = 0;
	}

	// Iterate the remaining intersections (endpoints, dots, gaps)
	for (int i = num_grid_points; i < numIntersections; i++) {
		float xd = (intersections[i * 2] - minx) / unitWidth;
		float yd = (intersections[i * 2 + 1] - miny) / unitHeight;
		int x = std::clamp((int)std::round(xd), 0, _width - 1);
		int y = _height - 1 - std::clamp((int)std::round(yd), 0, _height - 1);
		if (intersectionFlags[i] & IntersectionFlags::GAP) {
			float xd2 = (intersections[i * 2 + 2] - minx) / unitWidth;
			float yd2 = (intersections[i * 2 + 3] - miny) / unitHeight;
			x = std::clamp((int)std::round((xd + xd2) / 2), 0, _width - 1);
			y = _height - 1 - std::clamp((int)std::round((yd + yd2) / 2), 0, _height - 1);
			bool fake = false;
			for (int j = 0; j < numConnections; j++) {
				if (connections_a[j] == i && connections_b[j] == i + 1 ||
					connections_a[j] == i + 1 && connections_b[j] == i) {
					//Fake symmetry wall
					fake = true;
					_grid[x][y] = 0;
					i++;
					break;
				}
			}
			if (fake) continue;
			i++;
		}
		if (intersectionFlags[i] & IntersectionFlags::ENDPOINT) {
			for (int j = 0; j<numConnections; j++) {
				int location = 0;
				if (connections_a[j] == i) location = connections_b[j];
				if (connections_b[j] == i) location = connections_a[j];
				if (location != 0) {
					Endpoint::Direction dir;
					if (intersections[2 * i] < intersections[2 * location]) { // Our (i) x coordinate is less than the target's (location)
						dir = Endpoint::Direction::LEFT;
					}
					else if (intersections[2 * i] > intersections[2 * location]) {
						dir = Endpoint::Direction::RIGHT;
					}
					else if (intersections[2 * i + 1] > intersections[2 * location + 1]) { // y coordinate is 0 (bottom) 1 (top), so this check is reversed.
						dir = Endpoint::Direction::UP;
					}
					else {
						dir = Endpoint::Direction::DOWN;
					}
					xd = (intersections[location * 2] - minx) / unitWidth;
					yd = (intersections[location * 2 + 1] - miny) / unitHeight;
					x = std::clamp((int)std::round(xd), 0, _width - 1);
					y = _height - 1 - std::clamp((int)std::round(yd), 0, _height - 1);
					_endpoints.push_back(Endpoint(x, y, dir, intersectionFlags[i]));
					break;
				}
			}
		}

		else {
			_grid[x][y] = intersectionFlags[i];
		}
	}	
}

void Panel::WriteIntersections(int id) {
	std::vector<float> intersections;
	std::vector<int> intersectionFlags;
	std::vector<int> connections_a;
	std::vector<int> connections_b;
	std::vector<int> symmetryData;
	switch (symmetry) {
	case Symmetry::Horizontal: symmetryData = sym_data_h(); break;
	case Symmetry::Vertical: symmetryData = sym_data_v(); break;
	case Symmetry::Rotational: symmetryData = sym_data_r(); break;
	}

	float unitWidth = (maxx - minx) / (_width - 1);
	float unitHeight = (maxy - miny) / (_height - 1);

	_style &= ~HAS_DOTS;

	for (int y = _height - 1; y >= 0; y -= 2) {
		for (int x = 0; x <_width; x += 2) {
			intersections.push_back(static_cast<float>(minx + x * unitWidth));
			intersections.push_back(static_cast<float>(miny + (_height - 1 - y) * unitHeight));
			intersectionFlags.push_back(_grid[x][y] | IntersectionFlags::INTERSECTION);
			if (_grid[x][y] & DOT) {
				_style |= HAS_DOTS;
				if (_grid[x][y] & IntersectionFlags::DOT_IS_BLUE || _grid[x][y] & IntersectionFlags::DOT_IS_ORANGE)
					_style |= IS_2COLOR;
			}

			// Create connections for this intersection -- always write low -> high
			if (y > 0 && _grid[x][y - 1] != OPEN) {
				connections_a.push_back(xy_to_loc(x, y - 2));
				connections_b.push_back(xy_to_loc(x, y));
			}
			if (x > 0 && _grid[x - 1][y] != OPEN) {
				connections_a.push_back(xy_to_loc(x - 2, y));
				connections_b.push_back(xy_to_loc(x, y));
			}
		}
	}

	
	std::vector<std::string> out;
	/*for (int i = 0; i < connections_a.size(); i++) {
		auto[x1, y1] = loc_to_xy(connections_a[i]);
		auto[x2, y2] = loc_to_xy(connections_b[i]);
		out.push_back("(" + std::to_string(x1) + ", " + std::to_string(y1) + ") > (" +
			std::to_string(x2) + ", " + std::to_string(y2) + ")");
	}*/

	for (int i = 0; i < _endpoints.size(); i++) {
		Endpoint endpoint = _endpoints[i];
		int x = endpoint.GetX(); int y = endpoint.GetY();
		if (x % 2 || y % 2) {
			int i = locate_segment(x, y, connections_a, connections_b);
			int other_connection = connections_b[i];
			connections_b[i] = static_cast<int>(intersectionFlags.size());
			connections_a.push_back(static_cast<int>(intersectionFlags.size()));
			connections_b.push_back(other_connection);
			intersections.push_back(static_cast<float>(minx + x * unitWidth));
			intersections.push_back(static_cast<float>(miny + (_height - 1 - y) * unitHeight));
			connections_a.push_back(static_cast<int>(intersectionFlags.size())); // Target to connect to
			intersectionFlags.push_back(_grid[x][y]);
		}
		else {
			connections_a.push_back(xy_to_loc(endpoint.GetX(), endpoint.GetY())); // Target to connect to
		}
		connections_b.push_back(static_cast<int>(intersectionFlags.size()));  // This endpoint
		double xPos = minx + endpoint.GetX() * unitWidth;
		double yPos = miny + (_height - 1 - endpoint.GetY()) * unitHeight;
		if (endpoint.GetDir() == Endpoint::Direction::LEFT) {
			xPos -= 0.05;
		}
		else if (endpoint.GetDir() == Endpoint::Direction::RIGHT) {
			xPos += 0.05;
		}
		else if (endpoint.GetDir() == Endpoint::Direction::UP) {
			yPos += 0.05;
		}
		else if (endpoint.GetDir() == Endpoint::Direction::DOWN) {
			yPos -= 0.05;
		}
		intersections.push_back(static_cast<float>(xPos));
		intersections.push_back(static_cast<float>(yPos));
		intersectionFlags.push_back(endpoint.GetFlags());
		if (symmetry && i % 2 == 1) {
			//Link pairs of exits in symmetry data
			symmetryData.push_back(static_cast<int>(intersectionFlags.size()) - 1);
			symmetryData.push_back(static_cast<int>(intersectionFlags.size()) - 2);
		}
	}

	/*out.clear();
	for (int i = 0; i < connections_a.size(); i++) {
		auto[x1, y1] = loc_to_xy(connections_a[i]);
		auto[x2, y2] = loc_to_xy(connections_b[i]);
		out.push_back("(" + std::to_string(x1) + ", " + std::to_string(y1) + ") > (" +
			std::to_string(x2) + ", " + std::to_string(y2) + ")");
	}*/
	
	// Dots/Gaps
	for (int y = _height - 1; y >= 0; y--) {
		for (int x = 0; x < _width; x++) {
			out.clear();
			for (int i = 0; i < connections_a.size(); i++) {
				/*
				auto[x1, y1] = loc_to_xy(connections_a[i]);
				auto[x2, y2] = loc_to_xy(connections_b[i]);
				out.push_back("(" + std::to_string(x1) + ", " + std::to_string(y1) + ") > (" +
					std::to_string(x2) + ", " + std::to_string(y2) + ")");*/
				out.push_back("(" + std::to_string(connections_a[i]) + ", " + std::to_string(connections_b[i]) + ")");
			}
			if (x % 2 == y % 2) continue;
			auto[sx, sy] = get_sym_point(x, y);
			if (_grid[x][y] == 0 || _grid[x][y] == OPEN) continue;
			int i = locate_segment(x, y, connections_a, connections_b);
			if (i == -1)
				continue;
			int other_connection = connections_b[i];
			if (_grid[x][y] & IntersectionFlags::GAP) {
				connections_b[i] = static_cast<int>(intersectionFlags.size() + 1);
				connections_a.push_back(other_connection);
				connections_b.push_back(static_cast<int>(intersectionFlags.size()));
				intersectionFlags.push_back(_grid[x][y]);
				intersectionFlags.push_back(_grid[x][y]);
				double xOffset = _grid[x][y] & IntersectionFlags::ROW ? 0.5 : 0;
				double yOffset = _grid[x][y] & IntersectionFlags::COLUMN ? 0.5 : 0;
				intersections.push_back(static_cast<float>(minx + (x + xOffset) * unitWidth));
				intersections.push_back(static_cast<float>(miny + (_height - 1 - y - yOffset) * unitHeight));
				intersections.push_back(static_cast<float>(minx + (x - xOffset) * unitWidth));
				intersections.push_back(static_cast<float>(miny + (_height - 1 - y + yOffset) * unitHeight));
				if (symmetry) {
					i = locate_segment(sx, sy, connections_a, connections_b);
					other_connection = connections_b[i];
					connections_b[i] = static_cast<int>(intersectionFlags.size());
					connections_a.push_back(other_connection);
					connections_b.push_back(static_cast<int>(intersectionFlags.size() + 1));
					connections_a.push_back(static_cast<int>(intersectionFlags.size()));
					connections_b.push_back(static_cast<int>(intersectionFlags.size() + 1));
					intersectionFlags.push_back(_grid[x][y]);
					intersectionFlags.push_back(_grid[x][y]);
					xOffset = _grid[x][y] & IntersectionFlags::ROW ? 0.5 : 0;
					yOffset = _grid[x][y] & IntersectionFlags::COLUMN ? 0.5 : 0;
					intersections.push_back(static_cast<float>(minx + (sx - xOffset) * unitWidth));
					intersections.push_back(static_cast<float>(miny + (_height - 1 - sy + yOffset) * unitHeight));
					intersections.push_back(static_cast<float>(minx + (sx + xOffset) * unitWidth));
					intersections.push_back(static_cast<float>(miny + (_height - 1 - sy - yOffset) * unitHeight));
					symmetryData.push_back(static_cast<int>(intersectionFlags.size()) - 2);
					symmetryData.push_back(static_cast<int>(intersectionFlags.size()) - 1);
					symmetryData.push_back(static_cast<int>(intersectionFlags.size()) - 4);
					symmetryData.push_back(static_cast<int>(intersectionFlags.size()) - 3);
					if (symmetry == Symmetry::Horizontal && (y & 1) == 0 || symmetry == Symmetry::Vertical && (x & 1) == 0) {
						std::swap(connections_b[i], connections_b[connections_b.size() - 2]);
						std::swap(intersections[intersections.size() - 1], intersections[intersections.size() - 3]);
						std::swap(intersections[intersections.size() - 2], intersections[intersections.size() - 4]);
					}
				}
			}
			else if (_grid[x][y] & IntersectionFlags::DOT) {
				connections_b[i] = static_cast<int>(intersectionFlags.size());
				connections_a.push_back(static_cast<int>(intersectionFlags.size()));
				connections_b.push_back(other_connection);
				intersectionFlags.push_back(_grid[x][y]);
				intersections.push_back(static_cast<float>(minx + x * unitWidth));
				intersections.push_back(static_cast<float>(miny + (_height - 1 - y) * unitHeight));
				_style |= HAS_DOTS;
				if (_grid[x][y] & IntersectionFlags::DOT_IS_BLUE || _grid[x][y] & IntersectionFlags::DOT_IS_ORANGE)
					_style |= IS_2COLOR;
				if (symmetry) {
					i = locate_segment(sx, sy, connections_a, connections_b);
					other_connection = connections_b[i];
					connections_b[i] = static_cast<int>(intersectionFlags.size());
					connections_a.push_back(static_cast<int>(intersectionFlags.size()));
					connections_b.push_back(other_connection);
					intersectionFlags.push_back(_grid[sx][sy]);
					intersections.push_back(static_cast<float>(minx + sx * unitWidth));
					intersections.push_back(static_cast<float>(miny + (_height - 1 - sy) * unitHeight));
					symmetryData.push_back(static_cast<int>(intersectionFlags.size()) - 1);
					symmetryData.push_back(static_cast<int>(intersectionFlags.size()) - 2);
				}
			}
		}
	}

	_memory->WritePanelData<int>(id, NUM_DOTS, { static_cast<int>(intersectionFlags.size()) });
	_memory->WriteArray<float>(id, DOT_POSITIONS, intersections);
	_memory->WriteArray<int>(id, DOT_FLAGS, intersectionFlags);
	_memory->WritePanelData<int>(id, NUM_CONNECTIONS, { static_cast<int>(connections_a.size()) });
	_memory->WriteArray<int>(id, DOT_CONNECTION_A, connections_a);
	_memory->WriteArray<int>(id, DOT_CONNECTION_B, connections_b);
	if (symmetryData.size() > 0) {
		_style |= Style::SYMMETRICAL;
		_memory->WriteArray<int>(id, REFLECTION_DATA, symmetryData);
	}
	else {
		_style &= ~Style::SYMMETRICAL;
		_memory->WritePanelData<int>(id, REFLECTION_DATA, { 0 });
	}
}