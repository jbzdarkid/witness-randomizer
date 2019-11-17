#pragma once
#include <typeinfo>

class Random
{
public:
    static void SetSeed(int seed);
    static int RandInt(int min, int max);

private:
    static int s_seed;
};
