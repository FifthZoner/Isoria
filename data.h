#pragma once

// a file containing definitions of classes and structs regarding items, blocks etc

#include <string>
#include <SFML/Graphics.hpp>

#include "quickWrite.h"
#include "convert.h"

//				DEFINITION

//		CLASSES

// a class containing information about given background block
class backgroundBlockInfo {
public:
	std::string name;
	std::vector <sf::Texture> textures;
	std::vector <sf::Texture> shadeTextures;
	vec2f scaleToSet;
	bool doesObstruct = false;
	bool isVisible = false;
	unsigned short datapackId, internalId;

	void create(std::vector <str> paths, ushort baseBlockSize, unsigned short idOfDatapack, unsigned short idInDatapack) {
		datapackId = idOfDatapack;
		internalId = idInDatapack;

		for (ushort n = 0; n < paths.size(); n++) {
			textures[n].loadFromFile(paths[n]);

			// gets shade file in format xxxxShade.(!)png(!)
			std::string path = paths[n].erase(paths[n].size() - 4, 4);
			path += "Shade.png";
			shadeTextures[n].loadFromFile(path);
		}
		doesObstruct = true;
		isVisible = true;

		scaleToSet = vec2f(asFloat(baseBlockSize) / asFloat(textures[0].getSize().x) * 2, asFloat(baseBlockSize) / asFloat(textures[0].getSize().y) * 2);
	}
};

// a class containing information about given floor block
class floorBlockInfo {
public:
	std::string name;
	std::vector <sf::Texture> textures;
	std::vector <sf::Texture> shadeTextures;
	vec2f scaleToSet;
	bool doesObstruct = false;
	bool isVisible = false;
	unsigned short datapackId, internalId;

	void create(std::vector <str> paths, ushort baseBlockSize, unsigned short idOfDatapack, unsigned short idInDatapack) {
		datapackId = idOfDatapack;
		internalId = idInDatapack;

		for (ushort n = 0; n < paths.size(); n++) {
			textures[n].loadFromFile(paths[n]);

			// gets shade file in format xxxxShade.(!)png(!)
			std::string path = paths[n].erase(paths[n].size() - 4, 4);
			path += "Shade.png";
			shadeTextures[n].loadFromFile(path);
		}
		doesObstruct = true;
		isVisible = true;

		scaleToSet = vec2f(asFloat(baseBlockSize) / asFloat(textures[0].getSize().x) * 2, asFloat(baseBlockSize) / asFloat(textures[0].getSize().y) * 2);
	}
};

// a class containing information about given wall block
class wallBlockInfo {
public:
	std::string name;
	std::vector <sf::Texture> textures;
	std::vector <sf::Texture> shadeTextures;
	vec2f scaleToSet;
	bool doesObstruct = false;
	bool isVisible = false;
	unsigned short datapackId, internalId;

	void create(std::vector <str> paths, ushort baseBlockSize, unsigned short idOfDatapack, unsigned short idInDatapack) {
		datapackId = idOfDatapack;
		internalId = idInDatapack;

		for (ushort n = 0; n < paths.size(); n++) {
			textures[n].loadFromFile(paths[n]);

			// gets shade file in format xxxxShade.(!)png(!)
			std::string path = paths[n].erase(paths[n].size() - 4, 4);
			path += "Shade.png";
			shadeTextures[n].loadFromFile(path);
		}
		doesObstruct = true;
		isVisible = true;

		scaleToSet = vec2f(asFloat(baseBlockSize) / asFloat(textures[0].getSize().x) * 2, asFloat(baseBlockSize) / asFloat(textures[0].getSize().y) * 2);
	}
};

// a class containing datapack contents
class datapack {
public:
	str name, path;
	unsigned short number = NULL;
	std::vector<backgroundBlockInfo> backgroundBlocks;
	std::vector<floorBlockInfo> floorBlocks;
	std::vector<wallBlockInfo> wallBlocks;
};

// a class for easy pointer access
class datapackContainer {
public:
	std::vector <datapack> datapacks;
};

//		STRUCTS

// a struct for temporary layer path saving
struct lLayerPaths {
	std::vector <std::vector<str>> texturePaths;
};

// a struct for temporary datapack paths
struct lDatapackPaths {
	lLayerPaths background;
	lLayerPaths floor;
	lLayerPaths wall;
};

// a main struct for datapack paths
struct lDatapackPathsContainer {
	std::vector<lDatapackPaths> datapacks;
};

//				SINGLE CLASSES

// a class containing information about a single background block
class backgroundBlock {
private:
	bool isVisible = false;

public:
	backgroundBlockInfo* pointer;
	sf::Sprite sprite;

	// creates given background block at given coordinates
	void prepare(backgroundBlockInfo* blockInfoPointer, sf::Vector2i gridCoords, ushort baseSize) {
		pointer = blockInfoPointer;
		sprite.setPosition(vec2f(gridCoords.x * baseSize, gridCoords.y * baseSize));
		if (pointer->textures.size()) {
			isVisible = true;
		}
	}

	// sets right texture depending on neighboring blocks etc
	void create(ushort state = 0) {
		if (isVisible) {
			sprite.setTexture(pointer->textures[state]);
			sprite.setScale(pointer->scaleToSet);
		}
	}
};

// a class containing information about a single floor block
class floorBlock {
private:
	bool isVisible = false;

public:
	sf::Sprite sprite;
	floorBlockInfo* pointer;

	// creates given background block at given coordinates
	void prepare(floorBlockInfo* blockInfoPointer, sf::Vector2i gridCoords, ushort baseSize) {
		pointer = blockInfoPointer;
		sprite.setPosition(vec2f(gridCoords.x * baseSize, gridCoords.y * baseSize));
		if (pointer->textures.size()) {
			isVisible = true;
		}
	}

	// sets right texture depending on neighboring blocks etc
	void create(ushort state = 0) {
		if (isVisible) {
			sprite.setTexture(pointer->textures[state]);
			sprite.setScale(pointer->scaleToSet);
		}
	}
};

// a class containing information about a single wall block
class wallBlock {
private:
	bool isVisible = false;

public:
	sf::Sprite sprite;
	wallBlockInfo* pointer;

	// creates given background block at given coordinates
	void prepare(wallBlockInfo* blockInfoPointer, sf::Vector2i gridCoords, ushort baseSize) {
		pointer = blockInfoPointer;
		sprite.setPosition(vec2f(gridCoords.x * baseSize, gridCoords.y * baseSize));
		if (pointer->textures.size()) {
			isVisible = true;
		}
	}

	// sets right texture depending on neighboring blocks etc
	void create(ushort state = 0) {
		if (isVisible) {
			sprite.setTexture(pointer->textures[state]);
			sprite.setScale(pointer->scaleToSet);
		}
	}
};