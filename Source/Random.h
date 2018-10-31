#pragma once

static int s_seed;

class Random
{
public:
	static void SetSeed(int seed) {
		s_seed = seed;
	}

	static int RandInt(int min, int max) {
		s_seed = (214013 * s_seed + 2531011) % 2147483648;
		if (min == max) return min;
		return (s_seed % (max - (min - 1))) + min;
	}
};
