#pragma once
#include <vector>
#include <tuple>

#ifndef NEGATIONS_CANCEL_NEGATIONS
#define NEGATIONS_CANCEL_NEGATIONS true
#endif

#ifndef SIMPLE_POLYOMINOS
#define SIMPLE_POLYOMINOS true
#endif

#ifndef DISABLE_CACHE
#define DISABLE_CACHE false
#endif

struct Region{};
class Puzzle;
struct Pos;
class Validator {
public:
    static void Validate(Puzzle& p);

private:
    static void RegionCheckNegations(Puzzle& p, const Region& r);
    static std::vector<Pos> RegionCheck(Puzzle& p, const Region& r);
};
