#pragma once
#include "ThreadSafeAddressMap.h"

#include <functional>
#include <map>
#include <memory>

enum ProcStatus : WPARAM {
    // Emitted continuously while the randomizer is running and the game is not
    NotRunning,
    // Emitted continuously while the randomizer is running and the game is also running
    Running,
    // Emitted continuously while the randomizer is running and the game is loading
    Loading,

    // Emitted exactly once if game starts while the randomizer is running
    Started,
    // Emitted exactly once if randomzier starts while the game is running (and not in the middle of a load)
    AlreadyRunning,

    // Emitted exactly once if we detect that a save was loaded after ProcStats::Loading
    LoadSave,
    // Emitted exactly once if we detect that a new game was started after ProcStats::Loading
    NewGame,
    // Emitted exactly once if the game stops while the randomizer is running
    Stopped,
};

class Memory final : public std::enable_shared_from_this<Memory> {
public:
    Memory(const std::wstring& processName);
    ~Memory();
    void StartHeartbeat(HWND window, UINT message);
    void StopHeartbeat();
    void BringToFront();
    bool IsForeground();

    // Do not attempt to copy this object. Instead, use shared_ptr<Memory>
    Memory(const Memory& memory) = delete;
    Memory& operator=(const Memory& other) = delete;

    // bytesToEOL is the number of bytes from the given index to the end of the opcode.
    // Usually, the target address is last 4 bytes, since it's the destination of the call.
    static int64_t ReadStaticInt(int64_t offset, int index, const std::vector<uint8_t>& data, size_t bytesToEOL = 4);
    using ScanFunc = std::function<void(int64_t offset, int index, const std::vector<uint8_t>& data)>;
    using ScanFunc2 = std::function<bool(int64_t offset, int index, const std::vector<uint8_t>& data)>;
    void AddSigScan(const std::vector<uint8_t>& scanBytes, const ScanFunc& scanFunc);
    void AddSigScan2(const std::vector<uint8_t>& scanBytes, const ScanFunc2& scanFunc);
    [[nodiscard]] size_t ExecuteSigScans();

    // This is the fully typed function -- you mostly won't need to call this.
    int32_t CallFunction(int64_t address,
        const int64_t rcx, const int64_t rdx, const int64_t r8, const int64_t r9,
        const float xmm0, const float xmm1, const float xmm2, const float xmm3);
    int32_t CallFunction(int64_t address, int64_t rcx, const float xmm1) { return CallFunction(address, rcx, 0, 0, 0, 0.0f, xmm1, 0.0f, 0.0f); }

    template<class T>
    inline std::vector<T> ReadData(const std::vector<int64_t>& offsets, size_t numItems) {
        std::vector<T> data(numItems);
        if (!_handle) return data;
        ReadDataInternal(&data[0], ComputeOffset(offsets), numItems * sizeof(T));
        return data;
    }
    template<class T>
    inline std::vector<T> ReadAbsoluteData(const std::vector<int64_t>& offsets, size_t numItems) {
        std::vector<T> data(numItems);
        if (!_handle) return data;
        if (numItems == 0) return data;
        ReadDataInternal(&data[0], ComputeOffset(offsets, true), numItems * sizeof(T));
        return data;
    }
    std::string ReadString(const std::vector<int64_t>& offsets);

    template <class T>
    inline void WriteData(const std::vector<int64_t>& offsets, const std::vector<T>& data) {
        if (!_handle) return;
        if (data.size() == 0) return;
        WriteDataInternal(&data[0], ComputeOffset(offsets), sizeof(T) * data.size());
    }

    template <class T>
    void WriteArray(const std::vector<int64_t>& offsets, const std::vector<T>& data) {
        if (!_handle) return;
        if (data.size() == 0) return;
        WriteArrayInternal(&data[0], ComputeOffset(offsets), sizeof(T) * data.size());
    }

private:
    ProcStatus Heartbeat();
    void Initialize();
    static HWND GetProcessHwnd(DWORD pid);
    static void SetCurrentThreadName(const wchar_t* name);

    static void DebugPrint(const std::string& text);
    static void DebugPrint(const std::wstring& text);

    void ReadDataInternal(void* buffer, const uintptr_t computedOffset, size_t bufferSize);
    void WriteDataInternal(const void* buffer, const uintptr_t computedOffset, size_t bufferSize);
    void WriteArrayInternal(const void* buffer, const uintptr_t addressOfArray, size_t bufferSize);
    uintptr_t ComputeOffset(std::vector<int64_t> offsets, bool absolute = false);
    uintptr_t EnsureArrayCapacity(uintptr_t existingAddress, size_t size);

    // Parts of the constructor / StartHeartbeat
    std::wstring _processName;
    bool _threadActive = false;
    std::thread _thread;

    // Windows-y parts of initialize / heartbeat
    HANDLE _handle = nullptr;
    DWORD _pid = 0;
    HWND _hwnd = NULL;
    uintptr_t _baseAddress = 0;
    uintptr_t _endOfModule = 0;

    // These variables are all used to track game state
    int64_t _globals = 0;
    int _loadCountOffset = 0;
    int64_t _previousEntityManager = 0;
    int _previousLoadCount = 0;
    bool _gameHasStarted = false;
    bool _trainerHasStarted = false;
    bool _wasLoading = false;

#ifdef NDEBUG
    static constexpr std::chrono::milliseconds s_heartbeat = std::chrono::milliseconds(100);
#else // Induce more stress in debug, to catch errors more easily.
    static constexpr std::chrono::milliseconds s_heartbeat = std::chrono::milliseconds(10);
#endif

    // Parts of various memory primitives
    ThreadSafeAddressMap _computedAddresses;

    struct SigScan {
        bool found = false;
        ScanFunc2 scanFunc;
    };
    std::map<std::vector<uint8_t>, SigScan> _sigScans;

    std::vector<void*> _allocations;
    std::map<uintptr_t, size_t> _allocatedArrays;
    uintptr_t _functionPrimitive = 0;
};