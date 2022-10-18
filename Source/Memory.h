#pragma once
#include "ThreadSafeAddressMap.h"

#include <functional>
#include <map>
#include <memory>

enum ProcStatus : WPARAM {
    NotRunning,
    Started,
    Running,
    Reload,
    NewGame,
    Stopped
};

class Memory final : public std::enable_shared_from_this<Memory> {
public:
    Memory(const std::wstring& processName);
    ~Memory();
    void StartHeartbeat(HWND window, UINT message);
    void StopHeartbeat();
    void BringToFront();
    bool IsForeground();

    static HWND GetProcessHwnd(DWORD pid);

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
        ReadDataInternal(&data[0], ComputeOffset(offsets, true), numItems * sizeof(T));
        return data;
    }
    std::string ReadString(std::vector<int64_t> offsets);

    template <class T>
    inline void WriteData(const std::vector<int64_t>& offsets, const std::vector<T>& data) {
        WriteDataInternal(&data[0], offsets, sizeof(T) * data.size());
    }

private:
    void Heartbeat(HWND window, UINT message);
    void Initialize();
    static void SetCurrentThreadName(const wchar_t* name);
    static void DebugPrint(const std::string& text);
    static void DebugPrint(const std::wstring& text);

    void ReadDataInternal(void* buffer, const uintptr_t computedOffset, size_t bufferSize);
    void WriteDataInternal(const void* buffer, const std::vector<int64_t>& offsets, size_t bufferSize);
    uintptr_t ComputeOffset(std::vector<int64_t> offsets, bool absolute = false);

    // Parts of the constructor / StartHeartbeat
    std::wstring _processName;
    bool _threadActive = false;
    std::thread _thread;

    // Parts of Initialize / heartbeat
    HANDLE _handle = nullptr;
    DWORD _pid = 0;
    HWND _hwnd = NULL;
    uintptr_t _baseAddress = 0;
    uintptr_t _endOfModule = 0;
    int64_t _globals = 0;
    int _loadCountOffset = 0;
    int64_t _previousEntityManager = 0;
    int _previousLoadCount = 0;
    ProcStatus _nextStatus = ProcStatus::Started;
    bool _trainerHasStarted = false;

#ifdef NDEBUG
    static constexpr std::chrono::milliseconds s_heartbeat = std::chrono::milliseconds(100);
#else // Induce more stress in debug, to catch errors more easily.
    static constexpr std::chrono::milliseconds s_heartbeat = std::chrono::milliseconds(10);
#endif

    // Parts of Read / Write / Sigscan
    ThreadSafeAddressMap _computedAddresses;

    struct SigScan {
        bool found = false;
        ScanFunc2 scanFunc;
    };
    std::map<std::vector<uint8_t>, SigScan> _sigScans;
};