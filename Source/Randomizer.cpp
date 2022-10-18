#include "pch.h"
#include "Randomizer.h"
#include "PanelOffsets.h"

Randomizer::Randomizer(const std::shared_ptr<Memory>& memory) {
    _memory = memory;

    _memory->AddSigScan({0x74, 0x41, 0x48, 0x85, 0xC0, 0x74, 0x04, 0x48, 0x8B, 0x48, 0x10}, [&](int64_t offset, int index, const std::vector<uint8_t>& data) {
        _globals = Memory::ReadStaticInt(offset, index + 0x14, data);
    });

    size_t failedScans = _memory->ExecuteSigScans();
    assert(failedScans == 0);
}

std::vector<Traced_Edge> Randomizer::ReadTracedEdges(int panel) {
    std::vector<Traced_Edge> edgeData;

    int numEdges = ReadPanelData<int>(panel, TRACED_EDGES_COUNT);
    if (numEdges > 0) {
        // Explicitly computing this as an intermediate, since the edges array might have re-allocated.
        auto edgeDataPtr = ReadPanelData<int64_t>(panel, TRACED_EDGES_DATA);
        if (edgeDataPtr != 0) {
            static_assert(sizeof(Traced_Edge) == 0x34);
            edgeData = _memory->ReadAbsoluteData<Traced_Edge>({edgeDataPtr}, numEdges);
        }
    }

    return edgeData;
}
