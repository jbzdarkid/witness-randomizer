#include "Memory.h"
#include <psapi.h>
#include <tlhelp32.h>
#include <iostream>
#include <cassert>

#undef PROCESSENTRY32
#undef Process32Next

Memory::Memory() {
}

[[nodiscard]]
bool Memory::Initialize(const std::wstring& processName) {
	// First, get the handle of the process
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(entry);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	while (Process32NextW(snapshot, &entry)) {
		if (processName == entry.szExeFile) {
			_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
			break;
		}
	}
	if (!_handle) {
		std::cerr << "Couldn't find " << processName.c_str() << ", is it open?" << std::endl;
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
		if (processName == name) {
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

Memory::~Memory() {
    if (_handle != nullptr) {
	    CloseHandle(_handle);
    }
}

int Memory::GetCurrentFrame() {
	int SCRIPT_FRAMES;
	if (GLOBALS == 0x5B28C0) {
		SCRIPT_FRAMES = 0x5BE3B0;
	} else if (GLOBALS == 0x62D0A0) {
		SCRIPT_FRAMES = 0x63954C;
	} else {
        assert(false);
        return 0x7FFFFFFF;
	}
	return ReadData<int>({SCRIPT_FRAMES}, 1)[0];
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
	int length = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), 1024, &message[0], static_cast<DWORD>(message.size()), nullptr);
	message.resize(length);
#ifndef NDEBUG
    MessageBox(NULL, message.c_str(), L"Please tell darkid about this", MB_OK);
#endif
}

void* Memory::ComputeOffset(std::vector<int> offsets) {
	// Leave off the last offset, since it will be either read/write, and may not be of type unitptr_t.
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
                if (GetLastError() == ERROR_PARTIAL_COPY) {
                    int k = 1;
                }
				ThrowError();
			}
			_computedAddresses[cumulativeAddress] = computedAddress;
		}

		cumulativeAddress = _computedAddresses[cumulativeAddress];
	}
	return reinterpret_cast<void*>(cumulativeAddress + final_offset);
}
