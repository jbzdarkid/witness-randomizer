#pragma once
#include "Memory.h"
#include <memory>
#include <set>

class Randomizer {
public:
    Randomizer(const std::shared_ptr<Memory>& memory);
    void Randomize();
    void RandomizeChallenge();

    void AdjustSpeed();
    void RandomizeLasers();
    void PreventSnipes();
    void SetDoubleRandomizerMode(bool val);
    void SetPreventDesertSkips(bool val);

    enum SWAP {
        // NONE = 0,
        TARGETS = 1,
        LINES = 2,
        AUDIO_NAMES = 4,
        COLORS = 8,
    };

private:
    int _lastRandomizedFrame = 1 << 30;
    void RandomizeTutorial();
    void RandomizeSymmetry();
    void RandomizeDesert();
    void RandomizeQuarry();
    void RandomizeTreehouse();
    void RandomizeKeep();
    void RandomizeShadows();
    void RandomizeTown();
    void RandomizeMonastery();
    void RandomizeBunker();
    void RandomizeJungle();
    void RandomizeSwamp();
    void RandomizeMountain();
    void RandomizeAudioLogs();

    void Randomize(std::vector<int>& panels, int flags);
    void SwapWithRandomPanel(int panel1, const std::vector<int>& possible_panels, int flags);
    void Shuffle(std::vector<int>& order, size_t startIndex, size_t endIndex);
    void RandomizeRange(std::vector<int> panels, int flags, size_t startIndex, size_t endIndex);
    void SwapPanels(int panel1, int panel2, int flags);
    void ReassignTargets(const std::vector<int>& panels, const std::vector<int>& order, std::vector<int> targets = {});
    void ReassignNames(const std::vector<int>& panels, const std::vector<int>& order);

    std::shared_ptr<Memory> _memory;
    bool _doubleRandomizer = false;
    bool _preventDesertSkips = false;
    std::set<int> _alreadySwapped;

    friend class SwapTests_Shipwreck_Test;
};
