#pragma once
#include <memory>

class Memory;
class Randomizer2 {
public:
    Randomizer2(const std::shared_ptr<Memory>& memory);
    void Randomize();
    
private:
    std::shared_ptr<Memory> _memory;
};
