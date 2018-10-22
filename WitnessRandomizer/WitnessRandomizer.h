#pragma once

class WitnessRandomizer {
public:
	int RANDOMIZE_TARGETS = 0x1;
	int RANDOMIZE_DATA = 0x2;
		
	WitnessRandomizer() = default;

	void Randomize(std::vector<int> panels, int flags);

private:
	void SwapPanelData(int panel1, int panel2, int finalOffset, int dataSize);

	Memory _memory = Memory("witness64_d3d11.exe");
};