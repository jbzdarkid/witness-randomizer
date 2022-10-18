#pragma once
#include "Memory.h"

class Randomizer {
public:
    Randomizer(const std::shared_ptr<Memory>& memory);

    template <typename T>
    std::vector<T> ReadPanelData(int panel, int offset, size_t size) {
        if (size == 0) return {};
        return _memory->ReadData<T>({ _globals, 0x18, panel * 8, offset }, size);
    }

    template <typename T>
    T ReadPanelData(int panel, int offset) {
        return ReadPanelData<T>(panel, offset, 1)[0];
    }

private:
    std::shared_ptr<Memory> _memory;

    // Sigscan outputs
    int64_t _globals = 0;
};
