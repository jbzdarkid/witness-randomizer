#pragma once
#include <memory>
#include <unordered_map>

#include "Puzzle.h"

class Memory;

class PuzzleSerializer {
public:
    PuzzleSerializer(const std::shared_ptr<Memory>& memory);
    Puzzle ReadPuzzle(int id);
    void WritePuzzle(const Puzzle& p, int id);

private:
    // @Bug: Blue and orange are swapped?
    enum Flags {
        IS_ENDPOINT =            0x1,
        IS_STARTPOINT =          0x2,
        HAS_NO_CONN =            0x8,
        HAS_DOT =               0x20,
        DOT_IS_BLUE =          0x100,
        DOT_IS_ORANGE =        0x200,
        DOT_IS_INVISIBLE =    0x1000,
        HAS_ONE_CONN =      0x100000,
        HAS_VERTI_CONN =    0x200000,
        HAS_HORIZ_CONN =    0x400000,
    };

    void ReadIntersections(Puzzle& p);
    void ReadExtras(Puzzle& p);
    void ReadDecorations(Puzzle& p, int id);
    void ReadSequence(Puzzle& p, int id);

    void WriteIntersections(const Puzzle& p);
    void WriteDots(const Puzzle& p);
    void WriteGaps(const Puzzle& p);
    void WriteEndpoints(const Puzzle& p);
    void WriteDecorations(const Puzzle& p, int id);
    void WriteSequence(const Puzzle& p, int id);

    std::tuple<int, int> loc_to_xy(const Puzzle& p, int location) const;
    int xy_to_loc(const Puzzle& p, int x, int y) const;
    // Decoration location
    std::tuple<int, int> dloc_to_xy(const Puzzle& p, int location) const;
    int xy_to_dloc(const Puzzle& p, int x, int y) const;
    // Grid coordinates
    std::tuple<float, float> xy_to_pos(const Puzzle& p, int x, int y) const;
    Cell::Dot FlagsToDot(int flags) const;
    // Iterate connection lists for another location which is connected to us; return that other location.
    int FindConnection(int location) const;

    std::shared_ptr<Memory> _memory;

    std::vector<float> _intersectionLocations;
    std::vector<int> _intersectionFlags;
    std::vector<int> _connectionsA;
    std::vector<int> _connectionsB;
    std::vector<std::tuple<int, int, int>> _endpointLocations;

    float MIN, MAX, WIDTH_INTERVAL, HEIGHT_INTERVAL, HORIZ_GAP_SIZE, VERTI_GAP_SIZE;
};
