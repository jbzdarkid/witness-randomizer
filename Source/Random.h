#pragma once

class Random
{
public:
    static void SetSeed(int seed);
    static int RandInt(int min, int max);

private:
    static uint32_t s_seed;
};
