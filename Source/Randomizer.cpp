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

//012
//345
//678
static unordered_map<char, vector<int>> s_charToCoords = {
	{ 'a',{ 6,0,2,8,5,3 } },
	{ 'c',{ 2,0,6,8 } },
	{ 'd',{ 0,1,5,7,6,0 } },
	{ 'e',{ 2,0,3,5,3,6,8 } },
	{ 'g',{ 2,0,6,8,5,4 } },
	{ 'i',{ 0,2,1,7,6,8 } },
	{ 'k',{ 0,6,3,2,3,8 } },
	{ 'l',{ 0,6,8 } },
	{ 'm',{ 6,0,4,2,8 } },
	{ 'n',{ 6,0,8,2 } },
	{ 'o',{ 0,2,8,6,0 } },
	{ 'p',{ 6,0,2,5,3 } },
	{ 'r',{ 6,0,2,5,3,8 } },
	{ 's',{ 2,0,3,5,8,6 } },
	{ 't',{ 0,2,1,7 } },
	{ 'u',{ 0,6,8,2 } },
	{ 'x',{ 0,8,4,2,6 } },
	{ '0',{ 0,2,8,6,0 } },
	{ '1',{ 0,1,7,6,8 } },
	{ '2',{ 0,2,5,3,6,8 } },
	{ '3',{ 0,2,5,3,5,8,6 } },
	{ '4',{ 0,3,5,2,8 } },
	{ '5',{ 2,0,3,5,8,6 } },
	{ '6',{ 2,0,6,8,5,3 } },
	{ '7',{ 0,2,7 } },
	{ '8',{ 0,2,8,6,0,3,5 } },
	{ '9',{ 6,8,2,0,3,5 } },
	{ '!',{ 1,4,7 } },
	{ ' ',{ } },
	{ '.',{ 7 } },
	{ '-',{ 3,5 } },
};

void Randomizer::DrawLine(int panel, const vector<float>& coords) {
	int numIntersections = ReadPanelData<int>(panel, NUM_DOTS);
	vector<float> intersections = ReadPanelData<float>(panel, DOT_POSITIONS, numIntersections);
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

	WritePanelData<float>(panel, PATH_WIDTH_SCALE, 0.6f);
	WritePanelData<int>(panel, NUM_DOTS, static_cast<int>(intersectionFlags.size()));
	WriteArray<float>(panel, DOT_POSITIONS, intersections);
	WriteArray<int>(panel, DOT_FLAGS, intersectionFlags);
	WritePanelData<int>(panel, NUM_CONNECTIONS, static_cast<int>(connectionsA.size()));
	WriteArray<int>(panel, DOT_CONNECTION_A, connectionsA);
	WriteArray<int>(panel, DOT_CONNECTION_B, connectionsB);
}

void Randomizer::DrawText(int panel, const string& text, float top, float left, float bottom, float right) {
	int numIntersections = ReadPanelData<int>(panel, NUM_DOTS);
	vector<float> intersections = ReadPanelData<float>(panel, DOT_POSITIONS, numIntersections);
	vector<int> intersectionFlags = ReadPanelData<int>(panel, DOT_FLAGS, numIntersections);

	int numConnections = ReadPanelData<int>(panel, NUM_CONNECTIONS);
	vector<int> connectionsA = ReadPanelData<int>(panel, DOT_CONNECTION_A, numConnections);
	vector<int> connectionsB = ReadPanelData<int>(panel, DOT_CONNECTION_B, numConnections);

	float spacingX = (right - left) / (text.size() * 3 - 1);
	float spacingY = (top - bottom) / 2;

	for (int i = 0; i < text.size(); i++) {
		char c = text[i];
		const vector<int>& coords = s_charToCoords[c];
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

	WritePanelData<float>(panel, PATH_WIDTH_SCALE, 0.5f);
	WritePanelData<int>(panel, NUM_DOTS, static_cast<int>(intersectionFlags.size()));
	WriteArray<float>(panel, DOT_POSITIONS, intersections);
	WriteArray<int>(panel, DOT_FLAGS, intersectionFlags);
	WritePanelData<int>(panel, NUM_CONNECTIONS, static_cast<int>(connectionsA.size()));
	WriteArray<int>(panel, DOT_CONNECTION_A, connectionsA);
	WriteArray<int>(panel, DOT_CONNECTION_B, connectionsB);
}

void Randomizer::DrawStartingPanelText(const std::vector<std::string>& textLines) {
	int panel = 0x64; // The first panel in the game

	if (textLines.size() == 0) {
		// No lines, just draw a normal horizontal line
		DrawLine(panel, { 0.1f, 0.5f, 0.9f, 0.5f });
	} else if (textLines.size() == 1) {
		// If one text line, then draw it at 1/3, and the line at 2/3
		float width = textLines[0].size() * 0.06f; // Each character is 0.12f wide, I guess?
		DrawText(panel, textLines[0], 0.5f - width, 0.33f, 0.5f + width, 0.33f);
		DrawLine(panel, { 0.1f, 0.66f, 0.9f, 0.66f });
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
	WritePanelData<int>(panel, NEEDS_REDRAW, 1);
}