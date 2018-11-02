#pragma once
#include <vector>
#include <map>
#include <windows.h>

#define GLOBALS 0x5B28C0
//#define GLOBALS 0x62A080

// https://github.com/erayarslan/WriteProcessMemory-Example
// http://stackoverflow.com/q/32798185
// http://stackoverflow.com/q/36018838
// http://stackoverflow.com/q/1387064
class Memory
{
public:
	Memory(const std::string& processName);
	~Memory();

	Memory(const Memory& memory) = delete;
	Memory& operator=(const Memory& other) = delete;


	template <class T>
	std::vector<T> ReadArray(int panel, int offset, int size) {
		return ReadData<T>({GLOBALS, 0x18, panel*8, offset, 0}, size);
	}

	template <class T>
	void WriteArray(int panel, int offset, const std::vector<T>& data) {
		WriteData<T>({GLOBALS, 0x18, panel*8, offset, 0}, data);
	}

	template <class T>
	std::vector<T> ReadPanelData(int panel, int offset, size_t size) {
		return ReadData<T>({GLOBALS, 0x18, panel*8, offset}, size);
	}

	template <class T>
	void WritePanelData(int panel, int offset, const std::vector<T>& data) {
		WriteData<T>({GLOBALS, 0x18, panel*8, offset}, data);
	}

	template<class T>
	std::vector<T> ReadData(const std::vector<int>& offsets, size_t numItems) {
		std::vector<T> data;
		data.resize(numItems);
		for (int i=0; i<5; i++) {
			if (ReadProcessMemory(_handle, ComputeOffset(offsets), &data[0], sizeof(T) * numItems, nullptr))
			{
				return data;
			}
		}
		ThrowError();
		return {};
	}

	template <class T>
	void WriteData(const std::vector<int>& offsets, const std::vector<T>& data) {
		for (int i=0; i<5; i++) {
			if (WriteProcessMemory(_handle, ComputeOffset(offsets), &data[0], sizeof(T) * data.size(), nullptr)) {
				return;
			}
		}
		ThrowError();
	}

private:

	void ThrowError();

	void* ComputeOffset(std::vector<int> offsets);

	std::map<uintptr_t, uintptr_t> _computedAddresses;
	uintptr_t _baseAddress = 0;
	HANDLE _handle = nullptr;
};