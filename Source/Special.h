#pragma once
#include "Generate.h"
#include "Randomizer.h"

typedef std::pair<int, int> Point;
typedef std::set<Point> Shape;



//Functions to handle special case puzzles

class Special {

public:

	enum Flag { //Trigger certain actions in the generator

	};

	Special(std::shared_ptr<Generate> generator) {
		_generator = generator;
	}
	
	void generateReflectionDotPuzzle(int id1, int id2, std::vector<std::pair<int, int>> symbols, Panel::Symmetry symmetry);
	void generateAntiPuzzle(int id);
	void generateColorFilterPuzzle(int id, std::vector<std::pair<int, int>> symbols, Color filter);
	void generateSoundDotPuzzle(int id, std::vector<int> dotSequence, bool writeSequence);
	void generateSoundDotReflectionPuzzle(int id, std::vector<int> dotSequence1, std::vector<int> dotSequence2, int numColored, bool writeSequence);
	void generateRGBStonePuzzleN(int id);
	void generateRGBStarPuzzleN(int id);
	void generateJungleVault(int id);
	void generateApplePuzzle(int id, bool changeExit, bool flip);
	void generateKeepLaserPuzzle(int id, std::set<Point> path1, std::set<Point> path2, std::set<Point> path3, std::set<Point> path4, std::vector<std::pair<int, int>> symbols);

	void setTarget(int puzzle, int target);
	void clearTarget(int puzzle);
	void setTargetAndDeactivate(int puzzle, int target);


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
		std::vector<byte> bytes1 = panel->_memory->ReadPanelData<byte>(id1, 0, 0x1000);
		std::vector<byte> bytes2 = panel->_memory->ReadPanelData<byte>(id2, 0, 0x1000);
		panel->_memory->WritePanelData<byte>(id1, 0, bytes2);
		panel->_memory->WritePanelData<byte>(id2, 0, bytes1);
	}

	template <class T> std::vector<T> testRead(int address, int numItems) {
		std::shared_ptr<Panel> panel = std::make_shared<Panel>();
		std::vector<int> offsets = { address };
		return panel->_memory->ReadData<T>(offsets, numItems);
	}

private:

	Endpoint::Direction get_sym_dir(Endpoint::Direction direction, Panel::Symmetry symmetry) {
		std::vector<Endpoint::Direction> mapping;
		switch (symmetry) {
		case Panel::Symmetry::Horizontal: mapping = { Endpoint::Direction::LEFT, Endpoint::Direction::RIGHT, Endpoint::Direction::DOWN, Endpoint::Direction::UP }; break;
		case Panel::Symmetry::Vertical: mapping = { Endpoint::Direction::RIGHT, Endpoint::Direction::LEFT, Endpoint::Direction::UP, Endpoint::Direction::DOWN }; break;
		case Panel::Symmetry::Rotational: mapping = { Endpoint::Direction::RIGHT, Endpoint::Direction::LEFT, Endpoint::Direction::DOWN, Endpoint::Direction::UP }; break;
		case Panel::Symmetry::RotateLeft: mapping = { Endpoint::Direction::DOWN, Endpoint::Direction::UP, Endpoint::Direction::LEFT, Endpoint::Direction::RIGHT }; break;
		case Panel::Symmetry::RotateRight: mapping = { Endpoint::Direction::UP, Endpoint::Direction::DOWN, Endpoint::Direction::RIGHT, Endpoint::Direction::LEFT }; break;
		case Panel::Symmetry::FlipXY: mapping = { Endpoint::Direction::UP, Endpoint::Direction::DOWN, Endpoint::Direction::LEFT, Endpoint::Direction::RIGHT }; break;
		case Panel::Symmetry::FlipNegXY: mapping = { Endpoint::Direction::DOWN, Endpoint::Direction::UP, Endpoint::Direction::RIGHT, Endpoint::Direction::LEFT }; break;
		default: mapping = { Endpoint::Direction::LEFT, Endpoint::Direction::RIGHT, Endpoint::Direction::UP, Endpoint::Direction::DOWN }; break;
		}
		return mapping[direction];
	}

	std::shared_ptr<Generate> _generator;

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