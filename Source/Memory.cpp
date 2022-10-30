#include "pch.h"
#include "Windows.h"
#include <psapi.h>
#include <tlhelp32.h>

#include "Memory.h"

using namespace std;

Memory::Memory(const wstring& processName) : _processName(processName) {}

Memory::~Memory() {
    StopHeartbeat();
    if (_thread.joinable()) _thread.join();

    if (_handle != nullptr) {
        for (void* allocation : _allocations) {
            VirtualFreeEx(_handle, allocation, 0, MEM_RELEASE);
        }
        CloseHandle(_handle);
    }
}

void Memory::StartHeartbeat(HWND window, UINT message) {
    if (_threadActive) return;
    _threadActive = true;
    _thread = thread([sharedThis = shared_from_this(), window, message]{
        SetCurrentThreadName(L"Heartbeat");

        // Run the first heartbeat before setting trainerHasStarted, to detect if we are attaching to a game already in progress.
        ProcStatus status = sharedThis->Heartbeat();
        PostMessage(window, message, status, NULL);
        sharedThis->_trainerHasStarted = true;

        while (sharedThis->_threadActive) {
            this_thread::sleep_for(s_heartbeat);
            status = sharedThis->Heartbeat();
            PostMessage(window, message, status, NULL);

            // Wait for the process to fully close; otherwise we might accidentally re-attach to it.
            if (status == ProcStatus::Stopped) this_thread::sleep_for(chrono::milliseconds(1000));
        }
    });
    _thread.detach();
}

void Memory::StopHeartbeat() {
    _threadActive = false;
}

void Memory::BringToFront() {
    ShowWindow(_hwnd, SW_RESTORE); // This handles fullscreen mode
    SetForegroundWindow(_hwnd); // This handles windowed mode
}

bool Memory::IsForeground() {
    return GetForegroundWindow() == _hwnd;
}

HWND Memory::GetProcessHwnd(DWORD pid) {
    struct Data {
        DWORD pid;
        HWND hwnd;
    };
    Data data = Data{pid, NULL};

    BOOL result = EnumWindows([](HWND hwnd, LPARAM data) {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        DWORD targetPid = reinterpret_cast<Data*>(data)->pid;
        if (pid == targetPid) {
            reinterpret_cast<Data*>(data)->hwnd = hwnd;
            return FALSE; // Stop enumerating
        }
        return TRUE; // Continue enumerating
    }, (LPARAM)&data);

    return data.hwnd;
}

ProcStatus Memory::Heartbeat() {
    if (!_handle) {
        Initialize(); // Initialize promises to set _handle only on success
        if (!_handle) {
            // Couldn't initialize, definitely not running
            return NotRunning;
        }
    }

    DWORD exitCode = 0;
    GetExitCodeProcess(_handle, &exitCode);
    if (_trainerHasStarted && exitCode != STILL_ACTIVE) {
        // Process has exited, clean up. We only need to reset _handle here -- its validity is linked to all other class members.
        _computedAddresses.Clear();
        _allocatedArrays.clear();
        _functionPrimitive = 0;
        _handle = nullptr;
        _gameHasStarted = false;
        _wasLoading = false;

        return Stopped;
    }

    int64_t entityManager = ReadData<int64_t>({_globals}, 1)[0];
    // Game hasn't loaded yet, we're still sitting on the launcher
    if (entityManager == 0) return NotRunning;

    // To avoid obtaining the HWND for the launcher, we wait to determine HWND until after the entity manager is allocated (the main game has started).
    if (_hwnd == NULL) {
        _hwnd = GetProcessHwnd(_pid);
    } else {
        // Under some circumstances the window can expire? Or the game re-allocates it? I have no idea.
        // Anyways, we check to see if the title is wrong, and if so, search for the window again.
        constexpr int TITLE_SIZE = sizeof(L"The Witness") / sizeof(wchar_t);
        wchar_t title[TITLE_SIZE] = {L'\0'};
        GetWindowTextW(_hwnd, title, TITLE_SIZE);
        if (wcsncmp(title, L"The Witness", TITLE_SIZE) == 0) _hwnd = GetProcessHwnd(_pid);
    }

    if (_hwnd == NULL) {
        assert(false, "Couldn't find the window for the game");
        return NotRunning;
    }

    // New game / load game causes the entity manager to re-allocate,
    // but we need the entity manager to check if we're loading.
    if (entityManager != _previousEntityManager) {
        _previousEntityManager = entityManager;
        _computedAddresses.Clear();
    }

    uint8_t isLoading = ReadAbsoluteData<uint8_t>({entityManager, _loadCountOffset - 0x4}, 1)[0];
    // Saved game is currently loading, do not take any actions.
    if (isLoading == 0x01) {
        _wasLoading = true;
        return Loading;
    }

    int loadCount = ReadAbsoluteData<int>({entityManager, _loadCountOffset}, 1)[0];
    // Loading a game causes entities to be shuffled
    if (_previousLoadCount != loadCount) {
        _previousLoadCount = loadCount;
        _computedAddresses.Clear();
    }

    // This is slightly imprecise, since we can miss a load.
    // If we do detect a load, we should always emit either NewGame or LoadSave.
    if (_wasLoading) {
        _wasLoading = false;
        _gameHasStarted = true;

        if (loadCount == 0) return NewGame;
        return LoadSave;
    }

    // This is our first heartbeat where the entity manager was allocated.
    if (!_trainerHasStarted) {
        // The trainer just started, so the game was already running.
        _gameHasStarted = true;
        return AlreadyRunning;
    } else if (!_gameHasStarted) {
        // The trainer had already started, so the game must have just started.
        _gameHasStarted = true;
        return Started;
    }

    return Running;
}

void Memory::Initialize() {
    HANDLE handle = nullptr;
    // First, get the handle of the process
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(entry);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    while (Process32NextW(snapshot, &entry)) {
        if (_processName == entry.szExeFile) {
            _pid = entry.th32ProcessID;
            handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _pid);
            break;
        }
    }
    // Game likely not opened yet. Don't spam the log.
    if (!handle || !_pid) return;
    DebugPrint(L"Found " + _processName + L": PID " + to_wstring(_pid));

    _hwnd = NULL; // Will be populated later.

    DWORD unused;
    HMODULE modules[1];
    EnumProcessModules(handle, &modules[0], sizeof(HMODULE), &unused);
    MODULEINFO moduleInfo;
    GetModuleInformation(handle, modules[0], &moduleInfo, sizeof(moduleInfo));

    _baseAddress = reinterpret_cast<uint64_t>(moduleInfo.lpBaseOfDll);
    _endOfModule = _baseAddress + moduleInfo.SizeOfImage;
    if (_baseAddress == 0) {
        DebugPrint("Couldn't locate base address");
        return;
    }

    // Clear out any leftover sigscans from consumers (e.g. the trainer)
    _sigScans.clear();

    AddSigScan({0x74, 0x41, 0x48, 0x85, 0xC0, 0x74, 0x04, 0x48, 0x8B, 0x48, 0x10}, [&](int64_t offset, int index, const vector<uint8_t>& data) {
        _globals = Memory::ReadStaticInt(offset, index + 0x14, data);
    });

    AddSigScan({0x01, 0x00, 0x00, 0x66, 0xC7, 0x87}, [&](int64_t offset, int index, const vector<uint8_t>& data) {
        _loadCountOffset = *(int*)&data[index-1];
    });

    // This little song-and-dance is because we need _handle in order to execute sigscans.
    // But, we use _handle to indicate success, so we need to reset it.
    // Note that these sigscans are very lightweight -- they are *only* the scans required to handle loading.
    _handle = handle;
    size_t failedScans = ExecuteSigScans(); // Will DebugPrint the failed scans.
    if (failedScans > 0) _handle = nullptr;
}

void Memory::SetCurrentThreadName(const wchar_t* name) {
    HMODULE module = GetModuleHandleA("Kernel32.dll");
    if (!module) return;

    typedef HRESULT (WINAPI *TSetThreadDescription)(HANDLE, PCWSTR);
    auto setThreadDescription = (TSetThreadDescription)GetProcAddress(module, "SetThreadDescription");
    if (!setThreadDescription) return;

    setThreadDescription(GetCurrentThread(), name);
}

// These functions are much more generic than this witness-specific implementation. As such, I'm keeping them somewhat separated.

int64_t Memory::ReadStaticInt(int64_t offset, int index, const vector<uint8_t>& data, size_t bytesToEOL) {
    // (address of next line) + (index interpreted as 4uint8_t int)
    return offset + index + bytesToEOL + *(int*)&data[index];
}

// Small wrapper for non-failing scan functions
void Memory::AddSigScan(const vector<uint8_t>& scanBytes, const ScanFunc& scanFunc) {
    _sigScans[scanBytes] = {false, [scanFunc](int64_t offset, int index, const vector<uint8_t>& data) {
        scanFunc(offset, index, data);
        return true;
    }};
}

void Memory::AddSigScan2(const vector<uint8_t>& scanBytes, const ScanFunc2& scanFunc) {
    _sigScans[scanBytes] = {false, scanFunc};
}

int find(const vector<uint8_t>& data, const vector<uint8_t>& search) {
    const uint8_t* dataBegin = &data[0];
    const uint8_t* searchBegin = &search[0];
    size_t maxI = data.size() - search.size();
    size_t maxJ = search.size();

    for (int i=0; i<maxI; i++) {
        bool match = true;
        for (size_t j=0; j<maxJ; j++) {
            if (*(dataBegin + i + j) == *(searchBegin + j)) {
                continue;
            }
            match = false;
            break;
        }
        if (match) return i;
    }
    return -1;
}

#define BUFFER_SIZE 0x100000 // 100 KB
size_t Memory::ExecuteSigScans() {
    size_t notFound = 0;
    for (const auto& [_, sigScan] : _sigScans) if (!sigScan.found) notFound++;
    vector<uint8_t> buff;
    buff.resize(BUFFER_SIZE + 0x100); // padding in case the sigscan is past the end of the buffer

    for (uintptr_t i = _baseAddress; i < _endOfModule; i += BUFFER_SIZE) {
        SIZE_T numBytesWritten;
        if (!ReadProcessMemory(_handle, reinterpret_cast<void*>(i), &buff[0], buff.size(), &numBytesWritten)) continue;
        buff.resize(numBytesWritten);
        for (auto& [scanBytes, sigScan] : _sigScans) {
            if (sigScan.found) continue;
            int index = find(buff, scanBytes);
            if (index == -1) continue;
            sigScan.found = sigScan.scanFunc(i - _baseAddress, index, buff); // We're expecting i to be relative to the base address here.
            if (sigScan.found) notFound--;
        }
        if (notFound == 0) break;
    }

    if (notFound > 0) {
        DebugPrint("Failed to find " + to_string(notFound) + " sigscans:");
        for (const auto& [scanBytes, sigScan] : _sigScans) {
            if (sigScan.found) continue;
            stringstream ss;
            for (const auto b : scanBytes) {
                ss << "0x" << setw(2) << setfill('0') << hex << uppercase << static_cast<int16_t>(b) << ", ";
            }
            DebugPrint(ss.str());
        }
    } else {
        DebugPrint("Found all sigscans!");
    }

    _sigScans.clear();
    return notFound;
}

int32_t Memory::CallFunction(int64_t relativeAddress,
    const int64_t rcx, const int64_t rdx, const int64_t r8, const int64_t r9,
    const float xmm0, const float xmm1, const float xmm2, const float xmm3) {
    struct Arguments {
        uintptr_t address;
        int64_t rcx;
        int64_t rdx;
        int64_t r8;
        int64_t r9;
        float xmm0;
        float xmm1;
        float xmm2;
        float xmm3;
    };
    Arguments args = {
        ComputeOffset({relativeAddress}),
        rcx, rdx, r8, r9,
        xmm0, xmm1, xmm2, xmm3,
    };

    // Note: Assuming little endian
    #define LONG_TO_BYTES(val) \
        static_cast<uint8_t>((val & 0x00000000000000FF) >> 0x00), \
        static_cast<uint8_t>((val & 0x000000000000FF00) >> 0x08), \
        static_cast<uint8_t>((val & 0x0000000000FF0000) >> 0x10), \
        static_cast<uint8_t>((val & 0x00000000FF000000) >> 0x18), \
        static_cast<uint8_t>((val & 0x000000FF00000000) >> 0x20), \
        static_cast<uint8_t>((val & 0x0000FF0000000000) >> 0x28), \
        static_cast<uint8_t>((val & 0x00FF000000000000) >> 0x30), \
        static_cast<uint8_t>((val & 0xFF00000000000000) >> 0x38)

    #define OFFSET_OF(field) \
        static_cast<uint8_t>(((uint64_t)&args.##field - (uint64_t)&args.address) & 0x00000000000000FF)

	const uint8_t instructions[] = {
        0x48, 0xBB, LONG_TO_BYTES(0),               // mov rbx,  placeholder ; placeholder will be replaced by the address of the arguments struct
        0x48, 0x8B, 0x4B, OFFSET_OF(rcx),           // mov rcx,  args.rcx
        0x48, 0x8B, 0x53, OFFSET_OF(rdx),           // mov rdx,  args.rdx
        0x4C, 0x8B, 0x43, OFFSET_OF(r8),            // mov r8,   args.r8
        0x4C, 0x8B, 0x4B, OFFSET_OF(r9),            // mov r9,   args.r9
        0xF3, 0x0F, 0x7E, 0x43, OFFSET_OF(xmm0),    // mov xmm0, args.xmm0
        0xF3, 0x0F, 0x7E, 0x4B, OFFSET_OF(xmm1),    // mov xmm1, args.xmm1
        0xF3, 0x0F, 0x7E, 0x53, OFFSET_OF(xmm2),    // mov xmm2, args.xmm2
        0xF3, 0x0F, 0x7E, 0x5B, OFFSET_OF(xmm3),    // mov xmm3, args.xmm3
        0x48, 0x83, 0xEC, 0x48,                     // sub rsp,48 ; align the stack pointer for movss opcodes
        0xFF, 0x13,                                 // call [rbx]
        0x48, 0x83, 0xC4, 0x48,                     // add rsp,48
        0xC3,                                       // ret
    };

    if (!_functionPrimitive) {
        _functionPrimitive = EnsureArrayCapacity(NULL, sizeof(instructions) + sizeof(Arguments));
        WriteDataInternal(instructions, _functionPrimitive, sizeof(instructions));

        uint8_t argsAddress[] = {LONG_TO_BYTES(_functionPrimitive + sizeof(instructions))};
        WriteDataInternal(argsAddress, _functionPrimitive + 2, sizeof(argsAddress));
    }

    WriteDataInternal(&args, _functionPrimitive + sizeof(instructions), sizeof(args));

    // Argument 5 (lpParameter) is passed to the target thread in rcx, but just as a value.
    // If it points to data, it will continue to point to data in the wrong process.
    HANDLE thread = CreateRemoteThread(_handle, NULL, 0, (LPTHREAD_START_ROUTINE)_functionPrimitive, 0, 0, 0);
	DWORD result = WaitForSingleObject(thread, INFINITE);

    int32_t exitCode = 0;
    static_assert(sizeof(DWORD) == sizeof(exitCode));
    GetExitCodeThread(thread, reinterpret_cast<LPDWORD>(&exitCode));
    return exitCode;
}

int32_t Memory::CallFunction(int64_t address, const string& str) {
    uintptr_t addr = EnsureArrayCapacity(NULL, str.size());
    WriteDataInternal(&str[0], addr, str.size());
    return CallFunction(address, addr);
}

// Technically this is ReadChar*, but this name makes more sense with the return type.
string Memory::ReadString(const vector<int64_t>& offsets) {
    int64_t charAddr = ReadData<int64_t>(offsets, 1)[0];
    if (charAddr == 0) return ""; // Handle nullptr for strings
    
    vector<char> tmp;
    auto nullTerminator = tmp.begin(); // Value is only for type information.
    for (size_t maxLength = (1 << 6); maxLength < (1 << 10); maxLength *= 2) {
        tmp = ReadAbsoluteData<char>({charAddr}, maxLength);
        nullTerminator = find(tmp.begin(), tmp.end(), '\0');
        // If a null terminator is found, we will strip any trailing data after it.
        if (nullTerminator != tmp.end()) break;
    }
    return string(tmp.begin(), nullTerminator);
}

void Memory::ReadDataInternal(void* buffer, uintptr_t computedOffset, size_t bufferSize) {
    assert(bufferSize > 0, "Internal consistency check failed: attempted to read 0 bytes");
    if (!_handle) return;
    if (bufferSize > 0x1000 - (computedOffset & 0x0000FFF)) {
        bufferSize = 0x1000 - (computedOffset & 0x0000FFF);
        assert(false, "Warning: Attempted to read across a page boundary! Read has been truncated.");
    }
    if (!ReadProcessMemory(_handle, (void*)computedOffset, buffer, bufferSize, nullptr)) {
        assert(false, "Failed to read process memory.");
    }
}

void Memory::WriteDataInternal(const void* buffer, const uintptr_t computedOffset, size_t bufferSize) {
    assert(bufferSize > 0, "Internal consistency check failed: attempted to write 0 bytes");
    if (!_handle) return;
    if (bufferSize > 0x1000 - (computedOffset & 0x0000FFF)) {
        bufferSize = 0x1000 - (computedOffset & 0x0000FFF);
        assert(false, "Warning: Attempted to write across a page boundary! Write has been truncated.");
    }
    if (!WriteProcessMemory(_handle, (void*)computedOffset, buffer, bufferSize, nullptr)) {
        assert(false, "Failed to write process memory.");
    }
}

void Memory::WriteArrayInternal(const void* buffer, const uintptr_t addressOfArray, size_t bufferSize) {
    uintptr_t addressOfArrayData = ComputeOffset({static_cast<int64_t>(addressOfArray), 0}, true);

    uintptr_t newAddress = EnsureArrayCapacity(addressOfArrayData, bufferSize);
    if (newAddress) addressOfArrayData = newAddress;
    WriteDataInternal(buffer, addressOfArrayData, bufferSize); // To be extra safe, we fill out the array data before swapping the array pointer.

    if (newAddress) {
        // If the array reallocated, write the new address into the previous array.
        // Also reset the associated value in the computed address map.
        _computedAddresses.Set(addressOfArray, addressOfArrayData);
        uintptr_t newAddressArr[1] = { newAddress }; // Local array allocation because WriteDataInternal expects an array
        WriteDataInternal(&newAddressArr[0], addressOfArray, sizeof(uintptr_t));
    }
}

uintptr_t Memory::ComputeOffset(vector<int64_t> offsets, bool absolute) {
    assert(offsets.size() > 0, "Internal consistency check: attempted to compute offset with 0 offsets.");
    assert(offsets.front() != 0, "Internal consistency check: attempted to compute offset of NULL.");

    // Leave off the last offset, since it will be either read/write, and may not be of type uintptr_t.
    const int64_t final_offset = offsets.back();
    offsets.pop_back();

    uintptr_t cumulativeAddress = (absolute ? 0 : _baseAddress);
    for (const int64_t offset : offsets) {
        cumulativeAddress += offset;

        // If the address was already computed, continue to the next offset.
        uintptr_t foundAddress = _computedAddresses.Find(cumulativeAddress);
        if (foundAddress != 0) {
            cumulativeAddress = foundAddress;
            continue;
        }

        // If the address was not yet computed, read it from memory.
        uintptr_t computedAddress = 0;
        if (!_handle) return 0;
        if (ReadProcessMemory(_handle, reinterpret_cast<LPCVOID>(cumulativeAddress), &computedAddress, sizeof(computedAddress), NULL) && computedAddress != 0) {
            // Success!
            _computedAddresses.Set(cumulativeAddress, computedAddress);
            cumulativeAddress = computedAddress;
            continue;
        }

        MEMORY_BASIC_INFORMATION info;
        if (computedAddress == 0) {
            assert(false, "Attempted to dereference NULL!");
        } else if (!VirtualQuery(reinterpret_cast<LPVOID>(cumulativeAddress), &info, sizeof(computedAddress))) {
            assert(false, "Failed to read process memory, probably because cumulativeAddress was too large.");
        } else if (info.State != MEM_COMMIT) {
            assert(false, "Attempted to read unallocated memory.");
        } else if ((info.AllocationProtect & 0xC4) == 0) { // 0xC4 = PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY | PAGE_READWRITE
            assert(false, "Attempted to read unreadable memory.");
        } else {
            assert(false, "Failed to read memory for some as-yet unknown reason.");
        }
        return 0;
    }
    return cumulativeAddress + final_offset;
}

uintptr_t Memory::EnsureArrayCapacity(uintptr_t address, size_t size) {
    auto search = _allocatedArrays.find(address);
    if (search == _allocatedArrays.end()) {
		uintptr_t newAddress = (uintptr_t)VirtualAllocEx(_handle, 0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        _allocatedArrays[newAddress] = size;
        return newAddress;
    } else if (search->second < size) {
        VirtualFreeEx(_handle, (void*)address, 0, MEM_RELEASE);
		uintptr_t newAddress = (uintptr_t)VirtualAllocEx(_handle, 0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        search->second = size;
        return newAddress;
    } else {
        return NULL;
    }
}

void Memory::DebugPrint(const string& text) {
    OutputDebugStringA(text.c_str());
    cout << text;
    if (text[text.size()-1] != '\n') {
        OutputDebugStringA("\n");
        cout << endl;
    }
}

void Memory::DebugPrint(const wstring& text) {
    OutputDebugStringW(text.c_str());
    wcout << text;
    if (text[text.size()-1] != L'\n') {
        OutputDebugStringW(L"\n");
        wcout << endl;
    }
}
