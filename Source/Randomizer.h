#pragma once
#include "Memory.h"
#undef DrawText

struct Traced_Edge final {
    int32_t index_a;
    int32_t index_b;
    int32_t id_a;
    int32_t id_b;
    float t;
    float t_highest;
    float position_a[3];
    float position_b[3];
    bool water_reflect_a;
    bool water_reflect_b;
    bool hide_edge;
    bool padding;
};

class Randomizer {
public:
    Randomizer(const std::shared_ptr<Memory>& memory);

    template <typename T>
    std::vector<T> ReadPanelData(int panel, const int offset, size_t size) {
        if (size == 0) return {};
        return _memory->ReadData<T>({ _globals, 0x18, panel * 8, offset }, size);
    }

    template <typename T>
    T ReadPanelData(int panel, const int offset) {
        return ReadPanelData<T>(panel, offset, 1)[0];
    }

    std::vector<Traced_Edge> ReadTracedEdges(int panel);
    
    template <typename T>
    void WriteArray(int panel, const int offset, const std::vector<T>& data) {
        _memory->WriteArray({ _globals, 0x18, panel * 8, offset }, data);
    }

    /*
    void CreateText(int id, std::string text, std::vector<float>& intersections, std::vector<int>& connectionsA, std::vector<int>& connectionsB,
		float left, float right, float top, float bottom);
	void DrawText(int id, std::vector<float>& intersections, std::vector<int>& connectionsA, std::vector<int>& connectionsB, const std::vector<float>& finalLine);
	void DrawSeedAndDifficulty(int id, int seed, bool hard, bool setSeed, bool options);
	void DrawGoodLuckPanel(int id);
    */

private:
    std::shared_ptr<Memory> _memory;
    std::vector<uintptr_t> _allocations;

    // Sigscan outputs
    int64_t _globals = 0;
};
