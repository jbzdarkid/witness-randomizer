#include "pch.h"

#include "Randomizer.h"
#include "Panel.h"
#include "PanelOffsets.h"

using namespace std;

void Panel::Edit(const shared_ptr<Randomizer>& randomizer, int id, const PanelEditFunction& editFunction, bool readFromPanel) {
	if (readFromPanel) {
		int numIntersections = randomizer->ReadPanelData<int>(id, NUM_DOTS);
		intersections = randomizer->ReadPanelData<float>(id, DOT_POSITIONS, numIntersections * 2);
		intersectionFlags = randomizer->ReadPanelData<int>(id, DOT_FLAGS, numIntersections);

		int numConnections = randomizer->ReadPanelData<int>(id, NUM_CONNECTIONS);
		connectionsA = randomizer->ReadPanelData<int>(id, DOT_CONNECTION_A, numConnections);
		connectionsB = randomizer->ReadPanelData<int>(id, DOT_CONNECTION_B, numConnections);
	}

	editFunction(*this);
	assert(intersections.size() == intersectionFlags.size() * 2, "");
	assert(connectionsA.size() == connectionsB.size(), "");

	randomizer->WritePanelData<int>(id, NUM_DOTS, static_cast<int>(intersectionFlags.size()));
	randomizer->WritePanelData<float>(id, DOT_POSITIONS, intersections);
	randomizer->WritePanelData<int>(id, DOT_FLAGS, intersectionFlags);

	randomizer->WritePanelData<int>(id, NUM_CONNECTIONS, static_cast<int>(connectionsA.size()));
	randomizer->WritePanelData<int>(id, DOT_CONNECTION_A, connectionsA);
	randomizer->WritePanelData<int>(id, DOT_CONNECTION_B, connectionsB);
}