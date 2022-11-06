#include "pch.h"
#include "Randomizer.h"
#include "PanelOffsets.h"
#include "Panel.h"
#include "Watchdog.h"

using namespace std;

Randomizer::Randomizer(const shared_ptr<Memory>& memory) {
    _memory = memory;

    _memory->AddSigScan({0x74, 0x41, 0x48, 0x85, 0xC0, 0x74, 0x04, 0x48, 0x8B, 0x48, 0x10}, [this](int64_t offset, int index, const vector<uint8_t>& data) {
        _globals = Memory::ReadStaticInt(offset, index + 0x14, data);
    });

    // Entity_Door::open(float t_target)
    _memory->AddSigScan({0x48, 0x89, 0x48, 0x08, 0x53, 0x48, 0x81, 0xEC, 0x80, 0x00, 0x00, 0x00}, [this](int64_t offset, int index, const vector<uint8_t>& data) {
        _openDoor = offset + index - 3;
    });

    // Entity_Laser::activate(void)
    _memory->AddSigScan({0x40, 0x53, 0x48, 0x83, 0xEC, 0x60, 0x83, 0xB9}, [this](int64_t offset, int index, const vector<uint8_t>& data) {
        _activateLaser = offset + index;
    });

    // hud_report_text_to_player(const char*)
    _memory->AddSigScan({0x00, 0x48, 0x8B, 0xD9, 0x7F, 0x49, 0x48, 0x8B, 0x0D}, [this](int64_t offset, int index, const vector<uint8_t>& data) {
        _reportHudText = offset + index - 12;
    });

    // Entity::has_moved(Vector3*, Quaternion*, BOOL)
    _memory->AddSigScan({0x48, 0x8B, 0x79, 0x18, 0x41, 0x0F, 0xB6, 0xF1}, [this](int64_t offset, int index, const vector<uint8_t>& data) {
        _entityHasMoved = offset + index - 15;
    });
    
    size_t failedScans = _memory->ExecuteSigScans();
    assert(failedScans == 0, "Failed to find some number of sigscans");
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
        assert(false, "Don't know how to put more than 4 lines of text on a panel!");
        return;
    }

    WritePanelData<float>(panel, PATH_WIDTH_SCALE, 0.5f);
    WritePanelData<int>(panel, NEEDS_REDRAW, 1);
}

void Randomizer::OpenDoor(int32_t door, float target) {
    if (!_openDoor) return;
    assert(_memory->ReadString({_globals, 0x18, door * 8, TYPE, PORTABLE_TYPE_NAME}) == "Door", "Cannot open a non-door entity");

    int64_t doorEntity = _memory->ReadData<int64_t>({_globals, 0x18, door * 8}, 1)[0];
    _memory->CallFunction(_openDoor, doorEntity, target);
}

void Randomizer::ActivateLaser(int32_t laser) {
    if (!_activateLaser) return;
    assert(_memory->ReadString({_globals, 0x18, laser * 8, TYPE, PORTABLE_TYPE_NAME}) == "Laser", "Cannot activate a non-laser entity");

    int64_t laserEntity = _memory->ReadData<int64_t>({_globals, 0x18, laser * 8}, 1)[0];
    _memory->CallFunction(_activateLaser, laserEntity);
}

void Randomizer::ShowMessage(const string& message) {
    if (!_reportHudText) return;
    _memory->CallFunction(_reportHudText, message);
}

void Randomizer::MoveEntity(int entity, const function<void(EntityData*)>& updateFunc) {
    int64_t entityPointer = _memory->ReadData<int64_t>({_globals, 0x18, entity * 8}, 1)[0];
    assert(entityPointer != 0, "Entity does not exist");

    vector<float> rawData = _memory->ReadAbsoluteData<float>({entityPointer, POSITION}, sizeof(EntityData) / sizeof(float));
    updateFunc((EntityData*)&rawData[0]);
    _memory->WriteData({_globals, 0x18, entity * 8, POSITION}, rawData);

    if (_entityHasMoved) {
        _memory->CallFunction(_entityHasMoved, entityPointer, entityPointer + POSITION, entityPointer + ORIENTATION, 1 /* true */);
    }
}

void Randomizer::TrackWatchdog(Watchdog* watchdog) {
    std::thread watchdogThread([sharedThis = shared_from_this(), watchdog] {
        while (watchdog->IsRunning()) {
		    std::this_thread::sleep_for(watchdog->sleepTime);
            watchdog->action(sharedThis);
        }
        sharedThis->_watchdogs.erase(watchdog);
        delete watchdog;
    });

    watchdogThread.detach();
    _watchdogs[watchdog] = std::move(watchdogThread);
}

void Randomizer::StopWatchdogs() {
    for (auto& [watchdog, thrd] : _watchdogs) {
        watchdog->Shutdown();
        thrd.join();
    }
}

void Randomizer::ClearPanel(int panel) {
    WritePanelData<int>(panel, NUM_DOTS, 0);
    WritePanelData<int>(panel, NUM_CONNECTIONS, 0);
}

// These coordinates are laid out in a 3x3 grid with these positions:
//   012
//   345
//   678
// -1 is used as a 'gap indicator'
vector<int8_t> CharToCoords(char ch) {
    switch (ch) {
        case ' ': return { };
        case '!': return { 1,4,-1,7 };
        case '"': return { 0,3,-1,2,5 };
        case '#': return { 1,4,6,-1,4,8 }; // Negation symbol
        // case '$':
        // case '%':
        // case '&':
        case '\'': return { 1,4 };
        case '(': return { 2,4,8 };
        case ')': return { 0,4,6 };
        case '*': return { 0,8,-1,1,7,-1,2,6,-1,3,5 };
        case '+': return { 1,7,-1,3,5 };
        case ',': return { 4,6 };
        case '-': return { 3,5 };
        case '.': return { 7 };
        case '/': return { 2,6 };
        case '0': return { 2,8,6,0,2,6 };
        case '1': return { 0,1,7,-1,6,8 };
        case '2': return { 0,1,5,6,8 };
        case '3': return { 0,2,8,6,-1,3,5 };
        case '4': return { 0,3,5,-1,2,8 };
        case '5': return { 2,0,5,7,6 };
        case '6': return { 2,1,3,6,8,5,3 };
        case '7': return { 0,2,7 };
        case '8': return { 0,2,8,6,0,-1,3,5 };
        case '9': return { 5,3,0,2,5,7,6 };
        case ':': return { 1,-1,7 };
        case ';': return { 1,-1,4,7 };
        case '<': return { 2,3,8 };
        case '=': return { 0,2,-1,6,8 };
        case '>': return { 0,5,6 };
        case '?': return { 0,2,5,4,-1,7 };
        case '@': return { 4,5,1,3,7,8 };
        case 'A': return { 6,3,1,5,8,-1,3,5 };
        case 'B': return { 0,2,4,8,6,0,-1,3,4 };
        case 'C': return { 2,1,3,7,8 };
        case 'D': return { 0,1,5,7,6,0 };
        case 'E': return { 2,0,6,8,-1,3,4 };
        case 'F': return { 2,0,6,-1,3,4 };
        case 'G': return { 2,0,6,8,5,4 };
        case 'H': return { 0,6,-1,3,5,-1,2,8 };
        case 'I': return { 0,2,-1,1,7,-1,6,8 };
        case 'J': return { 0,2,-1,1,7,6,3 };
        case 'K': return { 0,6,-1,2,3,8 };
        case 'L': return { 0,6,8 };
        case 'M': return { 6,0,4,2,8 };
        case 'N': return { 6,0,8,2 };
        case 'O': return { 0,2,8,6,0 };
        case 'P': return { 6,0,2,5,3 };
        case 'Q': return { 0,2,8,6,0,-1,4,8 };
        case 'R': return { 6,0,2,5,3,8 };
        case 'S': return { 2,0,3,5,8,6 };
        case 'T': return { 0,2,-1,1,7 };
        case 'U': return { 0,6,8,2 };
        case 'V': return { 0,7,2 };
        case 'W': return { 0,6,4,8,2 };
        case 'X': return { 0,8,-1,2,6 };
        case 'Y': return { 0,4,2,-1,4,7 };
        case 'Z': return { 0,2,6,8 };
        case '[': return { 2,1,7,8 };
        case '\\': return { 0, 8 };
        case ']': return { 0,1,7,6 };
        case '^': return { 3,1,5 };
        case '_': return { 6,8 };
        case '`': return { 1,5 };
        // case '{':
        case '|': return { 1,7 };
        // case '}':
        // case '~':
        default: assert(false, "Don't know how to draw the provided character -- please update CharToCoords.");
    }

    return {};
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

    float spacingX = (text.size() * textSize * 2) / (text.size() * 3 - 1);
    float spacingY = textSize;

    for (int i = 0; i < text.size(); i++) {
        char c = toupper(text[i]);
        const vector<int8_t> coords = CharToCoords(c);
        for (int j = 0; j < coords.size(); j++) {
            int8_t n = coords[j];
            if (n == -1) continue; // Indicates a gap
            intersections.emplace_back((n % 3 + i * 3) * spacingX + left);
            intersections.emplace_back(1 - ((n / 3 - 2) * spacingY + bottom));
            intersectionFlags.emplace_back(0);
            if (j > 0) {
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
