#pragma once
#include <vector>
#include <map>
#include <windows.h>

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