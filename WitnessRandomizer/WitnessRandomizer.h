#pragma once

int SWAP_TARGETS = 0x1;
int SWAP_PATHWAYS = 0x2;
int SWAP_STYLE = 0x40;
int SWAP_COLORS = 0x4;
int SWAP_TRACED = 0x80;


int SWAP_SIZE = 0x8;


class WitnessRandomizer {
public:

	WitnessRandomizer() = default;

	void Randomize(std::vector<int> panels, int flags);
	void SwapPanels(int panel1, int panel2, int flags);

private:
	void SwapPanelData(int panel1, int panel2, int finalOffset, int dataSize);

	Memory _memory = Memory("witness64_d3d11.exe");
};