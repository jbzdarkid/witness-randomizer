#pragma once
#include <memory>

class Memory;
class Randomizer2 {
public:
    Randomizer2(const std::shared_ptr<Memory>& memory);
    void Randomize();
    void RandomizeKeep();
    
private:
    void SetGate(int panel, int X, int Y);

    std::shared_ptr<Memory> _memory;
};
