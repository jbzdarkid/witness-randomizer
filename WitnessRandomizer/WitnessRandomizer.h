#pragma once

int SWAP_NONE = 0x0;
int SWAP_TARGETS = 0x1;
int SWAP_LINES = 0x2;
int SWAP_STYLE = 0x4;

class WitnessRandomizer {
public:
	WitnessRandomizer();

	void Randomize(std::vector<int> &panels, int flags);
	void RandomizeRange(std::vector<int> &panels, int flags, size_t startIndex, size_t endIndex);
	void SwapPanels(int panel1, int panel2, int flags);
	void ReassignTargets(const std::vector<int>& panels, const std::vector<int>& order);

	template <class T>
	std::vector<T> ReadPanelData(int panel, int offset, int size) {
		return _memory.ReadData<T>({_globals, 0x18, panel*8, offset}, size);
	}

	template <class T>
	void WritePanelData(int panel, int offset, const std::vector<T>& data) {
		_memory.WriteData<T>({_globals, 0x18, panel*8, offset}, data);
	}

private:
	Memory _memory;
	int _globals = 0x5B28C0;
};