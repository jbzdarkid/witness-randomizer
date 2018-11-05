#include <chrono>
#include "Random.h"

int Random::s_seed = time(nullptr); // Seed from the time in milliseconds

void Random::SetSeed(int seed) {
	s_seed = seed;
}

int Random::RandInt(int min, int max) {
	s_seed = (214013 * s_seed + 2531011) % 2147483648;
	if (min == max) return min;
	return (s_seed % (max - (min - 1))) + min;
}
