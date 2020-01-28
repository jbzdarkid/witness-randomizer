#pragma once

extern int GLOBALS, POSITION, ORIENTATION, PATH_COLOR, REFLECTION_PATH_COLOR, DOT_COLOR, ACTIVE_COLOR, BACKGROUND_REGION_COLOR, SUCCESS_COLOR_A, SUCCESS_COLOR_B, STROBE_COLOR_A, STROBE_COLOR_B, ERROR_COLOR, PATTERN_POINT_COLOR, PATTERN_POINT_COLOR_A, PATTERN_POINT_COLOR_B, SYMBOL_A, SYMBOL_B, SYMBOL_C, SYMBOL_D, SYMBOL_E, PUSH_SYMBOL_COLORS, OUTER_BACKGROUND, OUTER_BACKGROUND_MODE, TRACED_EDGES, AUDIO_PREFIX, POWER, TARGET, POWER_OFF_ON_FAIL, IS_CYLINDER, CYLINDER_Z0, CYLINDER_Z1, CYLINDER_RADIUS, CURSOR_SPEED_SCALE, NEEDS_REDRAW, SPECULAR_ADD, SPECULAR_POWER, PATH_WIDTH_SCALE, STARTPOINT_SCALE, NUM_DOTS, NUM_CONNECTIONS, MAX_BROADCAST_DISTANCE, DOT_POSITIONS, DOT_FLAGS, DOT_CONNECTION_A, DOT_CONNECTION_B, DECORATIONS, DECORATION_FLAGS, DECORATION_COLORS, NUM_DECORATIONS, REFLECTION_DATA, GRID_SIZE_X, GRID_SIZE_Y, STYLE_FLAGS, SEQUENCE_LEN, SEQUENCE, DOT_SEQUENCE_LEN, DOT_SEQUENCE, DOT_SEQUENCE_LEN_REFLECTION, DOT_SEQUENCE_REFLECTION, NUM_COLORED_REGIONS, COLORED_REGIONS, PANEL_TARGET, SPECULAR_TEXTURE, CABLE_TARGET_2, AUDIO_LOG_NAME, OPEN_RATE, METADATA, HOTEL_EP_NAME;

enum class ProcStatus {
    NotRunning,
    Running,
    NewGame
};

using byte = unsigned char;

// https://github.com/erayarslan/WriteProcessMemory-Example
// http://stackoverflow.com/q/32798185
// http://stackoverflow.com/q/36018838
// http://stackoverflow.com/q/1387064
// https://github.com/fkloiber/witness-trainer/blob/master/source/foreign_process_memory.cpp
class Memory final : public std::enable_shared_from_this<Memory> {
public:
    Memory(const std::wstring& processName);
    ~Memory();
    void StartHeartbeat(HWND window, WPARAM wParam, std::chrono::milliseconds beat = std::chrono::milliseconds(1000));

    Memory(const Memory& memory) = delete;
    Memory& operator=(const Memory& other) = delete;

    template <class T>
    std::vector<T> ReadArray(int id, int offset, int size) {
        return ReadData<T>({GLOBALS, 0x18, id*8, offset, 0}, size);
    }

    template <class T>
    void WriteArray(int id, int offset, const std::vector<T>& data) {
        WriteData({GLOBALS, 0x18, id*8, offset, 0}, data);
    }

    template <class T>
    void WriteNewArray(int id, int offset, const std::vector<T>& data) {
        uintptr_t addr = (uintptr_t)VirtualAllocEx(_handle, nullptr, data.size() * sizeof(T), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        _allocations.emplace_back(addr);
        WriteEntityData<uintptr_t>(id, offset, {addr});
        WriteArray(id, offset, data);
    }

    template <class T>
    std::vector<T> ReadEntityData(int id, int offset, size_t size) {
        return ReadData<T>({GLOBALS, 0x18, id*8, offset}, size);
    }

    template <class T>
    void WriteEntityData(int id, int offset, const std::vector<T>& data) {
        WriteData({GLOBALS, 0x18, id*8, offset}, data);
    }

    void AddSigScan(const std::vector<byte>& scanBytes, const std::function<void(int index)>& scanFunc);
    int ExecuteSigScans();

private:
    template<class T>
    std::vector<T> ReadData(const std::vector<int>& offsets, size_t numItems) {
        assert(numItems);
        std::vector<T> data;
        data.resize(numItems);
        if (!ReadProcessMemory(_handle, ComputeOffset(offsets), &data[0], sizeof(T) * numItems, nullptr)) {
            MEMORY_THROW("Failed to read data.", offsets, numItems);
        }
        return data;
    }

    template <class T>
    void WriteData(const std::vector<int>& offsets, const std::vector<T>& data) {
        assert(data.size());
        if (!WriteProcessMemory(_handle, ComputeOffset(offsets), &data[0], sizeof(T) * data.size(), nullptr)) {
            MEMORY_THROW("Failed to write data.", offsets, data.size());
        }
    }

    void Heartbeat(HWND window, WPARAM wParam);
    bool Initialize();
    void* ComputeOffset(std::vector<int> offsets);
    void LoadPanelOffsets();

    int _previousFrame = 0;
    bool _threadActive = false;
    std::thread _thread;
    std::wstring _processName;
    std::map<uintptr_t, uintptr_t> _computedAddresses;
    uintptr_t _baseAddress = 0;
    HANDLE _handle = nullptr;
    std::vector<uintptr_t> _allocations;
    struct SigScan {
        std::function<void(int)> scanFunc;
        bool found;
    };
    std::map<std::vector<byte>, SigScan> _sigScans;

    friend class Temp;
    friend class ChallengeRandomizer;
    friend class Randomizer;
};
