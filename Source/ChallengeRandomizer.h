#pragma once

class ChallengeRandomizer {
public:
    ChallengeRandomizer(const std::shared_ptr<Memory>& memory, int seed);

private:
    void AdjustRng(int offset);
    std::shared_ptr<Memory> _memory;

    int RNG_ADDR;
    int RNG2_ADDR;
};
