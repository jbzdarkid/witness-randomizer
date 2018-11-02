#include "Panel.h"
#include "Random.h"

Decoration::Decoration(int shape) {
	_shape = shape;
}

int Decoration::GetValue() {
	return _shape;
}

Intersection::Intersection(float x, float y, int flags) {
	_x = x;
	_y = y;
	_flags = flags;
}

int Intersection::GetValue() {
	return _flags;
}

Panel::Panel(int id) {
	_width = _memory.ReadPanelData<int>(id, GRID_SIZE_X, 1)[0];
	_height = _memory.ReadPanelData<int>(id, GRID_SIZE_Y, 1)[0];

	ReadIntersections(id);
	ReadDecorations(id);
}

void Panel::Write(int id) {
	WriteIntersections(id);
	WriteDecorations(id);
	
	_memory.WritePanelData<int>(id, GRID_SIZE_X, {_width});
	_memory.WritePanelData<int>(id, GRID_SIZE_Y, {_height});
}

void Panel::Random() {
	for (auto& row : _decorations) {
		for (auto& cell : row) {
			cell._shape &= 0xFFFFFFF0;
			cell._shape |= Random::RandInt(1, 10);
		}
	}
}

void Panel::ReadDecorations(int id) {
	int numDecorations = _memory.ReadPanelData<int>(id, NUM_DECORATIONS, 1)[0];
	std::vector<int> decorations = _memory.ReadArray<int>(id, DECORATIONS, numDecorations);
	_decorations.resize(_width - 1);
	for (int x=0; x<_width-1; x++) {
		_decorations[x].resize(_height - 1);
		for (int y=0; y<_height-1; y++) {
			int i = x * (_height - 1) + y;
			_decorations[x][y] = Decoration(decorations[i]);
		}
	}
}

void Panel::WriteDecorations(int id) {
	std::vector<int> flattenedDecorations;
	for (std::vector<Decoration> row : _decorations) {
		for (Decoration decoration : row) {
			flattenedDecorations.push_back(decoration.GetValue());
		}
	}

	_memory.WritePanelData<int>(id, NUM_DECORATIONS, {static_cast<int>(flattenedDecorations.size())});
	_memory.WriteArray<int>(id, DECORATIONS, flattenedDecorations);
}

void Panel::ReadIntersections(int id) {
	int numIntersections = _memory.ReadPanelData<int>(id, NUM_DOTS, 1)[0];
	std::vector<float> intersections = _memory.ReadArray<float>(id, DOT_POSITIONS, numIntersections*2);
	std::vector<int> intersectionFlags = _memory.ReadArray<int>(id, DOT_FLAGS, numIntersections);
	_intersections.resize(_width);
	int i=0;
	for (int y=0; y<_height; y++) {
		for (int x=0; x<_width; x++) {
			_intersections[x].resize(_height);
			_intersections[x][y] = Intersection(intersections[2*i], intersections[2*i+1], intersectionFlags[i]);
			i++;
		}
	}

	// Iterate the remaining intersections (either endpoints or gaps)
	for (; i < numIntersections; i++) {
		if (intersectionFlags[i] & Intersection::Flags::IS_ENDPOINT) {
			_endpoints.push_back(Intersection(intersections[2*i], intersections[2*i+1], intersectionFlags[i]));
		}
		if (intersectionFlags[i] & Intersection::Flags::IS_GAP) {
			_gaps.push_back(Intersection(intersections[2*i], intersections[2*i+1], intersectionFlags[i]));
		}
	}
}

void Panel::WriteIntersections(int id) {
	std::vector<float> intersections;
	std::vector<int> intersectionFlags;
	std::pair<std::vector<int>, std::vector<int>> connections;

	for (int y=0; y<_height; y++) {
		for (int x=0; x<_width; x++) {
			Intersection intersection = _intersections[x][y];
			intersections.push_back(intersection._x);
			intersections.push_back(intersection._y);
			intersectionFlags.push_back(intersection._flags);
			if (y > 0) {
				connections.first.push_back(y * _width + x);
				connections.second.push_back((y - 1) * _width + x);
			}
			if (x > 0) {
				connections.first.push_back(y * _width + x);
				connections.second.push_back(y * _width + (x - 1));
			}
		}
	}

	// Endpoints go here :(

	int a = _memory.ReadPanelData<int>(id, NUM_DOTS, 1)[0];
	std::vector<float> b = _memory.ReadArray<float>(id, DOT_POSITIONS, a*2);
	std::vector<int> c = _memory.ReadArray<int>(id, DOT_FLAGS, a);
	std::pair<std::vector<int>, std::vector<int>> d;
	d.first = _memory.ReadArray<int>(id, DOT_CONNECTION_A, a);
	d.second = _memory.ReadArray<int>(id, DOT_CONNECTION_B, a);

	_memory.WritePanelData<int>(id, NUM_DOTS, {_width * _height});
	_memory.WriteArray<float>(id, DOT_POSITIONS, intersections);
	_memory.WriteArray<int>(id, DOT_FLAGS, intersectionFlags);
	_memory.WriteArray<int>(id, DOT_CONNECTION_A, connections.first);
	_memory.WriteArray<int>(id, DOT_CONNECTION_B, connections.second);
}

