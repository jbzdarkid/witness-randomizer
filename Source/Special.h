#pragma once
#include "Generate.h"
#include "Randomizer.h"
#include <algorithm>

typedef std::set<Point> Shape;



//Functions to handle special case puzzles

class Special {

public:

	enum Flag { //Trigger certain actions in the generator

	};

	Special(std::shared_ptr<Generate> generator) {
		_generator = generator;
	}
	
	void generateReflectionDotPuzzle(std::shared_ptr<Generate> generator, int id1, int id2, std::vector<std::pair<int, int>> symbols, Panel::Symmetry symmetry, bool split);
	void generateAntiPuzzle(int id);
	void generateColorFilterPuzzle(int id, Point size, std::vector<std::pair<int, int>> symbols, Color filter);
	void generateSoundDotPuzzle(int id, Point size, std::vector<int> dotSequence, bool writeSequence);
	void generateSoundDotPuzzle(int id1, int id2, std::vector<int> dotSequence, bool writeSequence);
	void generateSoundDotReflectionPuzzle(int id, Point size, std::vector<int> dotSequence1, std::vector<int> dotSequence2, int numColored, bool writeSequence);
	void generateRGBStonePuzzleN(int id);
	void generateRGBStarPuzzleN(int id);
	void generateRGBStonePuzzleH(int id);
	void generateRGBDotPuzzleH(int id);
	void generateJungleVault(int id);
	void generateApplePuzzle(int id, bool changeExit, bool flip);
	void generateKeepLaserPuzzle(int id, std::set<Point> path1, std::set<Point> path2, std::set<Point> path3, std::set<Point> path4, std::vector<std::pair<int, int>> symbols);
	void generateMountaintop(int id);
	void generateMultiPuzzle(std::vector<int> ids, std::vector<std::vector<std::pair<int, int>>> symbolVec);
	bool generateMultiPuzzle(std::vector<int> ids, std::vector<Generate>& gens, std::vector<PuzzleSymbols> symbols, std::set<Point> path);
	void generate2Bridge(int id1, int id2);
	bool generate2Bridge(int id1, int id2, std::vector<std::shared_ptr<Generate>> gens);
	void generateMountainFloor(std::vector<int> ids, int idfloor);
	void generatePivotPanel(int id, Point gridSize, std::vector<std::pair<int, int>> symbolVec); //Too slow right now
	void modifyGate(int id);
	void addDecoyExits(std::shared_ptr<Generate> gen, int amount);
	void initSSGrid(std::shared_ptr<Generate> gen);
	void generateSymmetryGate(int id);
	bool checkDotSolvability(std::shared_ptr<Panel> panel1, std::shared_ptr<Panel> panel2, Panel::Symmetry correctSym);

	void test();
	void setTarget(int puzzle, int target);
	void clearTarget(int puzzle);
	void setTargetAndDeactivate(int puzzle, int target);
	template <class T> std::vector<T> ReadPanelData(int panel, int offset, size_t size) {
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->ReadPanelData<T>(panel, offset, size);
	}
	template <class T> T ReadPanelData(int panel, int offset) {
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->ReadPanelData<T>(panel, offset);
	}
	template <class T> std::vector<T> ReadArray(int panel, int offset, int size) {
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->ReadArray<T>(panel, offset, size);
	}
	template <class T> void WritePanelData(int panel, int offset, const std::vector<T>& data) {
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->WritePanelData<T>(panel, offset, data);
	}
	template <class T> void WriteArray(int panel, int offset, const std::vector<T>& data) {
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->WriteArray<T>(panel, offset, data, false);
	}
	template <class T> void WriteArray(int panel, int offset, const std::vector<T>& data, bool force) {
		std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe"); return _memory->WriteArray<T>(panel, offset, data, force);
	}

	int testFind(std::vector<byte> bytes) {
		std::shared_ptr<Panel> panel = std::make_shared<Panel>();
		int index = 0;
		panel->_memory->AddSigScan(bytes, [&](int i) {
			index = i;
		});
		panel->_memory->ExecuteSigScans();
		return index;
	}

	void testSwap(int id1, int id2) {
		std::shared_ptr<Panel> panel = std::make_shared<Panel>();
		std::vector<byte> bytes1 = panel->_memory->ReadPanelData<byte>(id1, 0, 0x600);
		std::vector<byte> bytes2 = panel->_memory->ReadPanelData<byte>(id2, 0, 0x600);
		panel->_memory->WritePanelData<byte>(id1, 0, bytes2);
		panel->_memory->WritePanelData<byte>(id2, 0, bytes1);
	}

	template <class T> std::vector<T> testRead(int address, int numItems) {
		std::shared_ptr<Panel> panel = std::make_shared<Panel>();
		std::vector<int> offsets = { address };
		return panel->_memory->ReadData<T>(offsets, numItems);
	}

	void testPanel(int id) {
		std::shared_ptr<Panel> panel = std::make_shared<Panel>(id);
		panel->Write();
	}

private:

	std::shared_ptr<Generate> _generator;

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