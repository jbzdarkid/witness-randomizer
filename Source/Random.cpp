#include <chrono>
#include "Random.h"

uint32_t Random::s_seed = static_cast<int>(time(nullptr)); // Seed from the time in milliseconds

void Random::SetSeed(int seed) {
    s_seed = seed;
}

// Returns a random integer in [min, max]
int Random::RandInt(int min, int max) {
    s_seed = (214013 * s_seed + 2531011); // Implicit unsigned integer overflow
    int32_t maskedSeed = ((s_seed >> 16) & 0x7fff); // Only use bits 16-30
    return (maskedSeed % (max - min + 1)) + min;
}
