#include "gtest/gtest.h"
#include "Randomizer.h"

GTEST_TEST(SwapTests, Shipwreck) {
	Randomizer randomizer;
	int shipwreck = 0xAFB;
	int thEntry = 0x288C;
	int si1 = 0x00000022;
	int bu1 = 0x6;
	int td1 = 0x5D;
	int ypp = 0x33EA;
	int ramp_activation_shapers = 0x21D5;
	int mill_upper_5 = 0x146C;
	int mill_entry_left = 0x1E5A;
	int mill_upper_7 = 0x03686;

	randomizer.SwapPanels(ypp, mill_upper_7, Randomizer::SWAP::LINES);

}
