#include "pch.h"
#include "Memory.h"
#include <psapi.h>
#include <tlhelp32.h>

#undef PROCESSENTRY32
#undef Process32Next

Memory::Memory(const std::wstring& processName) : _processName(processName) {}

Memory::~Memory() {
    if (_threadActive) {
        _threadActive = false;
        _thread.join();
    }

    if (_handle != nullptr) {
        for (uintptr_t addr : _allocations) VirtualFreeEx(_handle, (void*)addr, 0, MEM_RELEASE);
        CloseHandle(_handle);
    }
}

void Memory::StartHeartbeat(HWND window, WPARAM wParam, std::chrono::milliseconds beat) {
    if (_threadActive) return;
    _threadActive = true;
    _thread = std::thread([sharedThis = shared_from_this(), window, wParam, beat]{
        while (sharedThis->_threadActive) {
            sharedThis->Heartbeat(window, wParam);
            std::this_thread::sleep_for(beat);
        }
    });
    _thread.detach();
}

void Memory::Heartbeat(HWND window, WPARAM wParam) {
    if (!_handle && !Initialize()) {
        // Couldn't initialize, definitely not running
        PostMessage(window, WM_COMMAND, wParam, (LPARAM)ProcStatus::NotRunning);
        return;
    }

    DWORD exitCode = 0;
    assert(_handle);
    GetExitCodeProcess(_handle, &exitCode);
    if (exitCode != STILL_ACTIVE) {
        // Process has exited, clean up.
        _computedAddresses.clear();
        _handle = NULL;
        PostMessage(window, WM_COMMAND, wParam, (LPARAM)ProcStatus::NotRunning);
        return;
    }

    int currentFrame = 0;
    if (GLOBALS == 0x5B28C0) {
        int currentFrame = ReadData<int>({0x5BE3B0}, 1)[0];
    } else if (GLOBALS == 0x62D0A0) {
        int currentFrame = ReadData<int>({0x63954C}, 1)[0];
    } else {
        assert(false);
        return;
    }

    int frameDelta = currentFrame - _previousFrame;
    _previousFrame = currentFrame;
    if (frameDelta < 0 && currentFrame < 250) {
        // Some addresses (e.g. Entity Manager) may get re-allocated on newgame.
        _computedAddresses.clear();
        PostMessage(window, WM_COMMAND, wParam, (LPARAM)ProcStatus::NewGame);
        return;
    }

    // TODO: Some way to return ProcStatus::Randomized vs ProcStatus::NotRandomized vs ProcStatus::DeRandomized;

    PostMessage(window, WM_COMMAND, wParam, (LPARAM)ProcStatus::Running);
}

[[nodiscard]]
bool Memory::Initialize() {
    // First, get the handle of the process
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(entry);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    while (Process32NextW(snapshot, &entry)) {
        if (_processName == entry.szExeFile) {
            _handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
            break;
        }
    }
    if (!_handle) {
        std::cerr << "Couldn't find " << _processName.c_str() << ", is it open?" << std::endl;
        return false;
    }

    // Next, get the process base address
    DWORD numModules;
    std::vector<HMODULE> moduleList(1024);
    EnumProcessModulesEx(_handle, &moduleList[0], static_cast<DWORD>(moduleList.size()), &numModules, 3);

    std::wstring name(64, '\0');
    for (DWORD i = 0; i < numModules / sizeof(HMODULE); i++) {
        int length = GetModuleBaseNameW(_handle, moduleList[i], &name[0], static_cast<DWORD>(name.size()));
        name.resize(length);
        if (_processName == name) {
            _baseAddress = (uintptr_t)moduleList[i];
            break;
        }
    }
    if (_baseAddress == 0) {
        std::cerr << "Couldn't locate base address" << std::endl;
        return false;
    }

    LoadPanelOffsets();
    return true;
}

void Memory::AddSigScan(const std::vector<byte>& scanBytes, const std::function<void(int index)>& scanFunc)
{
    _sigScans[scanBytes] = {scanFunc, false};
}

int find(const std::vector<byte> &data, const std::vector<byte>& search, size_t startIndex = 0) {
    for (size_t i=startIndex; i<data.size() - search.size(); i++) {
        bool match = true;
        for (size_t j=0; j<search.size(); j++) {
            if (data[i+j] == search[j]) {
                continue;
            }
            match = false;
            break;
        }
        if (match) return static_cast<int>(i);
    }
    return -1;
}

int Memory::ExecuteSigScans()
{
    for (int i=0; i<0x200000; i+=0x1000) {
        std::vector<byte> data = ReadData<byte>({i}, 0x1100);

        for (auto& [scanBytes, sigScan] : _sigScans) {
            if (sigScan.found) continue;
            int index = find(data, scanBytes);
            if (index == -1) continue;
            sigScan.scanFunc(i + index);
            sigScan.found = true;
        }
    }

    int notFound = 0;
    for (auto it : _sigScans) {
        if (it.second.found == false) notFound++;
    }
    return notFound;
}

void* Memory::ComputeOffset(std::vector<int> offsets) {
    // Leave off the last offset, since it will be either read/write, and may not be of type uintptr_t.
    int final_offset = offsets.back();
    offsets.pop_back();

    uintptr_t cumulativeAddress = _baseAddress;
    for (const int offset : offsets) {
        cumulativeAddress += offset;

        const auto search = _computedAddresses.find(cumulativeAddress);
        // This is an issue with re-randomization. Always. Just disable it in debug mode!
#ifdef NDEBUG
        if (search == std::end(_computedAddresses)) {
#endif
            // If the address is not yet computed, then compute it.
            uintptr_t computedAddress = 0;
            if (!ReadProcessMemory(_handle, reinterpret_cast<LPVOID>(cumulativeAddress), &computedAddress, sizeof(uintptr_t), NULL)) {
                MEMORY_THROW("Couldn't compute offset.", offsets);
            }
            if (computedAddress == 0) {
                MEMORY_THROW("Attempted to derefence NULL while computing offsets.", offsets);
            }
            _computedAddresses[cumulativeAddress] = computedAddress;
#ifdef NDEBUG
        }
#endif

        cumulativeAddress = _computedAddresses[cumulativeAddress];
    }
    return reinterpret_cast<void*>(cumulativeAddress + final_offset);
}

int GLOBALS, POSITION, ORIENTATION, PATH_COLOR, REFLECTION_PATH_COLOR, DOT_COLOR, ACTIVE_COLOR, BACKGROUND_REGION_COLOR, SUCCESS_COLOR_A, SUCCESS_COLOR_B, STROBE_COLOR_A, STROBE_COLOR_B, ERROR_COLOR, PATTERN_POINT_COLOR, PATTERN_POINT_COLOR_A, PATTERN_POINT_COLOR_B, SYMBOL_A, SYMBOL_B, SYMBOL_C, SYMBOL_D, SYMBOL_E, PUSH_SYMBOL_COLORS, OUTER_BACKGROUND, OUTER_BACKGROUND_MODE, TRACED_EDGES, AUDIO_PREFIX, POWER, TARGET, POWER_OFF_ON_FAIL, IS_CYLINDER, CYLINDER_Z0, CYLINDER_Z1, CYLINDER_RADIUS, CURSOR_SPEED_SCALE, NEEDS_REDRAW, SPECULAR_ADD, SPECULAR_POWER, PATH_WIDTH_SCALE, STARTPOINT_SCALE, NUM_DOTS, NUM_CONNECTIONS, MAX_BROADCAST_DISTANCE, DOT_POSITIONS, DOT_FLAGS, DOT_CONNECTION_A, DOT_CONNECTION_B, DECORATIONS, DECORATION_FLAGS, DECORATION_COLORS, NUM_DECORATIONS, REFLECTION_DATA, GRID_SIZE_X, GRID_SIZE_Y, STYLE_FLAGS, SEQUENCE_LEN, SEQUENCE, DOT_SEQUENCE_LEN, DOT_SEQUENCE, DOT_SEQUENCE_LEN_REFLECTION, DOT_SEQUENCE_REFLECTION, NUM_COLORED_REGIONS, COLORED_REGIONS, PANEL_TARGET, SPECULAR_TEXTURE, CABLE_TARGET_2, AUDIO_LOG_NAME, OPEN_RATE, METADATA, HOTEL_EP_NAME;

void Memory::LoadPanelOffsets() {
    AddSigScan({0x74, 0x41, 0x48, 0x85, 0xC0, 0x74, 0x04, 0x48, 0x8B, 0x48, 0x10}, [sharedThis = shared_from_this()](int index){
        // (address of next line) + (value at index)
        GLOBALS = index + 0x14 + 0x4 + sharedThis->ReadData<int>({index+0x14}, 1)[0];
    });
    ExecuteSigScans();

    if (GLOBALS == 0x5B28C0) {
        POSITION = 0x24;
        ORIENTATION = 0x34;
        PATH_COLOR = 0xC8;
        REFLECTION_PATH_COLOR = 0xD8;
        DOT_COLOR = 0xF8;
        ACTIVE_COLOR = 0x108;
        BACKGROUND_REGION_COLOR = 0x118;
        SUCCESS_COLOR_A = 0x128;
        SUCCESS_COLOR_B = 0x138;
        STROBE_COLOR_A = 0x148;
        STROBE_COLOR_B = 0x158;
        ERROR_COLOR = 0x168;
        PATTERN_POINT_COLOR = 0x188;
        PATTERN_POINT_COLOR_A = 0x198;
        PATTERN_POINT_COLOR_B = 0x1A8;
        SYMBOL_A = 0x1B8;
        SYMBOL_B = 0x1C8;
        SYMBOL_C = 0x1D8;
        SYMBOL_D = 0x1E8;
        SYMBOL_E = 0x1F8;
        PUSH_SYMBOL_COLORS = 0x208;
        OUTER_BACKGROUND = 0x20C;
        OUTER_BACKGROUND_MODE = 0x21C;
        TRACED_EDGES = 0x230;
        AUDIO_PREFIX = 0x278;
        POWER = 0x2A8;
        TARGET = 0x2BC;
        POWER_OFF_ON_FAIL = 0x2C0;
        IS_CYLINDER = 0x2FC;
        CYLINDER_Z0 = 0x300;
        CYLINDER_Z1 = 0x304;
        CYLINDER_RADIUS = 0x308;
        CURSOR_SPEED_SCALE = 0x358;
        NEEDS_REDRAW = 0x384;
        SPECULAR_ADD = 0x398;
        SPECULAR_POWER = 0x39C;
        PATH_WIDTH_SCALE = 0x3A4;
        STARTPOINT_SCALE = 0x3A8;
        NUM_DOTS = 0x3B8;
        NUM_CONNECTIONS = 0x3BC;
        MAX_BROADCAST_DISTANCE = 0x3C0;
        DOT_POSITIONS = 0x3C8;
        DOT_FLAGS = 0x3D0;
        DOT_CONNECTION_A = 0x3D8;
        DOT_CONNECTION_B = 0x3E0;
        DECORATIONS = 0x420;
        DECORATION_FLAGS = 0x428;
        DECORATION_COLORS = 0x430;
        NUM_DECORATIONS = 0x438;
        REFLECTION_DATA = 0x440;
        GRID_SIZE_X = 0x448;
        GRID_SIZE_Y = 0x44C;
        STYLE_FLAGS = 0x450;
        SEQUENCE_LEN = 0x45C;
        SEQUENCE = 0x460;
        DOT_SEQUENCE_LEN = 0x468;
        DOT_SEQUENCE = 0x470;
        DOT_SEQUENCE_LEN_REFLECTION = 0x478;
        DOT_SEQUENCE_REFLECTION = 0x480;
        NUM_COLORED_REGIONS = 0x4A0;
        COLORED_REGIONS = 0x4A8;
        PANEL_TARGET = 0x4B0;
        SPECULAR_TEXTURE = 0x4D8;
        CABLE_TARGET_2 = 0xD8;
        AUDIO_LOG_NAME = 0xC8;
        OPEN_RATE = 0xE8;
        METADATA = 0xF2; // sizeof(short)
        HOTEL_EP_NAME = 0x4BC640;
    } else if (GLOBALS == 0x62B0A0) {
        // TODO:
    } else if (GLOBALS == 0x62D0A0) {
        POSITION = 0x24;
        ORIENTATION = 0x34;
        PATH_COLOR = 0xC0;
        REFLECTION_PATH_COLOR = 0xD0;
        DOT_COLOR = 0xF0;
        ACTIVE_COLOR = 0x100;
        BACKGROUND_REGION_COLOR = 0x110;
        SUCCESS_COLOR_A = 0x120;
        SUCCESS_COLOR_B = 0x130;
        STROBE_COLOR_A = 0x140;
        STROBE_COLOR_B = 0x150;
        ERROR_COLOR = 0x160;
        PATTERN_POINT_COLOR = 0x180;
        PATTERN_POINT_COLOR_A = 0x190;
        PATTERN_POINT_COLOR_B = 0x1A0;
        SYMBOL_A = 0x1B0;
        SYMBOL_B = 0x1C0;
        SYMBOL_C = 0x1D0;
        SYMBOL_D = 0x1E0;
        SYMBOL_E = 0x1F0;
        PUSH_SYMBOL_COLORS = 0x200;
        OUTER_BACKGROUND = 0x204;
        OUTER_BACKGROUND_MODE = 0x214;
        TRACED_EDGES = 0x228;
        AUDIO_PREFIX = 0x270;
        POWER = 0x2A0;
        TARGET = 0x2B4;
        POWER_OFF_ON_FAIL = 0x2B8;
        IS_CYLINDER = 0x2F4;
        CYLINDER_Z0 = 0x2F8;
        CYLINDER_Z1 = 0x2FC;
        CYLINDER_RADIUS = 0x300;
        CURSOR_SPEED_SCALE = 0x350;
        NEEDS_REDRAW = 0x37C;
        SPECULAR_ADD = 0x38C;
        SPECULAR_POWER = 0x390;
        PATH_WIDTH_SCALE = 0x39C;
        STARTPOINT_SCALE = 0x3A0;
        NUM_DOTS = 0x3B4;
        NUM_CONNECTIONS = 0x3B8;
        MAX_BROADCAST_DISTANCE = 0x3BC;
        DOT_POSITIONS = 0x3C0;
        DOT_FLAGS = 0x3C8;
        DOT_CONNECTION_A = 0x3D0;
        DOT_CONNECTION_B = 0x3D8;
        DECORATIONS = 0x418;
        DECORATION_FLAGS = 0x420;
        DECORATION_COLORS = 0x428;
        NUM_DECORATIONS = 0x430;
        REFLECTION_DATA = 0x438;
        GRID_SIZE_X = 0x440;
        GRID_SIZE_Y = 0x444;
        STYLE_FLAGS = 0x448;
        SEQUENCE_LEN = 0x454;
        SEQUENCE = 0x458;
        DOT_SEQUENCE_LEN = 0x460;
        DOT_SEQUENCE = 0x468;
        DOT_SEQUENCE_LEN_REFLECTION = 0x470;
        DOT_SEQUENCE_REFLECTION = 0x478;
        NUM_COLORED_REGIONS = 0x498;
        COLORED_REGIONS = 0x4A0;
        PANEL_TARGET = 0x4A8;
        SPECULAR_TEXTURE = 0x4D0;
        CABLE_TARGET_2 = 0xD0;
        AUDIO_LOG_NAME = 0x0;
        OPEN_RATE = 0xE0;
        METADATA = 0x13A; // sizeof(short)
        HOTEL_EP_NAME = 0x51E340;
    } else {
        assert(false);
        return;
    }
}