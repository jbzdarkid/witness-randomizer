// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Random.h"
#include <time.h>

std::mt19937 Random::gen = std::mt19937((int)time(0));