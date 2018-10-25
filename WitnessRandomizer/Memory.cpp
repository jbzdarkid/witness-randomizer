#include "Memory.h"
#include <psapi.h>
#include <tlhelp32.h>
#include <iostream>

#undef PROCESSENTRY32
#undef Process32Next

Memory::Memory(const std::string& processName) {
	// First, get the handle of the process
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(entry);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	while (Process32Next(snapshot, &entry)) {
		if (processName == entry.szExeFile) {
			_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
			break;
		}
	}
	if (!_handle) {
		OutputDebugStringA("Process is not open!\n");
		exit(EXIT_FAILURE);
	}

	// Next, get the process base address
	DWORD numModules;
	std::vector<HMODULE> moduleList(1024);
	EnumProcessModulesEx(_handle, &moduleList[0], static_cast<DWORD>(moduleList.size()), &numModules, 3);

	std::string name(64, 0);
	for (DWORD i = 0; i < numModules / sizeof(HMODULE); i++) {
		GetModuleBaseNameA(_handle, moduleList[i], &name[0], sizeof(name));

		// TODO: Filling with 0s still yeilds name.size() == 64...
		if (strcmp(processName.c_str(), name.c_str()) == 0) {
			_baseAddress = (uintptr_t)moduleList[i];
			break;
		}
	}
	if (_baseAddress == 0) {
		OutputDebugStringA("Couldn't find base address!\n");
		exit(EXIT_FAILURE);
	}
}

Memory::~Memory() {
	CloseHandle(_handle);
}

// Private methods:

void Memory::ThrowError() {
	std::string message(256, '\0');
	FormatMessageA(4096, NULL, GetLastError(), 1024, &message[0], message.length(), NULL);
	OutputDebugStringA(message.c_str());
	exit(EXIT_FAILURE);
}

uintptr_t Memory::ComputeOffset(std::vector<int> offsets)
{
	// Leave off the last offset, since it will be either read/write, and may not be of type unitptr_t.
	int final_offset = offsets.back();
	offsets.pop_back();

	auto search = _computedOffsets.find(offsets);
	if (search != std::end(_computedOffsets)) {
		return search->second + final_offset;
	}

	uintptr_t cumulativeAddress = _baseAddress;
	for (int offset : offsets) {
		cumulativeAddress += offset;
		if (!ReadProcessMemory(_handle, (LPVOID)cumulativeAddress, &cumulativeAddress, sizeof(uintptr_t), NULL)) {
			ThrowError();
		}
	}
	_computedOffsets[offsets] = cumulativeAddress;
	return cumulativeAddress + final_offset;
}
