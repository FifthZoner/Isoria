#pragma once

// a file containing definitions of classes and structs regarding items, blocks etc

#include <string>
#include <SFML/Graphics.hpp>

#include "quickWrite.h"
#include "convert.h"
#include "shared.h"

//				DEFINITION

//		CLASSES

// a class containing information about given background block
class backgroundBlockInfo {
public:
	std::string name;
	std::vector <sf::Sprite> sprites;
	std::vector <sf::Texture> textures;
	vec2f scaleToSet;
	bool doesObstruct = false;
	bool isVisible = false;
	unsigned short datapackId, internalId;

	void create(std::vector <str> paths, ushort baseBlockSize, unsigned short idOfDatapack, unsigned short idInDatapack) {
		datapackId = idOfDatapack;
		internalId = idInDatapack;

		sprites.resize(paths.size());

		for (ushort n = 0; n < paths.size(); n++) {
			textures[n].loadFromFile(paths[n]);
			sprites[n].setTexture(textures[n]);
			sprites[n].setScale(vec2f(float(baseBlockSize) / float(textures[n].getSize().x), float(baseBlockSize) / float(textures[n].getSize().y)));

			// background blocks are excluded from shading to improve performance
		}
		doesObstruct = true;
		isVisible = true;

		scaleToSet = vec2f(float(baseBlockSize) / float(textures[0].getSize().x), float(baseBlockSize) / float(textures[0].getSize().y));
	}
};

// a class containing information about given floor block
class floorBlockInfo {
public:
	std::string name;
	std::vector <sf::Sprite> sprites;
	std::vector <sf::Sprite> shadeSprites;
	std::vector <sf::Texture> textures;
	std::vector <sf::Texture> shadeTextures;
	vec2f scaleToSet;
	bool doesObstruct = false;
	bool isVisible = false;
	unsigned short datapackId, internalId;

	void create(std::vector <str> paths, ushort baseBlockSize, unsigned short idOfDatapack, unsigned short idInDatapack) {
		datapackId = idOfDatapack;
		internalId = idInDatapack;

		sprites.resize(paths.size());
		shadeSprites.resize(paths.size());

		for (ushort n = 0; n < paths.size(); n++) {
			textures[n].loadFromFile(paths[n]);
			sprites[n].setTexture(textures[n]);
			sprites[n].setScale(vec2f(float(baseBlockSize) / float(textures[n].getSize().x), float(baseBlockSize) / float(textures[n].getSize().y)));

			// gets shade file in format xxxxShade.(!)png(!)
			std::string path = paths[n].erase(paths[n].size() - 4, 4);
			path += "Shade.png";
			shadeTextures[n].loadFromFile(path);
			shadeSprites[n].setTexture(shadeTextures[n]);
			shadeSprites[n].setScale(vec2f(float(baseBlockSize) / float(shadeTextures[n].getSize().x), float(baseBlockSize) / float(shadeTextures[n].getSize().y)));
		}
		doesObstruct = true;
		isVisible = true;

		scaleToSet = vec2f(asFloat(baseBlockSize) / asFloat(textures[0].getSize().x), asFloat(baseBlockSize) / asFloat(textures[0].getSize().y));
	}
};

// a class containing information about given wall block
class wallBlockInfo {
public:
	std::string name;
	std::vector <sf::Sprite> sprites;
	std::vector <sf::Sprite> shadeSprites;
	std::vector <sf::Texture> textures;
	std::vector <sf::Texture> shadeTextures;
	vec2f scaleToSet;
	bool doesObstruct = false;
	bool isVisible = false;
	unsigned short datapackId, internalId;

	void create(std::vector <str> paths, ushort baseBlockSize, unsigned short idOfDatapack, unsigned short idInDatapack) {
		datapackId = idOfDatapack;
		internalId = idInDatapack;

		sprites.resize(paths.size());
		shadeSprites.resize(paths.size());

		for (ushort n = 0; n < paths.size(); n++) {
			textures[n].loadFromFile(paths[n]);
			sprites[n].setTexture(textures[n]);
			sprites[n].setScale(vec2f(float(baseBlockSize) / float(textures[n].getSize().x), float(baseBlockSize) / float(textures[n].getSize().y)));

			// gets shade file in format xxxxShade.(!)png(!)
			std::string path = paths[n].erase(paths[n].size() - 4, 4);
			path += "Shade.png";
			shadeTextures[n].loadFromFile(path);
			shadeSprites[n].setTexture(shadeTextures[n]);
			shadeSprites[n].setScale(vec2f(float(baseBlockSize) / float(shadeTextures[n].getSize().x), float(baseBlockSize) / float(shadeTextures[n].getSize().y)));
		}
		doesObstruct = true;
		isVisible = true;

		scaleToSet = vec2f(asFloat(baseBlockSize) / asFloat(textures[0].getSize().x), asFloat(baseBlockSize) / asFloat(textures[0].getSize().y));
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
public:

	backgroundBlockInfo* pointer;
	bool isVisible = false;
	unsigned short variant = 0;
	sf::Vector2f position = sf::Vector2f(0, 0);

	// creates given background block at given coordinates
	void prepare(backgroundBlockInfo* blockInfoPointer, sf::Vector2i gridCoords, ushort baseSize) {
		pointer = blockInfoPointer;
		position.x = gridCoords.x * blockBaseSize;
		position.y = gridCoords.y * blockBaseSize;
	}

	// sets right texture depending on neighboring blocks etc
	void create(ushort state = 0) {
		variant = state;
	}
};

// a class containing information about a single floor block
class floorBlock {
public:

	floorBlockInfo* pointer;
	bool isVisible = false;
	unsigned short variant = 0;
	sf::Vector2f position = sf::Vector2f(0, 0);


	// creates given background block at given coordinates
	void prepare(floorBlockInfo* blockInfoPointer, sf::Vector2i gridCoords, ushort baseSize) {
		pointer = blockInfoPointer;
		position.x = gridCoords.x * blockBaseSize;
		position.y = gridCoords.y * blockBaseSize;

	}

	// sets right texture depending on neighboring blocks etc
	void create(ushort state = 0) {
		variant = state;
	}
};

// a class containing information about a single wall block
class wallBlock {
public:
	wallBlockInfo* pointer;
	bool isVisible = false;
	unsigned short variant = 0;
	sf::Vector2f position = sf::Vector2f(0, 0);


	// creates given background block at given coordinates
	void prepare(wallBlockInfo* blockInfoPointer, sf::Vector2i gridCoords, ushort baseSize) {
		pointer = blockInfoPointer;
		position.x = gridCoords.x * blockBaseSize;
		position.y = gridCoords.y * blockBaseSize;
	}


	// sets right texture depending on neighboring blocks etc
	void create(ushort state = 0) {
		variant = state;
	}
};