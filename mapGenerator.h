#pragma once

// a file containing map generator for Isoria

#include <SFML/System.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstdlib>

#include <iostream>

#include "data.h"
#include "map.h"
#include "quickWrite.h"
#include "mapGenerationMisc.h"
#include "declarations.h"

namespace fs = std::filesystem;

bool mgDebug = false;

std::vector<datapack> mgDatapacks;
std::vector<MGDatapack> mgConversionDatapacks;

MGMap map;

sf::Vector2i mgMapSize;

//				LOADING DATA

// classes to get empty infos with names without unnecessary hustle

backgroundBlockInfo getMGBackgroundBlockInfo(str name) {
	backgroundBlockInfo temp;
	temp.name = name;
	return temp;
}

floorBlockInfo getMGFloorBlockInfo(str name) {
	floorBlockInfo temp;
	temp.name = name;
	return temp;
}

wallBlockInfo getMGWallBlockInfo(str name) {
	wallBlockInfo temp;
	temp.name = name;
	return temp;
}

// loads added backgrounds of a datapack
void loadMGAddedBackground(datapack* pointer) {
	// for empty
	pointer->backgroundBlocks.resize(1);

	str path = pointer->path + "added/backgrounds/";
	for (const auto& entry : fs::directory_iterator(path)) {
		if (mgDebug) {
			std::cout << "MG debug: Found " << entry.path().string() << "\n";
		}

		// loads data about name
		str name = entry.path().string();
		// cuts path from block name
		name = name.erase(name.find('.'), name.size() - name.find('.')).erase(0, name.find_last_of('/') + 1);
		// only name needed for saving in a save file
		pointer->backgroundBlocks.push_back(getMGBackgroundBlockInfo(name));

		if (mgDebug) {
			std::cout << "MG debug: Loaded a background block as: " << pointer->backgroundBlocks[pointer->backgroundBlocks.size() - 1].name << "\n";
		}
	}
}

// loads added floors of a datapack
void loadMGAddedFloor(datapack* pointer) {
	// for empty
	pointer->floorBlocks.resize(1);

	str path = pointer->path + "added/floors/";
	for (const auto& entry : fs::directory_iterator(path)) {
		if (mgDebug) {
			std::cout << "MG debug: Found " << entry.path().string() << "\n";
		}

		// loads data about name
		str name = entry.path().string();
		// cuts path from block name
		name = name.erase(name.find('.'), name.size() - name.find('.')).erase(0, name.find_last_of('/') + 1);
		// only name needed for saving in a save file
		pointer->floorBlocks.push_back(getMGFloorBlockInfo(name));

		if (mgDebug) {
			std::cout << "MG debug: Loaded a floor block as: " << name << "\n";
		}
	}
}

// loads added walls of a datapack
void loadMGAddedWall(datapack* pointer) {
	// for empty
	pointer->wallBlocks.resize(1);

	str path = pointer->path + "added/walls/";
	for (const auto& entry : fs::directory_iterator(path)) {
		if (mgDebug) {
			std::cout << "MG debug: Found " << entry.path().string() << "\n";
		}

		// loads data about name
		str name = entry.path().string();
		// cuts path from block name
		name = name.erase(name.find('.'), name.size() - name.find('.')).erase(0, name.find_last_of('/') + 1);
		// only name needed for saving in a save file
		pointer->wallBlocks.push_back(getMGWallBlockInfo(name));

		if (mgDebug) {
			std::cout << "MG debug: Loaded a wall block as: " << name << "\n";
		}
	}
}

// loads a datapacks added 
void loadMGDatapackAdded(datapack* pointer) {
	str path = pointer->path;

	if (fs::is_directory(path + "added/backgrounds")) {
		if (mgDebug) {
			std::cout << "MG debug: Added background folder present in " << path << "added directory" << "\n";
			loadMGAddedBackground(pointer);
		}
	}

	if (fs::is_directory(path + "added/floors")) {
		if (mgDebug) {
			std::cout << "MG debug: Added background folder present in " << path << "added directory" << "\n";
		}
		loadMGAddedFloor(pointer);
	}

	if (fs::is_directory(path + "added/walls")) {
		if (mgDebug) {
			std::cout << "MG debug: Added wall folder present in " << path << "added directory" << "\n";
		}
		loadMGAddedWall(pointer);
	}
}

// loads a certain datapack from its folder
void loadMGDatapack(str path, str name, ushort number = mgDatapacks.size()) {
	mgDatapacks.resize(number + 1);
	mgDatapacks[number].path = path;
	mgDatapacks[number].name = name;
	if (fs::is_directory(path + "added")) {
		if (mgDebug) {
			std::cout << "MG debug: Added folder present in " << path << " directory" << "\n";
		}
		loadMGDatapackAdded(&mgDatapacks[number]);
	}
}

// loads datapacks from file
void loadMGDatapacks() {
	std::ifstream file;
	file.open("launcher/datapacks.txt");

	for (str input; file >> input;) {
		loadMGDatapack("data/" + input + "/", input);
	}

	file.close();
}

//				MAP PREPARATION

// main map preparation function
void prepareMGMap() {
	// temp before introducing flexible generation
	map.create(std::vector<vec2i>{mgMapSize}, std::vector <str>{"Forest"});
}

//				PROPER GENERATION

// a function to create layer background
void MGGenerateBackground(MGLayer* layer) {
	vec2i size = vec2i(layer->blocks.size(), layer->blocks[0].size());

	// filling with base stone
	for (ushort y = 0; y < size.y; y++) {
		for (ushort x = 0; x < size.x; x++) {
			layer->blocks[y][x] = getMGBlock(0, 1);
		}
	}
}

// a function to create layer floor
void MGGenerateFloor(MGLayer* layer) {
	vec2i size = vec2i(layer->blocks.size(), layer->blocks[0].size());

	// filling with base grass and air
	for (ushort y = 0; y < size.y; y++) {
		for (ushort x = 0; x < size.x; x++) {
			if (rand() % 2) {
				layer->blocks[y][x] = getMGBlock(0, 1);
			}
			else {
				layer->blocks[y][x] = getMGBlock(0, 0);
			}
		}
	}
}

// a function to create layer wall
void MGGenerateWall(MGLayer* layer) {
	vec2i size = vec2i(layer->blocks.size(), layer->blocks[0].size());

	// filling with base wood and air
	for (ushort y = 0; y < size.y; y++) {
		for (ushort x = 0; x < size.x; x++) {
			if (!(rand() % 4)) {
				layer->blocks[y][x] = getMGBlock(0, 1);
			}
			else {
				layer->blocks[y][x] = getMGBlock(0, 0);
			}
		}
	}
}

// collective function for all layers of a dimension
void MGGenerateDimension(MGDimension* pointer) {
	MGGenerateBackground(&pointer->background);
	MGGenerateFloor(&pointer->floor);
	MGGenerateWall(&pointer->wall);
}

// main generation function
void MGStartGeneration() {

	// get more random seed
	srand(time(NULL));

	// generating each dimension
	for (ushort n = 0; n < map.dimensions.size(); n++) {
		if (mgDebug) {
			std::cout << "MG debug: Generating dimension " << n << ": " << map.dimensions[n].name << "\n";
		}
		MGGenerateDimension(&map.dimensions[n]);
	}
}

//				MAP SAVING

// a function to save dimension order
void MGSaveDimensionOrder(str* path) {
	std::ofstream file;
	file.open(*path + "dimensionList.txt");

	for (ushort n = 0; n < map.dimensions.size(); n++) {
		file << map.dimensions[n].name << " " << map.dimensions[n].floor.blocks[0].size() << " " << map.dimensions[n].floor.blocks.size() << "\n";
	}

	file.close();
}

// a function to save default(!) player details
void MGSavePlayerData(str* path) {
	std::ofstream file;
	file.open(*path + "playerData.txt");

	// temp spawn in dimension 0 and at 1/4 of its coords
	file << 0 << " " << map.dimensions[0].storedSize.x / 4 << " " << map.dimensions[0].storedSize.y / 4;

	file.close();
}

// a function to save a single dimensions
void MGSaveDimension(str path, MGDimension* pointer) {
	std::ofstream file;
	file.open(path + pointer->name + ".txt");

	// background
	if (mgDebug) {
		std::cout << "MG debug: Saving background layer of: " << pointer->name << "\n";
	}
	for (ushort y = 0; y < pointer->storedSize.y; y++) {
		for (ushort x = 0; x < pointer->storedSize.x; x++) {
			file << mgConversionDatapacks[pointer->background.blocks[y][x].datapackNumber].backgroundConversion[pointer->background.blocks[y][x].blockNumber] << " ";
		}
	}
	file << "\n";

	// floor
	if (mgDebug) {
		std::cout << "MG debug: Saving floor layer of: " << pointer->name << "\n";
	}
	for (ushort y = 0; y < pointer->storedSize.y; y++) {
		for (ushort x = 0; x < pointer->storedSize.x; x++) {
			file << mgConversionDatapacks[pointer->floor.blocks[y][x].datapackNumber].floorConversion[pointer->floor.blocks[y][x].blockNumber] << " ";
		}
	}
	file << "\n";

	// wall
	if (mgDebug) {
		std::cout << "MG debug: Saving wall layer of: " << pointer->name << "\n";
	}
	for (ushort y = 0; y < pointer->storedSize.y; y++) {
		for (ushort x = 0; x < pointer->storedSize.x; x++) {
			file << mgConversionDatapacks[pointer->wall.blocks[y][x].datapackNumber].wallConversion[pointer->wall.blocks[y][x].blockNumber] << " ";
		}
	}

	file.close();
}

// main function for saving of specific dimensions
void MGSaveDimensions(str* path) {
	fs::create_directory(*path + "dimensions");

	for (ushort n = 0; n < map.dimensions.size(); n++) {
		MGSaveDimension(*path + "dimensions/", &map.dimensions[n]);
	}
}

// saves data about ids in file and their relation to real ids and prepare conversion tables
void MGSaveIdData(str* path) {
	std::ofstream fileB, fileF, fileW;
	fileB.open(*path + "conversionBackground.txt");
	fileF.open(*path + "conversionFloor.txt");
	fileW.open(*path + "conversionWall.txt");

	fileB << 0 << " 0 air" << "\n";
	fileF << 0 << " 0 air" << "\n";
	fileW << 0 << " 0 air" << "\n";

	ulong counterB = 1; // background
	ulong counterF = 1; // floor
	ulong counterW = 1; // wall

	if (mgDebug) {
		std::cout << "MG debug: Starting id saving" << "\n";
	}
	
	mgConversionDatapacks.resize(mgDatapacks.size());

	// convention:	 id		datapack	name
	for (ushort n = 0; n < mgDatapacks.size(); n++) {

		// background blocks
		if (mgDatapacks[n].backgroundBlocks.size()) {
			if (mgDebug) {
				std::cout << "MG debug: Adding background conversions for datapack: " << mgDatapacks[n].name << "\n";
			}

			mgConversionDatapacks[n].backgroundConversion.push_back(0);
			// starting after air
			for (uint nn = 1; nn < mgDatapacks[n].backgroundBlocks.size(); nn++, counterB++) {
				fileB << counterB << " " << n << " " << mgDatapacks[n].backgroundBlocks[nn].name << "\n";
				mgConversionDatapacks[n].backgroundConversion.push_back(counterB);
			}
		}

		// floor blocks
		if (mgDatapacks[n].floorBlocks.size()) {
			if (mgDebug) {
				std::cout << "MG debug: Adding floor conversions for datapack: " << mgDatapacks[n].name << "\n";
			}

			mgConversionDatapacks[n].floorConversion.push_back(0);
			// starting after air
			for (uint nn = 1; nn < mgDatapacks[n].floorBlocks.size(); nn++, counterF++) {
				fileF << counterF << " " << n << " " << mgDatapacks[n].floorBlocks[nn].name << "\n";
				mgConversionDatapacks[n].floorConversion.push_back(counterF);
			}
		}

		// wall blocks
		if (mgDatapacks[n].wallBlocks.size()) {
			if (mgDebug) {
				std::cout << "MG debug: Adding wall conversions for datapack: " << mgDatapacks[n].name << "\n";
			}

			mgConversionDatapacks[n].wallConversion.push_back(0);
			// starting after air
			for (uint nn = 1; nn < mgDatapacks[n].wallBlocks.size(); nn++, counterW++) {
				fileW << counterW << " " << n << " " << mgDatapacks[n].wallBlocks[nn].name << "\n";
				mgConversionDatapacks[n].wallConversion.push_back(counterW);
			}
		}
	
	}

	fileB.close();
	fileF.close();
	fileW.close();
}

// saves data about datapacks present
void MGSaveDatapackData(str* path) {
	std::ofstream file;
	file.open(*path + "datapackInfo.txt");

	if (mgDebug) {
		std::cout << "MG debug: Saving datapacks used, amount: " << mgDatapacks.size() << "\n";
	}

	for (ushort n = 0; n < mgDatapacks.size(); n++) {
		if (mgDebug) {
			std::cout << "MG debug: Adding to file: " << mgDatapacks[n].name << "\n";
		}
		file << mgDatapacks[n].name << "\n";
	}

	file.close();
}

// main map saving function
void MGSaveMap(str saveName) {
	str path = "saves/" + saveName;
	fs::create_directory(path);
	path += "/";

	MGSaveDatapackData(&path);

	MGSaveIdData(&path);

	MGSaveDimensions(&path);

	MGSavePlayerData(&path);

	MGSaveDimensionOrder(&path);
}

//				MAIN FUNCTION

// a main function to generate a map, almost completely independent from source file, status string gives current status of generator, only creates a file, needs to be loaded later!
void generateMap(str name, str* statusString, sf::Vector2i size, bool debugMode = true) {
	mgDebug = debugMode;
	mgMapSize = size;

	setStage(0, 1);

	if (mgDebug) {
		std::cout << "[ STARTING ] MG debug: Starting world generation..." << "\n";
	}

	*statusString = "Loading definitions...";
	if (mgDebug) {
		std::cout << "MG debug: Loading definitions" << "\n";
	}

	loadMGDatapacks();

	*statusString = "Preparing tables...";
	if (mgDebug) {
		std::cout << "MG debug: Preparing tables" << "\n";
	}

	prepareMGMap();

	*statusString = "Generating map...";
	if (mgDebug) {
		std::cout << "MG debug: Generating map" << "\n";
	}

	MGStartGeneration();

	*statusString = "Saving map...";
	if (mgDebug) {
		std::cout << "MG debug: Saving map" << "\n";
	}

	MGSaveMap(name);

	*statusString = "Map ready!";
	if (mgDebug) {
		std::cout << "MG debug: Map ready!" << "\n";
	}

	setStage(1, 0);
}