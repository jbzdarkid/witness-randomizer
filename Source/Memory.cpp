#include "Memory.h"
#include "Memoryapi.h"
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
		MessageBox(GetActiveWindow(), L"Process not found in RAM. Please open The Witness and then try again.", NULL, MB_OK);
		throw std::exception("Unable to find process!");
	}

	// Next, get the process base address
	DWORD numModules;
	std::vector<HMODULE> moduleList(1024);
	EnumProcessModulesEx(_handle, &moduleList[0], static_cast<DWORD>(moduleList.size()), &numModules, 3);

	std::string name(64, '\0');
	for (DWORD i = 0; i < numModules / sizeof(HMODULE); i++) {
		int length = GetModuleBaseNameA(_handle, moduleList[i], &name[0], static_cast<DWORD>(name.size()));
		name.resize(length);
		if (processName == name) {
			_baseAddress = (uintptr_t)moduleList[i];
			break;
		}
	}
	if (_baseAddress == 0) {
		throw std::exception("Couldn't find the base process address!");
	}
}

Memory::~Memory() {
	CloseHandle(_handle);
}

int Memory::GetCurrentFrame()
{
	int SCRIPT_FRAMES;
	if (GLOBALS == 0x5B28C0) {
		SCRIPT_FRAMES = 0x5BE3B0;
	} else if (GLOBALS == 0x62D0A0) {
		SCRIPT_FRAMES = 0x63651C;
	} else {
		throw std::exception("Unknown value for Globals!");
	}
	return ReadData<int>({SCRIPT_FRAMES}, 1)[0];
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

void Memory::ThrowError(std::string message) {
	DWORD exitCode;
	GetExitCodeProcess(_handle, &exitCode);
	if (exitCode != STILL_ACTIVE) throw std::exception(message.c_str());
	std::ofstream file("errorlog.txt", std::ofstream::app);
	file << message << std::endl;
	message += "\nPlease close The Witness and try again. If the error persists, please report the issue on the Github Issues page.";
	MessageBox(GetActiveWindow(), std::wstring(message.begin(), message.end()).c_str(), NULL, MB_OK);
	throw std::exception(message.c_str());
}

void Memory::ThrowError(const std::vector<int>& offsets, bool rw_flag) {
	std::stringstream ss; ss << std::hex;
	if (offsets.size() == 4) {
		ss << "Error " << (rw_flag ? "writing" : "reading") << " 0x" << offsets[3] << " in panel 0x" << offsets[2] / 8;
		ThrowError(ss.str());
	}
	else if (offsets.size() == 3) {
		std::ofstream file("errorlog.txt", std::ofstream::app);
		file << "Error calculating offsets: ";
		for (int i : offsets) file << i << " ";
		file << std::endl;
		//Don't bother throwing an error since it will be thrown anyway by the caller of ComputeOffsets.
	}
	else {
		for (int i : offsets) ss << "0x" << i << " ";
		ThrowError("Unknown error: " + ss.str());
	}
}

void Memory::ThrowError() {
	std::string message(256, '\0');
	int length = FormatMessageA(4096, nullptr, GetLastError(), 1024, &message[0], static_cast<DWORD>(message.size()), nullptr);
	message.resize(length);
	ThrowError(message);
}

void* Memory::ComputeOffset(std::vector<int> offsets)
{
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
			if (!Read(reinterpret_cast<LPVOID>(cumulativeAddress), &computedAddress, sizeof(uintptr_t))) {
				ThrowError(offsets, false);
			}
			_computedAddresses[cumulativeAddress] = computedAddress;
		}

		cumulativeAddress = _computedAddresses[cumulativeAddress];
	}
	return reinterpret_cast<void*>(cumulativeAddress + final_offset);
}