#include "gtest/gtest.h"
#include "Randomizer.h"

GTEST_TEST(SwapTests, Shipwreck) {
	Randomizer randomizer;
	int shipwreck = 0xAFB;
	int thEntry = 0x288C;
	int si1 = 0x00000022;
	int bu1 = 0x6;
	int td1 = 0x5D;

	randomizer.SwapPanels(si1, td1, SWAP_LINES);

}
