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

    template <typename T>
    void WritePanelData(int panel, const int offset, const std::vector<T>& data) {
        if (data.size() == 0) return;
        _memory->WriteData({ _globals, 0x18, panel * 8, offset }, data);
    }

    template <typename T>
    void WritePanelData(int panel, const int offset, const T& data) {
        std::vector<T> dataVector = { data };
        _memory->WriteData({ _globals, 0x18, panel * 8, offset }, dataVector);
    }

    template <typename T>
    void WriteArray(int panel, const int offset, const std::vector<T>& data) {
        _memory->WriteArray({ _globals, 0x18, panel * 8, offset }, data);
    }

    // Slightly more human-usable functions
    std::vector<Traced_Edge> ReadTracedEdges(int panel);
	void DrawStartingPanelText(const std::vector<std::string>& textLines);

private:
    void DrawLine(int panel, const std::vector<float>& coords);
    void DrawText(int panel, const std::string& text, float top, float left, float bottom, float right);

    std::shared_ptr<Memory> _memory;
    std::vector<uintptr_t> _allocations;

    // Sigscan outputs
    int64_t _globals = 0;
};
