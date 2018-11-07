#include <chrono>
#include "Random.h"

int Random::s_seed = static_cast<int>(time(nullptr)); // Seed from the time in milliseconds

void Random::SetSeed(int seed) {
	s_seed = seed;
}

// Returns a random integer in [min, max]
int Random::RandInt(int min, int max) {
	s_seed = (214013 * s_seed + 2531011) % 2147483648;
	return (s_seed % (max - min + 1)) + min;
}
