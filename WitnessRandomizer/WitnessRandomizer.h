#pragma once

int SWAP_TARGETS = 0x1;
int SWAP_PATHWAYS = 0x2;
int SWAP_STYLE = 0x4;
int SWAP_COLORS = 0x8;
int SWAP_TRACED = 0x10;


int SWAP_SIZE = 0x20;


class WitnessRandomizer {
public:

	WitnessRandomizer();

	void Randomize(std::vector<int> panels, int flags);
	void SwapPanels(int panel1, int panel2, int flags);

private:
	void SwapPanelData(int panel1, int panel2, int finalOffset, int dataSize);

	Memory _memory;
};