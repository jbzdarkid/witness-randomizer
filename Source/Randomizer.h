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
        // When reading more than one datapoint, the object in the panel is a pointer to an array.
        return _memory->ReadData<T>({ _globals, 0x18, panel * 8, offset, 0 }, size);
    }

    template <typename T>
    T ReadPanelData(int panel, const int offset) {
        return _memory->ReadData<T>({ _globals, 0x18, panel * 8, offset }, 1)[0];
    }

    template <typename T>
    void WritePanelData(int panel, const int offset, const std::vector<T>& data) {
        if (data.size() == 0) return;
        _memory->WriteArray({ _globals, 0x18, panel * 8, offset }, data);
    }

    template <typename T>
    void WritePanelData(int panel, const int offset, const T& data) {
        std::vector<T> dataVector = { data };
        _memory->WriteData({ _globals, 0x18, panel * 8, offset }, dataVector);
    }

    // Slightly more human-usable functions
    std::vector<Traced_Edge> ReadTracedEdges(int panel);
    void DrawStartingPanelText(const std::vector<std::string>& textLines);\
    // 0.0: closed, 1.0: open
    void OpenDoor(int32_t door, float target = 1.0f);
    void ActivateLaser(int32_t laser);
    void ShowMessage(const std::string& message);

private:
    void ClearPanel(int panel);

    const int HALIGN_LEFT   = 0x1;
    const int HALIGN_CENTER = 0x2;
    const int HALIGN_RIGHT  = 0x4;
    const int VALIGN_TOP    = 0x8;
    const int VALIGN_CENTER = 0x10;
    const int VALIGN_BOTTOM = 0x20;
    void DrawText(int panel, const std::string& text, float x, float y, int alignment, float textSize = 0.05);
    void DrawLine(int panel, const std::vector<float>& coords);

    std::shared_ptr<Memory> _memory;
    std::vector<uintptr_t> _allocations;

    // Sigscan outputs
    int64_t _globals = 0;
    int64_t _openDoor = 0;
    int64_t _activateLaser = 0;
    int64_t _reportHudText = 0;
};
