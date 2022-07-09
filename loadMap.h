#pragma once

// a file containing map loading functions

#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cstdlib>

#include <iostream>

#include "map.h"
#include "data.h"
#include "quickWrite.h"
#include "loadMapMisc.h"
#include "declarations.h"
#include "shared.h"

bool mlDebug;

mapContainer* mlMap;
str mlSaveName;

std::vector<datapackCheckStruct> mlDatapackCheck;
datapackContainer* mlDatapack;

std::vector<str> mlDimensionNames;
std::vector<vec2i> mlDimensionSizes;

ushort mlDatapackCounter = 0;

//		CONVERSION TABLES

mlSecondaryConversionTableContainer mlSecondConvert;

//				MAP CONTENT LOADING

// loads background layer using conversion tables
void mlLoadDimensionBackground(backgroundLayer* pointer, std::ifstream* file, vec2i size, dimension* dimPointer) {
	if (mlDebug) {
		std::cout << "ML debug: Loading background blocks \n";
	}
	for (uint y = 0; y < size.y; y++) {
		for (uint x = 0; x < size.x; x++) {
			ulong temp;
			*file >> temp;

			// variant is random for now, to be changed later
			dimPointer->grid[y][x].createBackground(&mlDatapack->datapacks[mlSecondConvert.background[temp].datapackNumber].backgroundBlocks[mlSecondConvert.background[temp].objectID].variants[rand()% mlDatapack->datapacks[mlSecondConvert.background[temp].datapackNumber].backgroundBlocks[mlSecondConvert.background[temp].objectID].variants.size()]);

		}
	}
}

// loads floor layer using conversion tables
void mlLoadDimensionFloor(floorLayer* pointer, std::ifstream* file, vec2i size, dimension* dimPointer) {
	if (mlDebug) {
		std::cout << "ML debug: Loading floor blocks \n";
	}
	for (uint y = 0; y < size.y; y++) {
		for (uint x = 0; x < size.x; x++) {
			ulong temp;
			*file >> temp;
			// variant is random for now, to be changed
			dimPointer->grid[y][x].createFloor(&mlDatapack->datapacks[mlSecondConvert.floor[temp].datapackNumber].floorBlocks[mlSecondConvert.floor[temp].objectID].variants[rand() % mlDatapack->datapacks[mlSecondConvert.floor[temp].datapackNumber].floorBlocks[mlSecondConvert.floor[temp].objectID].variants.size()]);

		}
	}
}

// loads wall layer using conversion tables
void mlLoadDimensionWall(wallLayer* pointer, std::ifstream* file, vec2i size, dimension* dimPointer) {
	if (mlDebug) {
		std::cout << "ML debug: Loading wall blocks \n";
	}
	for (uint y = 0; y < size.y; y++) {
		for (uint x = 0; x < size.x; x++) {
			ulong temp;
			*file >> temp;
			// variant is random for now, to be changed
			dimPointer->grid[y][x].createWall(&mlDatapack->datapacks[mlSecondConvert.wall[temp].datapackNumber].wallBlocks[mlSecondConvert.wall[temp].objectID].variants[rand() % mlDatapack->datapacks[mlSecondConvert.wall[temp].datapackNumber].wallBlocks[mlSecondConvert.wall[temp].objectID].variants.size()]);
		}
	}
}

// main function to load a certain dimension
void mlLoadDimension(dimension* pointer) {

	std::ifstream file;
	file.open("saves/" + mlSaveName + "/dimensions/" + pointer->name + ".txt");
	if (file.is_open()) {
		if (mlDebug) {
			std::cout << "[ MILESTONE ] ML debug: Loading dimension: " << pointer->name << "\n";
		}

		if (mlDebug) {
			std::cout << "ML debug: Loading dimension background... " << "\n";
		}

		mlLoadDimensionBackground(&pointer->backgrounds, &file, vec2i(pointer->backgrounds.blocks[0].size(), pointer->backgrounds.blocks.size()), pointer);

		if (mlDebug) {
			std::cout << "ML debug: Loading dimension floors... " << "\n";
		}

		mlLoadDimensionFloor(&pointer->floors, &file, vec2i(pointer->floors.blocks[0].size(), pointer->floors.blocks.size()), pointer);

		if (mlDebug) {
			std::cout << "ML debug: Loading dimension walls... " << "\n";
		}

		mlLoadDimensionWall(&pointer->walls, &file, vec2i(pointer->walls.blocks[0].size(), pointer->walls.blocks.size()), pointer);

		file.close();
	}

	
}

// prepares background block states
void mlPrepareDimensionBackground(backgroundLayer* pointer, vec2i size) {
	for (uint y = 0; y < size.y; y++) {
		for (uint x = 0; x < size.x; x++) {
			pointer->blocks[y][x].create();
		}
	}
}

// prepares floor block states
void mlPrepareDimensionFloor(floorLayer* pointer, vec2i size) {
	for (uint y = 0; y < size.y; y++) {
		for (uint x = 0; x < size.x; x++) {
			pointer->blocks[y][x].create();
		}
	}
}

// prepares wall block states
void mlPrepareDimensionWall(wallLayer* pointer, vec2i size) {
	for (uint y = 0; y < size.y; y++) {
		for (uint x = 0; x < size.x; x++) {
			pointer->blocks[y][x].create();
		}
	}
}

// main function for second map block loading
void mlPrepareBlockStates(dimension* pointer, bool debug) {
	if (debug) {
		std::cout << "[ MILESTONE ] ML debug: Preparing block states for dimension: " << pointer->name << "\n";
	}

	if (debug) {
		std::cout << "ML debug: Preparing dimension background... " << "\n";
	}

	mlPrepareDimensionBackground(&pointer->backgrounds, vec2i(pointer->size.x, pointer->size.y));

	if (debug) {
		std::cout << "ML debug: Preparing dimension floors... " << "\n";
	}

	mlPrepareDimensionFloor(&pointer->floors, vec2i(pointer->size.x, pointer->size.y));

	if (debug) {
		std::cout << "ML debug: Preparing dimension walls... " << "\n";
	}

	mlPrepareDimensionWall(&pointer->walls, vec2i(pointer->size.x, pointer->size.y));
}

// prepares tables in mapContainer
void mlPrepareMapTables() {

	mlMap->create(mlDimensionSizes, mlDimensionNames, mlSaveName);

}

// loads data about saved dimensions
void mlLoadDimensionData() {
	std::ifstream file;
	file.open("saves/" + mlSaveName + "/dimensionList.txt");
	if (file.is_open()) {
		vec2i size;
		for (str input; std::getline(file, input);) {
			mlDimensionNames.push_back(input);
			file >> size.x >> size.y;
			mlDimensionSizes.push_back(size);
			// move to next line
			std::getline(file, input);
			if (mlDebug) {
				std::cout << "ML debug: Loaded dimension name: " << input << "\n";
				std::cout << "ML debug: Loaded dimension size: " << size.x << " " << size.y << "\n";
			}
		}

		file.close();
	}
	else if (mlDebug) {
		std::cout << "ML debug: Could not find dimension list!" << "\n";
	}
}

// main function to start loading map dimensions
void mlLoadDimensions() {

	mlLoadDimensionData();

	mlPrepareMapTables();


	for (ushort n = 0; n < mlMap->dimensions.size(); n++) {

		mlLoadDimension(&mlMap->dimensions[n]);

		mlPrepareBlockStates(&mlMap->dimensions[n], mlDebug);
	}

}

//				SECOND ROW CONVERSION TABLES

void mlLoadSecondaryBackgrounds(mlConversionTableContainer* convert) {
	std::ifstream file;
	file.open("saves/" + mlSaveName + "/conversionBackground.txt");
	mlObjectID tempData;
	ulong number;
	for (ulong n = 0; file >> number >> tempData.datapackNumber >> tempData.name; n++) {
		if (n != number and mlDebug) {
			std::cout << "[ WARNING ] ML debug: Background conversion id match problem occured: " << number << " != " << n << "\n";
		}

		mlSecondConvert.background.push_back(convert->background[tempData]);
		if (mlDebug) {
			std::cout << "ML debug: Background conversion: " << tempData.datapackNumber << " " << tempData.name << " -> " << convert->background[tempData].datapackNumber << " " << convert->background[tempData].objectID << "\n";
		}

	}
	file.close();
}

void mlLoadSecondaryFloors(mlConversionTableContainer* convert) {
	std::ifstream file;
	file.open("saves/" + mlSaveName + "/conversionFloor.txt");
	mlObjectID tempData;
	ulong number;
	for (ulong n = 0; file >> number >> tempData.datapackNumber >> tempData.name; n++) {
		if (n != number and mlDebug) {
			std::cout << "[ WARNING ] ML debug: Floor conversion id match problem occured: " << number << " != " << n << "\n";
		}

		mlSecondConvert.floor.push_back(convert->floor[tempData]);
		if (mlDebug) {
			std::cout << "ML debug: Floor conversion: " << tempData.datapackNumber << " " << tempData.name << " -> " << convert->floor[tempData].datapackNumber << " " << convert->floor[tempData].objectID << "\n";
		}
	}
	file.close();
}

void mlLoadSecondaryWalls(mlConversionTableContainer* convert) {
	std::ifstream file;
	file.open("saves/" + mlSaveName + "/conversionWall.txt");
	mlObjectID tempData;
	ulong number;
	for (ulong n = 0; file >> number >> tempData.datapackNumber >> tempData.name; n++) {
		if (n != number and mlDebug) {
			std::cout << "[ WARNING ] ML debug: Wall conversion id match problem occured: " << number << " != " << n << "\n";
		}

		mlSecondConvert.wall.push_back(convert->wall[tempData]);
		if (mlDebug) {
			std::cout << "ML debug: Wall conversion: " << tempData.datapackNumber << " " << tempData.name << " -> " << convert->wall[tempData].datapackNumber << " " << convert->wall[tempData].objectID << "\n";
		}

	}
	file.close();
}

// main function to load second row conversion tables
void mlLoadSecondRowConversionTables(mlConversionTableContainer* convert) {

	mlLoadSecondaryBackgrounds(convert);

	mlLoadSecondaryFloors(convert);

	mlLoadSecondaryWalls(convert);
}

//				CONVERSION TABLES

// loads background data from a certain datapack
void mlConvertSingleBackgrounds(ushort ingameID, ushort mlID, datapack* dataPoint, mlConversionTableContainer* mlConvert) {
	for (uint n = 0; n < dataPoint->backgroundBlocks.size(); n++) {
		mlObjectID tempID;
		tempID.datapackNumber = mlID;
		if (!n) {
			tempID.name = "air";
		}
		else {
			tempID.name = dataPoint->backgroundBlocks[n].name;
		}

		mlObjectData tempData;
		tempData.datapackNumber = ingameID;
		tempData.objectID = n;

		mlConvert->background[tempID] = tempData;

		if (mlDebug) {
			std::cout << "ML debug: Added background conversion: " << tempID.datapackNumber << " " << tempID.name << " -> " << tempData.datapackNumber << " " << tempData.objectID << "\n";
		}
	}
}

// loads backgrounds from all datapacks
void mlConvertBackgrounds(mlConversionTableContainer* mlConvert) {
	for (ushort n = 0; n < mlDatapackCheck.size(); n++) {
		if (mlDatapackCheck[n].isPresent) {
			// all such objects from a certain datapack

			mlConvertSingleBackgrounds(mlDatapackCheck[n].ingameNumber, n, &mlDatapack->datapacks[mlDatapackCheck[n].ingameNumber], mlConvert);
		}
	}
}

// loads wall data from a certain datapack
void mlConvertSingleWalls(ushort ingameID, ushort mlID, datapack* dataPoint, mlConversionTableContainer* mlConvert) {
	for (uint n = 0; n < dataPoint->wallBlocks.size(); n++) {
		mlObjectID tempID;
		tempID.datapackNumber = mlID;
		if (!n) {
			tempID.name = "air";
		}
		else {
			tempID.name = dataPoint->wallBlocks[n].name;
		}

		mlObjectData tempData;
		tempData.datapackNumber = ingameID;
		tempData.objectID = n;

		mlConvert->wall[tempID] = tempData;

		if (mlDebug) {
			std::cout << "ML debug: Added wall conversion: " << tempID.datapackNumber << " " << tempID.name << " -> " << tempData.datapackNumber << " " << tempData.objectID << "\n";
		}
	}
}

// loads walls from all datapacks
void mlConvertWalls(mlConversionTableContainer* mlConvert) {
	for (ushort n = 0; n < mlDatapackCheck.size(); n++) {
		if (mlDatapackCheck[n].isPresent) {
			// all such objects from a certain datapack

			mlConvertSingleWalls(mlDatapackCheck[n].ingameNumber, n, &mlDatapack->datapacks[mlDatapackCheck[n].ingameNumber], mlConvert);
		}
	}
}

// loads floor data from a certain datapack
void mlConvertSingleFloors(ushort ingameID, ushort mlID, datapack* dataPoint, mlConversionTableContainer* mlConvert) {
	for (uint n = 0; n < dataPoint->floorBlocks.size(); n++) {
		mlObjectID tempID;
		tempID.datapackNumber = mlID;
		if (!n) {
			tempID.name = "air";
		}
		else {
			tempID.name = dataPoint->floorBlocks[n].name;
		}

		mlObjectData tempData;
		tempData.datapackNumber = ingameID;
		tempData.objectID = n;

		mlConvert->floor[tempID] = tempData;

		if (mlDebug) {
			std::cout << "ML debug: Added floor conversion: " << tempID.datapackNumber << " " << tempID.name << " -> " << tempData.datapackNumber << " " << tempData.objectID << "\n";
		}
	}
}

// loads floors from all datapacks
void mlConvertFloors(mlConversionTableContainer* mlConvert) {
	for (ushort n = 0; n < mlDatapackCheck.size(); n++) {
		if (mlDatapackCheck[n].isPresent) {
			// all such objects from a certain datapack

			mlConvertSingleFloors(mlDatapackCheck[n].ingameNumber, n, &mlDatapack->datapacks[mlDatapackCheck[n].ingameNumber], mlConvert);
		}
	}
}

// creates a map of currently loaded names
void mlConvertLoadedNames(mlConversionTableContainer* mlConvert) {

	mlConvertBackgrounds(mlConvert);

	mlConvertWalls(mlConvert);

	mlConvertFloors(mlConvert);
	
}

// main function for conversion table loading
void mlLoadConversionTables() {

	mlConversionTableContainer mlConvert;

	mlConvertLoadedNames(&mlConvert);

	if (mlDebug) {
		std::cout << "[ MILESTONE ] ML debug: Creating second row conversion tables..." << "\n";
	}

	mlLoadSecondRowConversionTables(&mlConvert);
}

//				DATAPACKS CHECK

// loads datapack names to memory
void mlLoadDatapacksPresent() {
	std::ifstream file;
	file.open("saves/" + mlSaveName + "/datapackInfo.txt");
	for (datapackCheckStruct input; file >> input.name;) {
		mlDatapackCheck.push_back(input);
	}
	file.close();
}

// checks which datapacks are present
void mlCheckDatapackPresence() {
	for (ushort n = 0; n < mlDatapackCheck.size(); n++) {
		for (ushort m = 0; m < mlDatapack->datapacks.size(); m++) {
			if (mlDatapack->datapacks[m].name == mlDatapackCheck[n].name) {
				mlDatapackCheck[n].ingameNumber = m;
				mlDatapackCheck[n].isPresent = true;
				mlDatapackCounter++;
				break;
			}
		}
	}

	if (mlDatapackCounter < mlDatapackCheck.size() and  mlDebug) {
		std::cout << "[ WARNING ] ML debug: Some datapacks of the selected save file are not currently present!" << "\n";

		for (ushort n = 0; n < mlDatapackCheck.size(); n++) {
			if (!mlDatapackCheck[n].isPresent) {
				std::cout << "[ WARNING ] ML debug: Missing datapack: " << mlDatapackCheck[n].name << "! \n";
			}
		}
	}
	else if (mlDebug) {
		std::cout << "ML debug: All datapacks from save file present!" << "\n";
	}
}

// main datapack check function
void mlCheckpacks() {
	mlLoadDatapacksPresent();

	mlCheckDatapackPresence();
}

//				MAIN FUNCTION

void loadMap(mapContainer* pointer, const str saveName, datapackContainer* datapackPointerForLoading, bool isDebugActive = false) {
	mlMap = pointer;
	mlSaveName = saveName;
	mlDatapack = datapackPointerForLoading;
	mlDebug = isDebugActive;

	if (mlDebug) {
		std::cout << "[ STARTING ] ML debug: Starting map loading from: saves/" << saveName << "\n";
	}

	// datapack check
	mlCheckpacks();

	if (mlDebug) {
		std::cout << "[ MILESTONE ] ML debug: Creating conversion tables... \n";
	}

	// conversion tables
	mlLoadConversionTables();

	if (mlDebug) {
		std::cout << "[ MILESTONE ] ML debug: Loading dimensions... \n";
	}

	// loading dimensions
	mlLoadDimensions();

	if (mlDebug) {
		std::cout << "[ MILESTONE ] ML debug: Preparing render grids... \n";
	}

	// all other things to do before finishing up, invontories, players, etc

	if (mlDebug) {
		std::cout << "[ DONE ] ML debug: Map loaded! \n";
	}
}