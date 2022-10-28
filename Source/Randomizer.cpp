#include "pch.h"
#include "Randomizer.h"
#include "PanelOffsets.h"
#include "Panel.h"

using namespace std;

Randomizer::Randomizer(const shared_ptr<Memory>& memory) {
    _memory = memory;

    _memory->AddSigScan({0x74, 0x41, 0x48, 0x85, 0xC0, 0x74, 0x04, 0x48, 0x8B, 0x48, 0x10}, [&](int64_t offset, int index, const vector<uint8_t>& data) {
        _globals = Memory::ReadStaticInt(offset, index + 0x14, data);
    });

    size_t failedScans = _memory->ExecuteSigScans();
    assert(failedScans == 0);
}

vector<Traced_Edge> Randomizer::ReadTracedEdges(int panel) {
    vector<Traced_Edge> edgeData;

    int numEdges = ReadPanelData<int>(panel, TRACED_EDGES_COUNT);
    if (numEdges > 0) {
        // Explicitly computing this as an intermediate, since the edges array might have re-allocated.
        auto edgeDataPtr = ReadPanelData<int64_t>(panel, TRACED_EDGES_DATA);
        if (edgeDataPtr != 0) {
            static_assert(sizeof(Traced_Edge) == 0x34);
            edgeData = _memory->ReadAbsoluteData<Traced_Edge>({edgeDataPtr}, numEdges);
        }
    }

    return edgeData;
}

// These coordinates are laid out in a 3x3 grid with these positions:
//   012
//   345
//   678
vector<int> CharToCoords(char ch) {
    switch (ch) {
        case 'a': return { 6,0,2,8,5,3 };
        case 'b': return { 0,1,4,7,6,3,0,3,4 };
        case 'c': return { 2,0,6,8 };
        case 'd': return { 0,1,5,7,6,0 };
        case 'e': return { 2,0,3,5,3,6,8 };
        case 'f': return { 1,0,6,3,4 };
        case 'g': return { 2,0,6,8,5,4 };
        case 'h': return { 0,6,3,5,2,8 };
        case 'i': return { 0,2,1,7,6,8 };
        case 'j': return { 0,2,1,7,6,3 };
        case 'k': return { 0,6,3,2,3,8 };
        case 'l': return { 0,6,8 };
        case 'm': return { 6,0,4,2,8 };
        case 'n': return { 6,0,8,2 };
        case 'o': return { 0,2,8,6,0 };
        case 'p': return { 6,0,2,5,3 };
        case 'q': return { 8,6,0,2,8,4 };
        case 'r': return { 6,0,2,5,3,8 };
        case 's': return { 2,0,3,5,8,6 };
        case 't': return { 0,2,1,7 };
        case 'u': return { 0,6,8,2 };
        case 'v': return { 0,3,7,5,2 };
        case 'w': return { 0,6,4,8,2 };
        case 'x': return { 0,8,4,2,6 };
        case 'y': return { 0,4,7,4,2 };
        case 'z': return { 0,2,6,8 };
        case '0': return { 0,2,8,6,0 };
        case '1': return { 0,1,7,6,8 };
        case '2': return { 0,2,5,3,6,8 };
        case '3': return { 0,2,5,3,5,8,6 };
        case '4': return { 0,3,5,2,8 };
        case '5': return { 2,0,3,5,8,6 };
        case '6': return { 2,0,6,8,5,3 };
        case '7': return { 0,2,7 };
        case '8': return { 0,2,8,6,0,3,5 };
        case '9': return { 6,8,2,0,3,5 };
        case '!': return { 1,4,7 };
        case ' ': return { };
        case '.': return { 7 };
        case '-': return { 3,5 };
        case '_': return { 6,8 };
        default: assert(false);
    }

    return {};
}

void Randomizer::ClearPanel(int panel) {
    WritePanelData<int>(panel, NUM_DOTS, 0);
    WritePanelData<int>(panel, NUM_CONNECTIONS, 0);
}

void Randomizer::DrawLine(int panel, const vector<float>& coords) {
    int numIntersections = ReadPanelData<int>(panel, NUM_DOTS);
    vector<float> intersections = ReadPanelData<float>(panel, DOT_POSITIONS, numIntersections * 2);
    vector<int> intersectionFlags = ReadPanelData<int>(panel, DOT_FLAGS, numIntersections);

    int numConnections = ReadPanelData<int>(panel, NUM_CONNECTIONS);
    vector<int> connectionsA = ReadPanelData<int>(panel, DOT_CONNECTION_A, numConnections);
    vector<int> connectionsB = ReadPanelData<int>(panel, DOT_CONNECTION_B, numConnections);

    intersections.emplace_back(coords[0]);
    intersectionFlags.emplace_back(IntersectionFlags::STARTPOINT | IntersectionFlags::INTERSECTION);

    for (int i = 1; i < coords.size(); i++) {
        intersections.emplace_back(coords[i]);
        if (i % 2 == 0) {
            intersectionFlags.emplace_back(i == coords.size() - 2 ? IntersectionFlags::ENDPOINT | IntersectionFlags::INTERSECTION : 0);
            connectionsA.emplace_back(static_cast<int>(intersectionFlags.size()) - 2);
            connectionsB.emplace_back(static_cast<int>(intersectionFlags.size()) - 1);
        }
    }

    // For safety, ensure that arrays have contents before updating their size.
    WritePanelData<float>(panel, DOT_POSITIONS, intersections);
    WritePanelData<int>(panel, DOT_FLAGS, intersectionFlags);
    WritePanelData<int>(panel, NUM_DOTS, static_cast<int>(intersectionFlags.size()));
    WritePanelData<int>(panel, DOT_CONNECTION_A, connectionsA);
    WritePanelData<int>(panel, DOT_CONNECTION_B, connectionsB);
    WritePanelData<int>(panel, NUM_CONNECTIONS, static_cast<int>(connectionsA.size()));
}

//void Randomizer::DrawText(int panel, const string& text, float left, float top, float right, float bottom) {
/*
    enum HAlign : int {
        Left    = 0x1,
        Center  = 0x2,
        Right   = 0x4,
    };
    enum VAlign : int {
        Top     = 0x8,
        Center  = 0x16,
        Bottom  = 0x32,
    };

*/

void Randomizer::DrawText(int panel, const std::string& text, float x, float y, int alignment, float textSize) {
    int numIntersections = ReadPanelData<int>(panel, NUM_DOTS);
    vector<float> intersections = ReadPanelData<float>(panel, DOT_POSITIONS, numIntersections * 2);
    vector<int> intersectionFlags = ReadPanelData<int>(panel, DOT_FLAGS, numIntersections);

    int numConnections = ReadPanelData<int>(panel, NUM_CONNECTIONS);
    vector<int> connectionsA = ReadPanelData<int>(panel, DOT_CONNECTION_A, numConnections);
    vector<int> connectionsB = ReadPanelData<int>(panel, DOT_CONNECTION_B, numConnections);

    float left;
    if (alignment & HALIGN_LEFT)   left = x - text.size() * textSize * 2;
    if (alignment & HALIGN_CENTER) left = x - text.size() * textSize;
    if (alignment & HALIGN_RIGHT)  left = x;

    float right;
    if (alignment & HALIGN_LEFT)   right = x;
    if (alignment & HALIGN_CENTER) right = x + text.size() * textSize;
    if (alignment & HALIGN_RIGHT)  right = x + text.size() * textSize * 2;

    float top;
    if (alignment & VALIGN_TOP)    top = y - textSize * 2;
    if (alignment & VALIGN_CENTER) top = y - textSize;
    if (alignment & VALIGN_BOTTOM) top = y;

    float bottom;
    if (alignment & VALIGN_TOP)    bottom = y;
    if (alignment & VALIGN_CENTER) bottom = y + textSize;
    if (alignment & VALIGN_BOTTOM) bottom = y + textSize * 2;

    float spacingX = (right - left) / (text.size() * 3 - 1);
    float spacingY = (top - bottom) / 2;

    for (int i = 0; i < text.size(); i++) {
        char c = tolower(text[i]);
        const vector<int> coords = CharToCoords(c);
        for (int j = 0; j < coords.size(); j++) {
            int n = coords[j];
            intersections.emplace_back((n % 3 + i * 3) * spacingX + left);
            intersections.emplace_back(1 - ((2 - n / 3) * spacingY + bottom));
            intersectionFlags.emplace_back(0);
            if (j > 0 && !(c == '!' && j == 2)) {
                connectionsA.emplace_back(static_cast<int>(intersections.size()) / 2 - 2);
                connectionsB.emplace_back(static_cast<int>(intersections.size()) / 2 - 1);
            }
        }
    }

    // For safety, ensure that arrays have contents before updating their size.
    WritePanelData<float>(panel, DOT_POSITIONS, intersections);
    WritePanelData<int>(panel, DOT_FLAGS, intersectionFlags);
    WritePanelData<int>(panel, NUM_DOTS, static_cast<int>(intersectionFlags.size()));
    WritePanelData<int>(panel, DOT_CONNECTION_A, connectionsA);
    WritePanelData<int>(panel, DOT_CONNECTION_B, connectionsB);
    WritePanelData<int>(panel, NUM_CONNECTIONS, static_cast<int>(connectionsA.size()));
}

void Randomizer::DrawStartingPanelText(const std::vector<std::string>& textLines) {
    int panel = 0x64; // The first panel in the game

    if (textLines.size() == 0) {
        // Do nothing, the horizontal line is already there.
    } else if (textLines.size() == 1) {
        // If one text line, then draw it at 1/3, and the line at 2/3
        float width = textLines[0].size() * 0.05f; // Each character is 0.12f wide
        float height = 0.05f;
        ClearPanel(panel);
        DrawText(panel, textLines[0], 0.5f, 0.33f, HALIGN_CENTER | VALIGN_CENTER);
        DrawLine(panel, { 0.1f, 0.33f, 0.9f, 0.33f });
    } else if (textLines.size() == 2) {
        ClearPanel(panel);
        DrawText(panel, textLines[0], 0.5f, 0.25f, HALIGN_CENTER | VALIGN_CENTER);
        DrawText(panel, textLines[1], 0.5f, 0.75f, HALIGN_CENTER | VALIGN_CENTER);
        DrawLine(panel, { 0.1f, 0.5f, 0.9f, 0.5f });
    } else {
        assert(false); // Don't know how to put more than 4 lines of text on a panel
        return;
    }


    /*

     createText(id, hard ? "expert" : "normal", intersections, connectionsA, connectionsB, 0.1f, 0.9f, 0.25f, 0.4f);
     std::string seedStr = std::to_string(seed);
     createText(id, seedStr, intersections, connectionsA, connectionsB, 0.5f - seedStr.size()*0.06f, 0.5f + seedStr.size()*0.06f, setSeed ? 0.6f : 0.65f, setSeed ? 0.75f : 0.8f);
     if (setSeed) createText(id, "Archipelago", intersections, connectionsA, connectionsB, 0.05f, 0.95f, 0.86f, 0.96f);
     std::string version = VERSION_STR;
     createText(id, version, intersections, connectionsA, connectionsB, 0.98f - version.size()*0.06f, 0.98f, 0.02f, 0.10f);
     if (options) createText(id, "option", intersections, connectionsA, connectionsB, 0.02f, 0.5f, 0.02f, 0.10f);

     drawText(id, intersections, connectionsA, connectionsB, { 0.1f, 0.5f, 0.9f, 0.5f });
    */
    WritePanelData<float>(panel, PATH_WIDTH_SCALE, 0.5f);
    WritePanelData<int>(panel, NEEDS_REDRAW, 1);
}