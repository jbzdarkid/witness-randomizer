#include "pch.h"
#include "Randomizer.h"

Randomizer::Randomizer(const std::shared_ptr<Memory>& memory) {
    _memory = memory;

    _memory->AddSigScan({0x74, 0x41, 0x48, 0x85, 0xC0, 0x74, 0x04, 0x48, 0x8B, 0x48, 0x10}, [&](__int64 offset, int index, const std::vector<uint8_t>& data) {
        _globals = Memory::ReadStaticInt(offset, index + 0x14, data);
    });

    size_t failedScans = _memory->ExecuteSigScans();
    assert(failedScans == 0);
}
