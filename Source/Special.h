#pragma once
#include "Generate.h"
#include "Randomizer.h"
#include "Watchdog.h"
#include <algorithm>

typedef std::set<Point> Shape;

//Functions to handle special case puzzles

template <class T> struct MemoryWrite {
	int id;
	int offset;
	std::vector<T> data;
	MemoryWrite(int id, int offset, std::vector<T> data) { this->id = id; this->offset = offset; this->data = data; }
};

class Special {

public:

	Special(std::shared_ptr<Generate> generator) {
		this->generator = generator;
	}
	
	void generateReflectionDotPuzzle(std::shared_ptr<Generate> gen, int id1, int id2, std::vector<std::pair<int, int>> symbols, Panel::Symmetry symmetry, bool split);
	void generateAntiPuzzle(int id);
	void generateColorFilterPuzzle(int id, Point size, std::vector<std::pair<int, int>> symbols, Color filter);
	void generateSoundDotPuzzle(int id, Point size, std::vector<int> dotSequence, bool writeSequence);
	void generateSoundDotPuzzle(int id1, int id2, std::vector<int> dotSequence, bool writeSequence);
	void generateSoundDotReflectionPuzzle(int id, Point size, std::vector<int> dotSequence1, std::vector<int> dotSequence2, int numColored, bool writeSequence);
	bool generateSoundDotReflectionSpecial(int id, Point size, std::vector<int> dotSequence1, std::vector<int> dotSequence2, int numColored);
	void generateRGBStonePuzzleN(int id);
	void generateRGBStarPuzzleN(int id);
	void generateRGBStonePuzzleH(int id);
	void generateRGBDotPuzzleH(int id);
	void generateJungleVault(int id);
	void generateApplePuzzle(int id, bool changeExit, bool flip);
	void generateKeepLaserPuzzle(int id, std::set<Point> path1, std::set<Point> path2, std::set<Point> path3, std::set<Point> path4, std::vector<std::pair<int, int>> symbols);
	void generateMountaintop(int id, std::vector<std::pair<int, int>> symbolVec);
	void generateMultiPuzzle(std::vector<int> ids, std::vector<std::vector<std::pair<int, int>>> symbolVec, bool flip);
	bool generateMultiPuzzle(std::vector<int> ids, std::vector<Generate>& gens, std::vector<PuzzleSymbols> symbols, std::set<Point> path);
	void generate2Bridge(int id1, int id2);
	bool generate2Bridge(int id1, int id2, std::vector<std::shared_ptr<Generate>> gens);
	void generate2BridgeH(int id1, int id2);
	bool generate2BridgeH(int id1, int id2, std::vector<std::shared_ptr<Generate>> gens);
	void generateMountainFloor(std::vector<int> ids, int idfloor);
	void generateMountainFloorH(std::vector<int> ids, int idfloor);
	void generatePivotPanel(int id, Point gridSize, std::vector<std::pair<int, int>> symbolVec); //Too slow right now
	void modifyGate(int id);
	void addDecoyExits(std::shared_ptr<Generate> gen, int amount);
	void initSSGrid(std::shared_ptr<Generate> gen);
	void initRotateGrid(std::shared_ptr<Generate> gen);
	void initPillarSymmetry(std::shared_ptr<Generate> gen, int id, Panel::Symmetry symmetry);
	void generateSymmetryGate(int id);
	bool checkDotSolvability(std::shared_ptr<Panel> panel1, std::shared_ptr<Panel> panel2, Panel::Symmetry correctSym);
	void createArrowPuzzle(int id, int x, int y, int dir, int ticks, std::vector<Point> gaps);
	void createArrowSecretDoor(int id);
	void generateCenterPerspective(int id, std::vector<std::pair<int, int>> symbolVec, int symbolType);
	static void drawSeedAndDifficulty(int id, int seed, bool hard);
	static void drawGoodLuckPanel(int id);

	void test();

	static void setTarget(int puzzle, int target)
	{
		WritePanelData(puzzle, TARGET, target + 1);
	}

	static void clearTarget(int puzzle)
	{
		WritePanelData(puzzle, TARGET, 0);
	}

	static void copyTarget(int puzzle, int sourceTarget)
	{
		WritePanelData(puzzle, TARGET, ReadPanelData<int>(sourceTarget, TARGET));
	}

	static void setTargetAndDeactivate(int puzzle, int target)
	{
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); _memory->WritePanelData<float>(target, POWER, { 0.0, 0.0 });
		WritePanelData(puzzle, TARGET, target + 1);
	}
	static void setPower(int puzzle, bool power) {
		
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe");
		if (power) _memory->WritePanelData<float>(puzzle, POWER, { 1.0, 1.0 });
		else _memory->WritePanelData<float>(puzzle, POWER, { 0.0, 0.0 });
	}
	template <class T> static std::vector<T> ReadPanelData(int panel, int offset, size_t size) {
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->ReadPanelData<T>(panel, offset, size);
	}
	template <class T> T static ReadPanelData(int panel, int offset) {
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->ReadPanelData<T>(panel, offset);
	}
	template <class T> static std::vector<T> ReadArray(int panel, int offset, int size) {
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->ReadArray<T>(panel, offset, size);
	}
	static void WritePanelData(int panel, int offset, int data) {
		writeInt.push_back(MemoryWrite<int>(panel, offset, { data }));
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->WritePanelData<int>(panel, offset, { data });
	}
	static void WritePanelData(int panel, int offset, float data) {
		writeFloat.push_back(MemoryWrite<float>(panel, offset, { data }));
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->WritePanelData<float>(panel, offset, { data });
	}
	static void WritePanelData(int panel, int offset, Color data) {
		writeColor.push_back(MemoryWrite<Color>(panel, offset, { data }));
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->WritePanelData<Color>(panel, offset, { data });
	}
	static void WriteArray(int panel, int offset, const std::vector<int>& data) {
		return WriteArray(panel, offset, data, false);
	}
	static void WriteArray(int panel, int offset, const std::vector<int>& data, bool force) {
		writeIntVec.push_back(MemoryWrite<int>(panel, offset, data));
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->WriteArray<int>(panel, offset, data, force);
	}
	static void WriteArray(int panel, int offset, const std::vector<float>& data) {
		return WriteArray(panel, offset, data, false);
	}
	static void WriteArray(int panel, int offset, const std::vector<float>& data, bool force) {
		writeFloatVec.push_back(MemoryWrite<float>(panel, offset, data));
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->WriteArray<float>(panel, offset, data, force);
	}
	static void WriteArray(int panel, int offset, const std::vector<Color>& data) {
		return WriteArray(panel, offset, data, false);
	}
	static void WriteArray(int panel, int offset, const std::vector<Color>& data, bool force) {
		writeColorVec.push_back(MemoryWrite<Color>(panel, offset, data));
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->WriteArray<Color>(panel, offset, data, force);
	}

	static int testFind(std::vector<byte> bytes) {
		std::shared_ptr<Panel> panel = std::make_shared<Panel>();
		int index = 0;
		panel->_memory->AddSigScan(bytes, [&](int i) {
			index = i;
		});
		panel->_memory->ExecuteSigScans();
		return index;
	}

	static void testSwap(int id1, int id2) {
		std::shared_ptr<Panel> panel = std::make_shared<Panel>();
		std::vector<byte> bytes1 = panel->_memory->ReadPanelData<byte>(id1, 0, 0x600);
		std::vector<byte> bytes2 = panel->_memory->ReadPanelData<byte>(id2, 0, 0x600);
		panel->_memory->WritePanelData<byte>(id1, TRACED_EDGES, bytes2);
		panel->_memory->WritePanelData<byte>(id2, TRACED_EDGES, bytes1);
		panel->_memory->WritePanelData<int>(id1, NEEDS_REDRAW, { 1 });
		panel->_memory->WritePanelData<int>(id2, NEEDS_REDRAW, { 1 });
	}

	template <class T> static std::vector<T> testRead(int address, int numItems) {
		std::shared_ptr<Panel> panel = std::make_shared<Panel>();
		std::vector<int> offsets = { address };
		return panel->_memory->ReadData<T>(offsets, numItems);
	}

	static void testPanel(int id) {
		std::shared_ptr<Panel> panel = std::make_shared<Panel>(id);
		panel->Write();
	}

	static std::vector<MemoryWrite<int>> writeInt;
	static std::vector<MemoryWrite<float>> writeFloat;
	static std::vector<MemoryWrite<Color>> writeColor;
	static std::vector<MemoryWrite<int>> writeIntVec;
	static std::vector<MemoryWrite<float>> writeFloatVec;
	static std::vector<MemoryWrite<Color>> writeColorVec;

private:

	std::shared_ptr<Generate> generator;

	template <class T> T pick_random(std::vector<T>& vec) { return vec[rand() % vec.size()]; }
	template <class T> T pick_random(std::set<T>& set) { auto it = set.begin(); std::advance(it, rand() % set.size()); return *it; }
	template <class T> T pop_random(std::vector<T>& vec) {
		int i = rand() % vec.size();
		T item = vec[i];
		vec.erase(vec.begin() + i);
		return item;
	}
	template <class T> T pop_random(std::set<T>& set) {
		auto it = set.begin();
		std::advance(it, rand() % set.size());
		T item = *it;
		set.erase(item);
		return item;
	}
};