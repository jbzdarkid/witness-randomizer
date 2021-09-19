/*
Things to do for V2:
- Better interface design. It's way too simplistic, take (some) notes from talos.
  - Seed: [   ] (Randomize)
  ?? Challenge

  - [] Prevent speedrun snipes // Shadows, Swamp, Town, Quarry stairs
  - [] Speed up various autoscrollers // Swamp platforms, Desert/Mountain elevators, Desert rotating panels

  (Additional required panels)
  - [] Desert 8
  - [] Pond 5
  - [] Both keep halves
  - [] Town lattice requires stars door // and stars door will be randomized

  (Debug version only)
  - [] Noclip
  - [] Noclip speed


- Really randomize panels. Sorted by ROI
  - Random with style
    - Tutorial
    - Mountain 1 orange, green, blue, purple
    - Mountain 2 multipanel
    - Mountain 3 pillars
    - Laser areas (Glass Factory, Symmetry, Quarry, Treehouse, Swamp, Keep pressure plates, Town 25 dots)
    - (low) Discarded panels
    - (low) Tutorials // Dots, Stones, Swamp

  - Keep Hedges become like hedges 4, intersection between path and panel
  - Keep Pressure plates: Random with style

  - No idea how to randomize:
    - Symmetry transparent
    - Desert
    - Shadows
    - Town (lattice, RGB area, snipes, triple)
    - Monastery
    - Jungle
    - Bunker
    - UTM
    - Mountain 2 rainbow
    - Challenge

- Any RNG rerolls should be based on previous seed so that everyone can go to next seed easily

- Stability. Duh. I need to clearly define the ownership between the randomizer and the game.

- Challenge should have some way to 'reroll every run'
- Challenge should not turn off after time limit?
- Challenge triangles should not turn off




*/






/*
 * Try to wire up both keep halves
 * Wire up both halves of symmetry laser
 * Turn off floating panel in desert
 * Try randomizing default-on for pitches & bunker


 * Speed up *everything* ? Or maybe we'll just stop using this setting entirely.

 * Add setting for "Don't reset the challenge seed on new challenge"
 * Don't rerandomize anything outside of challenge on re-click
 * Change re-randomization prevention?


 * BUGS:
 * Shipwreck vault is solved reversed? -> Not reversed, just "half", you can normally solve orange. Seems to need pattern name.
 * Tutorial sounds don't always play -> Unsure. Not controlled by pattern name.
 * Rainbow seems to be not copying background?
 ** Rainbow 1 <-> Green 3 (the poly one) worked
 ** Rainbow 2 <-> Treehouse Right Orange 1 didn't
 * FEATURES:
 * Start the game if it isn't running?
 * Randomize audio logs -- Hard, seem to be unloaded some times?
 * Swap sounds in jungle (along with panels) -- maybe impossible
 * Make orange 7 (all of oranges?) hard. Like big = hard. (See: HARD_MODE)
 * Try turning on first half of wire in shadows once tutorial is done
 * It might be possible to remove the texture on top of rainbow 5 (so that any panel can be placed there)
 * 20 challenges with 20 consecutive seeds
 * Random *rotation* of desert laser redirect?
*/
#include "Memory.h"
#include "Randomizer.h"
#include "ChallengeRandomizer.h"
#include "Panels.h"
#include "Random.h"
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>

template <class T>
int find(const std::vector<T> &data, T search, size_t startIndex = 0) {
    for (size_t i=startIndex ; i<data.size(); i++) {
        if (data[i] == search) return static_cast<int>(i);
    }
    std::cout << "Couldn't find " << search << " in data!" << std::endl;
    throw std::exception("Couldn't find value in data!");
}

std::vector<int> copyWithoutElements(const std::vector<int>& input, const std::vector<int>& toRemove) {
  std::vector<int> result;
  result.reserve(input.size());
  for (int val : input) {
      if (std::find(toRemove.begin(), toRemove.end(), val) == toRemove.end()) {
          result.push_back(val);
      }
  }
  return result;
}

Randomizer::Randomizer(const std::shared_ptr<Memory>& memory) : _memory(memory) {}

void Randomizer::Randomize() {
    _alreadySwapped.clear();

    // reveal_exit_hall - Prevent actually ending the game (EEE)
    _memory->AddSigScan({0x45, 0x8B, 0xF7, 0x48, 0x8B, 0x4D}, [&](int index){
        _memory->WriteData<byte>({index + 0x15}, {0xEB}); // jz -> jmp
    });

    // begin_endgame_1 - Prevent actually ending the game (Wonkavator)
    _memory->AddSigScan({0x83, 0x7C, 0x01, 0xD0, 0x04}, [&](int index){
        if (GLOBALS == 0x5B28C0) { // Version differences.
            index += 0x75;
        } else if (GLOBALS == 0x62D0A0) {
            index += 0x86;
        }
        _memory->WriteData<byte>({index}, {0xEB}); // jz -> jmp
    });
    // Sig scans will be run during challenge randomization.

    // Seed challenge first for future-proofing
    RandomizeChallenge();

    // Content swaps -- must happen before squarePanels
    if (_doubleRandomizer) {
        // Prevent the fourth UTM perspective puzzle from being shuffled, as it
        // is unknown whether any of the other puzzles in these pools can be
        // solved there after using Sigma's randomizer.
        // 0x288AA UTM Perspective 4
        _alreadySwapped.push_back(0x288AA);
        SwapWithRandomPanel(0x17CC4, millElevatorControlOptions, SWAP::LINES | SWAP::COLORS); // Mill Elevator Control
        // The pool that Tutorial Back Left is in has some panels that may not
        // be solveable in the down position after using Sigma's randomizer. We
        // do not want to swap any such panel with Tutorial Back Left, because
        // that would make it impossible to exit Tutorial.
        std::vector<int> tutorialBackLeftDoubleMode = copyWithoutElements(tutorialBackLeftOptions, {
            0x00070, // Symmetry Island Fading Lines 5
            0x01E5A, // Mill Entry Door Left
            0x00072, // Symmetry Island Fading 3
            0x00076, // Symmetry Island Fading 7
            0x3C125, // Mill Control Room Extra Panel
            0x09E85, // Tunnels Town Shortcut
        });
        SwapWithRandomPanel(0x0A3B5, tutorialBackLeftDoubleMode, SWAP::LINES | SWAP::COLORS); // Tutorial Back Left
        // Shuffle the UTM elevator controls amongst themselves.
        Shuffle(utmElevatorControls, SWAP::LINES | SWAP::COLORS);
        // The four pivot panels in Treehouse must be solveable in the up, left,
        // and right positions. However, the other panels in the pools those
        // panels are found in may not be solveable in all three directions
        // after using Sigma's randomizer. For safety, we will only swap the
        // pivot panels amongst themselves.
        Shuffle(treehousePivotSet, SWAP::LINES | SWAP::COLORS);
        // This will additionally shuffle the UTM perspective puzzles amongst
        // themselves.
        Shuffle(utmPerspectiveSet, SWAP::LINES | SWAP::COLORS);
        // In order to prevent a situation where access to a Symmetry Laser
        // Yellow is blocked by its corresponding Blue panel, we will only
        // shuffle these six panels amongst themselves.
        Shuffle(symmetryLaserYellows, SWAP::LINES | SWAP::COLORS);
        Shuffle(symmetryLaserBlues, SWAP::LINES | SWAP::COLORS);
        // Many puzzles either crash the game or do not solve properly when
        // swapped with Swamp Entry. To make things simpler, we will just not
        // shuffle it.
        std::vector<int> squarePanelsDoubleMode = copyWithoutElements(squarePanels, doubleModeBannedSquarePanels);
        std::vector<int> quarryLaserOptions = copyWithoutElements(squarePanelsDoubleMode, quarryLaserBanned);
        quarryLaserOptions.push_back(0x03612);
        SwapWithRandomPanel(0x03612, quarryLaserOptions, SWAP::LINES | SWAP::COLORS); // Quarry Laser

        Shuffle(squarePanelsDoubleMode, SWAP::LINES | SWAP::COLORS);
    } else {
        Shuffle(utmElevatorControls, SWAP::LINES | SWAP::COLORS);
        Shuffle(leftForwardRightPanelsSetOne, SWAP::LINES | SWAP::COLORS);
        Shuffle(leftForwardRightPanelsSetTwo, SWAP::LINES | SWAP::COLORS);
        
        SwapWithRandomPanel(0x17D02, townWindmillControlOptions, SWAP::LINES | SWAP::COLORS); // Town Windmill Control
        SwapWithRandomPanel(0x17CC4, millElevatorControlOptions, SWAP::LINES | SWAP::COLORS); // Mill Elevator Control
        SwapWithRandomPanel(0x288AA, utmPerspectiveFourOptions, SWAP::LINES | SWAP::COLORS); // UTM Perspective 4
        SwapWithRandomPanel(0x0A3B5, tutorialBackLeftOptions, SWAP::LINES | SWAP::COLORS); // Tutorial Back Left
        SwapWithRandomPanel(0x33AF5, mountainBlueOneOptions, SWAP::LINES | SWAP::COLORS); // Mountain Blue 1
        SwapWithRandomPanel(0x33AF7, mountainBlueTwoOptions, SWAP::LINES | SWAP::COLORS); // Mountain Blue 2

        std::vector<int> symTwoOptions = copyWithoutElements(squarePanels, symmetryDoorTwoBanned);
        symTwoOptions.push_back(0x1C349);
        SwapWithRandomPanel(0x1C349, symTwoOptions, SWAP::LINES | SWAP::COLORS); // Symmetry Door 2

        std::vector<int> quarryLaserOptions = copyWithoutElements(squarePanels, quarryLaserBanned);
        quarryLaserOptions.push_back(0x03612);
        SwapWithRandomPanel(0x03612, quarryLaserOptions, SWAP::LINES | SWAP::COLORS); // Quarry Laser

        Shuffle(squarePanels, SWAP::LINES | SWAP::COLORS);
    }

    // Individual area modifications
    RandomizeTutorial();
    RandomizeDesert();
    RandomizeQuarry();
    RandomizeTreehouse();
    RandomizeKeep();
    RandomizeShadows();
    RandomizeMonastery();
    RandomizeBunker();
    RandomizeJungle();
    RandomizeSwamp();
    RandomizeMountain();
    RandomizeTown();
    RandomizeSymmetry();
    // RandomizeAudioLogs();
}

void Randomizer::AdjustSpeed() {
    // Desert Surface Final Control
    _memory->WriteEntityData<float>(0x09F95, OPEN_RATE, {0.04f}); // 4x
    // Swamp Sliding Bridge
    _memory->WriteEntityData<float>(0x0061A, OPEN_RATE, {0.1f}); // 4x
    // Mountain 2 Elevator
    _memory->WriteEntityData<float>(0x09EEC, OPEN_RATE, {0.075f}); // 3x
}

void Randomizer::RandomizeLasers() {
    Shuffle(lasers, SWAP::TARGETS);
    // Read the target of keep front laser, and write it to keep back laser.
    std::vector<int> keepFrontLaserTarget = _memory->ReadEntityData<int>(0x0360E, TARGET, 1);
    _memory->WriteEntityData<int>(0x03317, TARGET, keepFrontLaserTarget);
}

void Randomizer::PreventSnipes()
{
    // Distance-gate swamp snipe 1 to prevent RNG swamp snipe
    _memory->WriteEntityData<float>(0x17C05, MAX_BROADCAST_DISTANCE, {15.0});
    // Distance-gate shadows laser to prevent sniping through the bars
    _memory->WriteEntityData<float>(0x19650, MAX_BROADCAST_DISTANCE, {2.5});
}

void Randomizer::SetDoubleRandomizerMode(bool val)
{
    _doubleRandomizer = val;
}

// Private methods
void Randomizer::RandomizeTutorial() {
    // Disable tutorial cursor speed modifications (not working?)
    _memory->WriteEntityData<float>(0x00295, CURSOR_SPEED_SCALE, {1.0});
    _memory->WriteEntityData<float>(0x0C373, CURSOR_SPEED_SCALE, {1.0});
    _memory->WriteEntityData<float>(0x00293, CURSOR_SPEED_SCALE, {1.0});
    _memory->WriteEntityData<float>(0x002C2, CURSOR_SPEED_SCALE, {1.0});
}

void Randomizer::RandomizeSymmetry() {
    std::vector<int> randomOrder(transparent.size(), 0);
    std::iota(randomOrder.begin(), randomOrder.end(), 0);
    ReorderRange(randomOrder, 1, 5);
    ReassignTargets(transparent, randomOrder);
}

void Randomizer::RandomizeDesert() {
    Shuffle(desertPanels, SWAP::LINES);
    Shuffle(desertPanelsWide, SWAP::LINES);

    // Turn off desert surface 8
    _memory->WriteEntityData<float>(0x09F94, POWER, {0.0, 0.0});
    // Turn off desert flood final
    _memory->WriteEntityData<float>(0x18076, POWER, {0.0, 0.0});
    // Change desert floating target to desert flood final
    _memory->WriteEntityData<int>(0x17ECA, TARGET, {0x18077});
}

void Randomizer::RandomizeQuarry() {
}

void Randomizer::RandomizeTreehouse() {
    // Ensure that whatever pivot panels we have are flagged as "pivotable"
    int panelFlags = _memory->ReadEntityData<int>(0x17DD1, STYLE_FLAGS, 1)[0];
    _memory->WriteEntityData<int>(0x17DD1, STYLE_FLAGS, {panelFlags | 0x8000});
    panelFlags = _memory->ReadEntityData<int>(0x17CE3, STYLE_FLAGS, 1)[0];
    _memory->WriteEntityData<int>(0x17CE3, STYLE_FLAGS, {panelFlags | 0x8000});
    panelFlags = _memory->ReadEntityData<int>(0x17DB7, STYLE_FLAGS, 1)[0];
    _memory->WriteEntityData<int>(0x17DB7, STYLE_FLAGS, {panelFlags | 0x8000});
    panelFlags = _memory->ReadEntityData<int>(0x17E52, STYLE_FLAGS, 1)[0];
    _memory->WriteEntityData<int>(0x17E52, STYLE_FLAGS, {panelFlags | 0x8000});
}

void Randomizer::RandomizeKeep() {
}

void Randomizer::RandomizeShadows() {
    // Change the shadows tutorial cable to only activate avoid
    _memory->WriteEntityData<int>(0x319A8, CABLE_TARGET_2, {0});
    // Change shadows avoid 8 to power shadows follow
    _memory->WriteEntityData<int>(0x1972F, TARGET, {0x1C34C});

    std::vector<int> randomOrder(shadowsPanels.size(), 0);
    std::iota(randomOrder.begin(), randomOrder.end(), 0);
    ReorderRange(randomOrder, 0, 8); // Tutorial
    ReorderRange(randomOrder, 8, 16); // Avoid
    ReorderRange(randomOrder, 16, 21); // Follow
    ReassignTargets(shadowsPanels, randomOrder);
    // Turn off original starting panel
    _memory->WriteEntityData<float>(shadowsPanels[0], POWER, {0.0f, 0.0f});
    // Turn on new starting panel
    _memory->WriteEntityData<float>(shadowsPanels[randomOrder[0]], POWER, {1.0f, 1.0f});
}

void Randomizer::RandomizeTown() {
    // @Hack...? To open the gate at the end
    std::vector<int> randomOrder(orchard.size() + 1, 0);
    std::iota(randomOrder.begin(), randomOrder.end(), 0);
    ReorderRange(randomOrder, 1, 5);
    // Ensure that we open the gate before the final puzzle (by swapping)
    int panel3Index = find(randomOrder, 3);
    int panel4Index = find(randomOrder, 4);
    randomOrder[min(panel3Index, panel4Index)] = 3;
    randomOrder[max(panel3Index, panel4Index)] = 4;
    ReassignTargets(orchard, randomOrder);
}

void Randomizer::RandomizeMonastery() {
    std::vector<int> randomOrder(monasteryPanels.size(), 0);
    std::iota(randomOrder.begin(), randomOrder.end(), 0);
    ReorderRange(randomOrder, 3, 9); // Outer 2 & 3, Inner 1-4
    ReassignTargets(monasteryPanels, randomOrder);
}

void Randomizer::RandomizeBunker() {
    std::vector<int> randomOrder(bunkerPanels.size(), 0);
    std::iota(randomOrder.begin(), randomOrder.end(), 0);
    // Randomize Tutorial 2-Advanced Tutorial 4 + Glass 1
    // Tutorial 1 cannot be randomized, since no other panel can start on
    // Glass 1 will become door + glass 1, due to the targetting system
    ReorderRange(randomOrder, 1, 10);
    // Randomize Glass 1-3 into everything after the door/glass 1
    const size_t glass1Index = find(randomOrder, 9);
    ReorderRange(randomOrder, glass1Index + 1, 12);
    ReassignTargets(bunkerPanels, randomOrder);
}

void Randomizer::RandomizeJungle() {
    std::vector<int> randomOrder(junglePanels.size(), 0);
    std::iota(randomOrder.begin(), randomOrder.end(), 0);
    // Waves 1 cannot be randomized, since no other panel can start on
    ReorderRange(randomOrder, 1, 7); // Waves 2-7
    ReorderRange(randomOrder, 8, 13); // Pitches 1-6
    ReassignTargets(junglePanels, randomOrder);

    // Fix the wall's target to point back to the cable, and the cable to point to the pitches panel.
    // auto wallTarget = _memory->ReadPanelData<int>(junglePanels[7], TARGET, 1);
    // _memory->WritePanelData<int>(junglePanels[7], TARGET, {0x3C113});
    // _memory->WritePanelData<int>(0x3C112, CABLE_TARGET_1, wallTarget);
}

void Randomizer::RandomizeSwamp() {
}

void Randomizer::RandomizeMountain() {
    // Randomize multipanel
    Shuffle(mountainMultipanel, SWAP::LINES | SWAP::COLORS);
    // With Sigma's randomizer, split solution metapuzzles may become impossible
    // if the interior panels are shuffled, so we will just not shuffle them in
    // double randomizer mode.
    if (!_doubleRandomizer) {
        Shuffle(mountainMetaPanels, SWAP::LINES | SWAP::COLORS);
    }

    // Randomize final pillars order
    std::vector<int> targets = {pillars[0] + 1};
    for (const int pillar : pillars) {
        int target = _memory->ReadEntityData<int>(pillar, TARGET, 1)[0];
        targets.push_back(target);
    }
    targets[5] = pillars[5] + 1;

    std::vector<int> randomOrder(pillars.size(), 0);
    std::iota(randomOrder.begin(), randomOrder.end(), 0);
    ReorderRange(randomOrder, 0, 4); // Left Pillars 1-4
    ReorderRange(randomOrder, 5, 9); // Right Pillars 1-4
    ReassignTargets(pillars, randomOrder, targets);
    // Turn off original starting panels
    _memory->WriteEntityData<float>(pillars[0], POWER, {0.0f, 0.0f});
    _memory->WriteEntityData<float>(pillars[5], POWER, {0.0f, 0.0f});
    // Turn on new starting panels
    _memory->WriteEntityData<float>(pillars[randomOrder[0]], POWER, {1.0f, 1.0f});
    _memory->WriteEntityData<float>(pillars[randomOrder[5]], POWER, {1.0f, 1.0f});
}

void Randomizer::RandomizeChallenge() {
    ChallengeRandomizer cr(_memory, Random::RandInt(1, 0x7FFFFFFF)); // 0 will trigger an "RNG not initialized" block
    for (int panel : challengePanels) {
        _memory->WriteEntityData<int>(panel, POWER_OFF_ON_FAIL, {0});
    }
}

void Randomizer::RandomizeAudioLogs() {
    std::vector<int> randomOrder(audiologs.size(), 0);
    std::iota(randomOrder.begin(), randomOrder.end(), 0);
    ReorderRange(randomOrder, 0, audiologs.size());
    ReassignNames(audiologs, randomOrder);
}

void Randomizer::Shuffle(std::vector<int> panels, int flags) {
    for (size_t i = panels.size() - 1; i > 0; i--) {
        const int target = Random::RandInt(0, static_cast<int>(i));
        if (i != target) {
            // std::cout << "Swapping panels " << std::hex << panels[i] << " and " << std::hex << panels[target] << std::endl;
            SwapPanels(panels[i], panels[target], flags);
            std::swap(panels[i], panels[target]); // Panel indices in the array
        }
    }
}

// Range is [start, end)
void Randomizer::ReorderRange(std::vector<int>& panels, size_t startIndex, size_t endIndex) {
    if (panels.size() == 0) return;
    if (startIndex >= endIndex) return;
    if (endIndex >= panels.size()) endIndex = static_cast<int>(panels.size());
    for (size_t i = endIndex-1; i > startIndex; i--) {
        const int target = Random::RandInt(static_cast<int>(startIndex), static_cast<int>(i));
        if (i != target) {
            std::swap(panels[i], panels[target]);
        }
    }
}

void Randomizer::SwapWithRandomPanel(int panel1, const std::vector<int>& possible_panels, int flags) {
    std::vector<int> filtered = copyWithoutElements(possible_panels, _alreadySwapped);
    const int target = Random::RandInt(0, static_cast<int>(filtered.size()) - 1);
    const int toSwap = filtered[target];
    if (panel1 != toSwap) {
        SwapPanels(panel1, toSwap, flags);
    }
    _alreadySwapped.push_back(toSwap);
}

void Randomizer::SwapPanels(int panel1, int panel2, int flags) {
    std::map<int, int> offsets;

    if (flags & SWAP::TARGETS) {
        offsets[TARGET] = sizeof(int);
    }
    if (flags & SWAP::AUDIO_NAMES) {
        offsets[AUDIO_LOG_NAME] = sizeof(void*);
    }
    if (flags & SWAP::COLORS) {
        offsets[PATH_COLOR] = 16;
        offsets[REFLECTION_PATH_COLOR] = 16;
        offsets[DOT_COLOR] = 16;
        offsets[ACTIVE_COLOR] = 16;
        offsets[BACKGROUND_REGION_COLOR] = 12; // Not copying alpha to preserve transparency.
        offsets[SUCCESS_COLOR_A] = 16;
        offsets[SUCCESS_COLOR_B] = 16;
        offsets[STROBE_COLOR_A] = 16;
        offsets[STROBE_COLOR_B] = 16;
        offsets[ERROR_COLOR] = 16;
        offsets[PATTERN_POINT_COLOR] = 16;
        offsets[PATTERN_POINT_COLOR_A] = 16;
        offsets[PATTERN_POINT_COLOR_B] = 16;
        offsets[SYMBOL_A] = 16;
        offsets[SYMBOL_B] = 16;
        offsets[SYMBOL_C] = 16;
        offsets[SYMBOL_D] = 16;
        offsets[SYMBOL_E] = 16;
        offsets[PUSH_SYMBOL_COLORS] = sizeof(int);
        offsets[OUTER_BACKGROUND] = 16;
        offsets[OUTER_BACKGROUND_MODE] = sizeof(int);
        // These two control the "burn intensity", but I can't swap out burn marks in new ver, so they should probably stay tied to each frame.
        // offsets[SPECULAR_ADD] = sizeof(float);
        // offsets[SPECULAR_POWER] = sizeof(int);
        offsets[NUM_COLORED_REGIONS] = sizeof(int);
        offsets[COLORED_REGIONS] = sizeof(void*);
    }
    if (flags & SWAP::LINES) {
        offsets[TRACED_EDGES] = 16;
        offsets[AUDIO_PREFIX] = sizeof(void*);
//        offsets[IS_CYLINDER] = sizeof(int);
//        offsets[CYLINDER_Z0] = sizeof(float);
//        offsets[CYLINDER_Z1] = sizeof(float);
//        offsets[CYLINDER_RADIUS] = sizeof(float);
        offsets[PATH_WIDTH_SCALE] = sizeof(float);
        offsets[STARTPOINT_SCALE] = sizeof(float);
        offsets[NUM_DOTS] = sizeof(int);
        offsets[NUM_CONNECTIONS] = sizeof(int);
        offsets[DOT_POSITIONS] = sizeof(void*);
        offsets[DOT_FLAGS] = sizeof(void*);
        offsets[DOT_CONNECTION_A] = sizeof(void*);
        offsets[DOT_CONNECTION_B] = sizeof(void*);
        offsets[DECORATIONS] = sizeof(void*);
        offsets[DECORATION_FLAGS] = sizeof(void*);
        offsets[DECORATION_COLORS] = sizeof(void*);
        offsets[NUM_DECORATIONS] = sizeof(int);
        offsets[REFLECTION_DATA] = sizeof(void*);
        offsets[GRID_SIZE_X] = sizeof(int);
        offsets[GRID_SIZE_Y] = sizeof(int);
        offsets[STYLE_FLAGS] = sizeof(int);
        offsets[SEQUENCE_LEN] = sizeof(int);
        offsets[SEQUENCE] = sizeof(void*);
        offsets[DOT_SEQUENCE_LEN] = sizeof(int);
        offsets[DOT_SEQUENCE] = sizeof(void*);
        offsets[DOT_SEQUENCE_LEN_REFLECTION] = sizeof(int);
        offsets[DOT_SEQUENCE_REFLECTION] = sizeof(void*);
        offsets[PANEL_TARGET] = sizeof(void*);
        offsets[SPECULAR_TEXTURE] = sizeof(void*);
    }

    for (auto const& [offset, size] : offsets) {
        std::vector<byte> panel1data = _memory->ReadEntityData<byte>(panel1, offset, size);
        std::vector<byte> panel2data = _memory->ReadEntityData<byte>(panel2, offset, size);
        _memory->WriteEntityData<byte>(panel2, offset, panel1data);
        _memory->WriteEntityData<byte>(panel1, offset, panel2data);
    }
}

void Randomizer::ReassignTargets(const std::vector<int>& panels, const std::vector<int>& order, std::vector<int> targets) {
    if (targets.empty()) {
        // This list is offset by 1, so the target of the Nth panel is in position N (aka the N+1th element)
        // The first panel may not have a wire to power it, so we use the panel ID itself.
        targets = {panels[0] + 1};
        for (const int panel : panels) {
            int target = _memory->ReadEntityData<int>(panel, TARGET, 1)[0];
            targets.push_back(target);
        }
    }

    for (size_t i=0; i<order.size() - 1; i++) {
        // Set the target of order[i] to order[i+1], using the "real" target as determined above.
        const int panelTarget = targets[order[i+1]];
        _memory->WriteEntityData<int>(panels[order[i]], TARGET, {panelTarget});
    }
}

void Randomizer::ReassignNames(const std::vector<int>& panels, const std::vector<int>& order) {
    std::vector<int64_t> names;
    for (const int panel : panels) {
        names.push_back(_memory->ReadEntityData<int64_t>(panel, AUDIO_LOG_NAME, 1)[0]);
    }

    for (int i=0; i<panels.size(); i++) {
        _memory->WriteEntityData<int64_t>(panels[i], AUDIO_LOG_NAME, {names[order[i]]});
    }
}
