#include "Memory.h"
#include <psapi.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <cassert>

#undef PROCESSENTRY32
#undef Process32Next

Memory::Memory(const std::wstring& processName) : _processName(processName) {
}

Memory::~Memory() {
    if (_threadActive) {
        _threadActive = false;
        _thread.join();
    }
    if (_handle != nullptr) {
	    CloseHandle(_handle);
    }
}

void Memory::StartHeartbeat(HWND window, std::chrono::milliseconds beat) {
    if (_threadActive) return;
    _threadActive = true;
    _thread = std::thread([sharedThis = shared_from_this(), window, beat]{
        while (sharedThis->_threadActive) {
            sharedThis->Heartbeat(window);
            std::this_thread::sleep_for(beat);
        }
    });
    _thread.detach();
}

void Memory::Heartbeat(HWND window) {
    if (!_handle && !Initialize()) {
        // Couldn't initialize, definitely not running
        PostMessage(window, WM_COMMAND, HEARTBEAT, (LPARAM)ProcStatus::NotRunning);
        return;
    }

    DWORD exitCode = 0;
    assert(_handle);
    GetExitCodeProcess(_handle, &exitCode);
    if (exitCode != STILL_ACTIVE) {
        // Process has exited, clean up.
        _computedAddresses.clear();
        _handle = NULL;
        PostMessage(window, WM_COMMAND, HEARTBEAT, (LPARAM)ProcStatus::NotRunning);
        return;
    }

#if GLOBALS == 0x5B28C0
    int currentFrame = ReadData<int>({0x5BE3B0}, 1)[0];
#elif GLOBALS == 0x62D0A0
    int currentFrame = ReadData<int>({0x63954C}, 1)[0];
#endif
    int frameDelta = currentFrame - _previousFrame;
    _previousFrame = currentFrame;
    if (frameDelta < 0 && currentFrame < 250) {
        // Some addresses (e.g. Entity Manager) may get re-allocated on newgame.
        _computedAddresses.clear();
        PostMessage(window, WM_COMMAND, HEARTBEAT, (LPARAM)ProcStatus::NewGame);
        return;
    }

    // TODO: Some way to return ProcStatus::Randomized vs ProcStatus::NotRandomized vs ProcStatus::DeRandomized;

    PostMessage(window, WM_COMMAND, HEARTBEAT, (LPARAM)ProcStatus::Running);
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

void Memory::ThrowError() {
	std::wstring message(256, '\0');
    DWORD error = GetLastError();
	int length = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error, 1024, &message[0], static_cast<DWORD>(message.size()), nullptr);
	message.resize(length);
#ifndef NDEBUG
    MessageBox(NULL, message.c_str(), L"Please tell darkid about this", MB_OK);
#endif
}

void* Memory::ComputeOffset(std::vector<int> offsets) {
	// Leave off the last offset, since it will be either read/write, and may not be of type uintptr_t.
	int final_offset = offsets.back();
	offsets.pop_back();

	uintptr_t cumulativeAddress = _baseAddress;
	for (const int offset : offsets) {
		cumulativeAddress += offset;

		const auto search = _computedAddresses.find(cumulativeAddress);
		if (search == std::end(_computedAddresses)) {
			// If the address is not yet computed, then compute it.
			uintptr_t computedAddress = 0;
			if (bool result = !ReadProcessMemory(_handle, reinterpret_cast<LPVOID>(cumulativeAddress), &computedAddress, sizeof(uintptr_t), NULL)) {
				ThrowError();
			}
            if (computedAddress == 0) { // Attempting to dereference a nullptr
                ThrowError();
            }
			_computedAddresses[cumulativeAddress] = computedAddress;
		}

		cumulativeAddress = _computedAddresses[cumulativeAddress];
	}
	return reinterpret_cast<void*>(cumulativeAddress + final_offset);
}

uintptr_t Memory::Allocate(size_t bytes) {
    uintptr_t current = _freeMem;
    _freeMem += bytes;

    if (_freeMem > _freeMemEnd) {
        // If we don't have enough space at our current location, go allocate some more space.
        // Note that the remaining space in our current page is unused. Oh well.
        _freeMem = reinterpret_cast<uintptr_t>(::VirtualAllocEx(_handle, NULL, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
        _freeMemEnd = _freeMem + 0x1000;

        current = _freeMem;
        _freeMem += bytes;
        assert(_freeMem <= _freeMemEnd); // Don't allocate data > 0x1000 at a time. Duh.
    }

    return current;
}
