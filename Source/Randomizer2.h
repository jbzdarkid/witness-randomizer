#pragma once
#include "PuzzleSerializer.h"

class Randomizer2 {
public:
    Randomizer2(const PuzzleSerializer& serializer);
    void Randomize();
    void RandomizeTutorial();
    void RandomizeGlassFactory();
    void RandomizeSymmetryIsland();
    void RandomizeKeep();
    
private:
    Puzzle GetUniqueSolution(Puzzle& p);
    void SetGate(int panel, int X, int Y);
    void SetPos(int panel, float x, float y, float z);

    PuzzleSerializer _serializer;
};
