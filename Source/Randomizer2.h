#pragma once
#include <memory>
#include "PuzzleSerializer.h"

class Memory;
class Randomizer2 {
public:
    Randomizer2(const std::shared_ptr<Memory>& memory);
    void Randomize();
    void RandomizeTutorial();
    void RandomizeSymmetry();
    void RandomizeKeep();
    
private:
    Puzzle GetUniqueSolution(Puzzle& p);
    void SetGate(int panel, int X, int Y);
    void SetPos(int panel, float x, float y, float z);

    std::shared_ptr<Memory> _memory;
    PuzzleSerializer _serializer;
};
