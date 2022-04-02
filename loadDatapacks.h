#pragma once

// a file containing datapack loading functions

#include <string>
#include <vector>
#include <fstream>
#include <SFML/Graphics.hpp>

#include <iostream>

#include "data.h"
#include "quickWrite.h"

datapackContainer* container;
lDatapackPathsContainer* paths;
ushort dlBaseSize = 16;
bool dlDebug = 1;

//				PRELOADING

// function to load backgrounds of added in a datapack
void dlLoadAddedBackgrounds(str path, lDatapackPaths* pPointer, datapack* dPointer) {
	for (const auto& entry : fs::directory_iterator(path)) {
		backgroundBlockInfo temp;

		std::ifstream file;
		file.open(entry.path().string());

		// loads data about name
		str name = entry.path().string();
		// cuts path from block name
		name = name.erase(name.find('.'), name.size() - name.find('.')).erase(0, name.find_last_of('/') + 1);

		temp.name = name;

		if (dlDebug) {
			std::cout << "DL debug: Loaded background named: " << name << "\n";
		}
		

		for (str input; file >> input;) {
			if (input == "path") {
				ushort number;
				file >> number;
				std::getline(file, input);
				std::vector<str> tempPaths;
				for (ushort n = 0; n < number; n++) {
					std::getline(file, input);
					tempPaths.push_back(input);
				}
				pPointer->background.texturePaths.push_back(tempPaths);
				temp.textures.resize(tempPaths.size());
				if (dlDebug) {
					std::cout << "DL debug: Loaded background texture paths:" << "\n";
					for (ushort n = 0; n < number; n++) {
						std::cout << "DL debug:" << tempPaths[n] << "\n";
					}
				}
			}
		}
		dPointer->backgroundBlocks.push_back(temp);

		file.close();
	}
}

// function to load floors of added in a datapack
void dlLoadAddedFloors(str path, lDatapackPaths* pPointer, datapack* dPointer) {
	for (const auto& entry : fs::directory_iterator(path)) {
		floorBlockInfo temp;

		std::ifstream file;
		file.open(entry.path().string());

		// loads data about name
		str name = entry.path().string();
		// cuts path from block name
		name = name.erase(name.find('.'), name.size() - name.find('.')).erase(0, name.find_last_of('/') + 1);

		temp.name = name;
		
		if (dlDebug) {
			std::cout << "DL debug: Loaded floor named: " << name << "\n";
		}

		for (str input; file >> input;) {
			if (input == "path") {
				ushort number;
				file >> number;
				std::getline(file, input);
				std::vector<str> tempPaths;
				for (ushort n = 0; n < number; n++) {
					std::getline(file, input);
					tempPaths.push_back(input);
				}
				pPointer->floor.texturePaths.push_back(tempPaths);
				temp.textures.resize(tempPaths.size());
				if (dlDebug) {
					std::cout << "DL debug: Loaded floor texture paths:" << "\n";
					for (ushort n = 0; n < number; n++) {
						std::cout << "DL debug:" << tempPaths[n] << "\n";
					}
				}
			}
		}
		dPointer->floorBlocks.push_back(temp);

		file.close();
	}
}

// function to load walls of added in a datapack
void dlLoadAddedWalls (str path, lDatapackPaths* pPointer, datapack* dPointer) {
	for (const auto& entry : fs::directory_iterator(path)) {
		wallBlockInfo temp;

		std::ifstream file;
		file.open(entry.path().string());

		// loads data about name
		str name = entry.path().string();
		// cuts path from block name
		name = name.erase(name.find('.'), name.size() - name.find('.')).erase(0, name.find_last_of('/') + 1);

		temp.name = name;
		
		if (dlDebug) {
			std::cout << "DL debug: Loaded wall named: " << name << "\n";
		}

		for (str input; file >> input;) {
			if (input == "path") {
				ushort number;
				file >> number;
				std::getline(file, input);
				std::vector<str> tempPaths;
				for (ushort n = 0; n < number; n++) {
					std::getline(file, input);
					tempPaths.push_back(input);
				}
				pPointer->wall.texturePaths.push_back(tempPaths);
				temp.textures.resize(tempPaths.size());
				if (dlDebug) {
					std::cout << "DL debug: Loaded wall texture paths:" << "\n";
					for (ushort n = 0; n < number; n++) {
						std::cout << "DL debug:" << tempPaths[n] << "\n";
					}
				}
			}
		}
		dPointer->wallBlocks.push_back(temp);

		file.close();
	}
}

// loading added folder of a datapack
void dlLoadAdded(str path, lDatapackPaths* pPointer, datapack* dPointer) {

	// backgrounds
	if (fs::is_directory(path + "backgrounds")) {
		if (dlDebug) {
			std::cout << "DL debug: Background folder present in " << path << " directory" << "\n";
		}

		dlLoadAddedBackgrounds(path + "backgrounds/", pPointer, dPointer);
	}

	// floors
	if (fs::is_directory(path + "floors")) {
		if (dlDebug) {
			std::cout << "DL debug: Floor folder present in " << path << " directory" << "\n";
		}

		dlLoadAddedFloors(path + "floors/", pPointer, dPointer);
	}

	// walls
	if (fs::is_directory(path + "walls")) {
		if (dlDebug) {
			std::cout << "DL debug: Wall folder present in " << path << " directory" << "\n";
		}

		dlLoadAddedWalls(path + "walls/", pPointer, dPointer);
	}
}

// main datapack loading function
void dlLoadDatapack(str path, lDatapackPaths* pPointer, datapack* dPointer) {
	if (dlDebug) {
		std::cout << "DL debug: Starting datapack loading from " << path << " directory" << "\n";
	}

	// add 0 index
	dPointer->backgroundBlocks.resize(1);
	dPointer->floorBlocks.resize(1);
	dPointer->wallBlocks.resize(1);

	pPointer->background.texturePaths.resize(1);
	pPointer->floor.texturePaths.resize(1);
	pPointer->wall.texturePaths.resize(1);

	// added
	if (fs::is_directory(path + "added")) {
		if (dlDebug) {
			std::cout << "DL debug: Added folder present in " << path << " directory" << "\n";
		}

		dlLoadAdded(path + "added/", pPointer, dPointer);
	}
}

// main function to load non graphical datapack files and prepare tables for graphics
void preLoadMainGameDatapacks(datapackContainer* datapackVariable, lDatapackPathsContainer* datapackPaths) {
	container = datapackVariable;
	paths = datapackPaths;

	if (dlDebug) {
		std::cout << "[ STARTING ] DL debug: First stage staring..." << "\n";
	}

	std::ifstream file;
	file.open("launcher/datapacks.txt");

	for (str input; file >> input;) {

		// preparing space
		container->datapacks.resize(container->datapacks.size() + 1);
		paths->datapacks.resize(paths->datapacks.size() + 1);

		container->datapacks[container->datapacks.size() - 1].name = input;

		dlLoadDatapack("data/" + input + "/", &paths->datapacks[paths->datapacks.size() - 1], &container->datapacks[container->datapacks.size() - 1]);
	}

	file.close();
}

//				GRAPHICS LOADING

// loads each datapack graphics
void dlLoadDatapackGraphics(lDatapackPaths* pPointer, datapack* dPointer) {

	if (dlDebug) {
		std::cout << "DL debug: Loading datapack graphics for: " << dPointer->name << "\n";
	}

	// backgrounds
	for (uint n = 1; n < pPointer->background.texturePaths.size(); n++) {
		dPointer->backgroundBlocks[n].create(pPointer->background.texturePaths[n], dlBaseSize);
	}

	// floors
	for (uint n = 1; n < pPointer->floor.texturePaths.size(); n++) {
		dPointer->floorBlocks[n].create(pPointer->floor.texturePaths[n], dlBaseSize);
	}

	// walls
	for (uint n = 1; n < pPointer->wall.texturePaths.size(); n++) {
		dPointer->wallBlocks[n].create(pPointer->wall.texturePaths[n], dlBaseSize);
	}
}

// main function for datapack graphics loading
void loadMainGameDatapacks(datapackContainer* datapackVariable, lDatapackPathsContainer* datapackPaths, ushort blockBaseSize = 16) {

	if (dlDebug) {
		std::cout << "[ STARTING ] DL debug: Second stage starting..." << "\n";
	}

	container = datapackVariable;
	paths = datapackPaths;
	dlBaseSize = blockBaseSize;

	if (dlDebug) {
		std::cout << "DL debug: Loading datapack graphics..." << "\n";
	}

	for (ushort n = 0; n < paths->datapacks.size(); n++) {
		dlLoadDatapackGraphics(&paths->datapacks[n], &container->datapacks[n]);
	}
}