#pragma once

// a file containing definitions of classes and structs regarding items, blocks etc

#include <string>
#include <SFML/Graphics.hpp>

#include "quickWrite.h"
#include "convert.h"
#include "shared.h"

//				DEFINITION

struct blockVariantStruct {
	sf::Texture texture;
	sf::Texture shadeTexture;
	char variantNumber = 0;
	bool doesObstruct = false;
	bool isVisible = false;
	vec2f scaleToSet = sf::Vector2f(1, 1);
	vec2f shadeScaleToSet = sf::Vector2f(1, 1);
	
};

// a class containing information about given background block
class backgroundBlockInfo {
public:
	std::string name;
	std::vector <sf::Sprite> sprites;
	std::vector <sf::Texture> textures;

	std::vector <blockVariantStruct> variants;

	vec2f scaleToSet;
	bool doesObstruct = false;
	bool isVisible = false;
	unsigned short datapackId, internalId;

	void create(std::vector <str> paths, ushort baseBlockSize, unsigned short idOfDatapack, unsigned short idInDatapack, bool isAir = false) {
		datapackId = idOfDatapack;
		internalId = idInDatapack;

		
	
			sprites.resize(paths.size());

			for (ushort n = 0; n < paths.size(); n++) {
				// new loading for unified storage
				variants[n].texture.loadFromFile(paths[n]);
				variants[n].variantNumber = n;

				variants[n].scaleToSet = sf::Vector2f(float(baseBlockSize) / float(variants[n].texture.getSize().x), float(baseBlockSize) / float(variants[n].texture.getSize().y));
				// temp
				variants[n].doesObstruct = true;
				variants[n].isVisible = true;


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

	std::vector <blockVariantStruct> variants;

	vec2f scaleToSet;
	bool doesObstruct = false;
	bool isVisible = false;
	unsigned short datapackId, internalId;

	void create(std::vector <str> paths, ushort baseBlockSize, unsigned short idOfDatapack, unsigned short idInDatapack, bool isAir = false) {
		datapackId = idOfDatapack;
		internalId = idInDatapack;

		
	
			sprites.resize(paths.size());
			shadeSprites.resize(paths.size());

			for (ushort n = 0; n < paths.size(); n++) {
				// new loading for unified storage
				variants[n].texture.loadFromFile(paths[n]);


				variants[n].variantNumber = n;

				variants[n].scaleToSet = sf::Vector2f(float(baseBlockSize) / float(variants[n].texture.getSize().x), float(baseBlockSize) / float(variants[n].texture.getSize().y));

				// temp, to be read from file in the future
				variants[n].doesObstruct = true;
				variants[n].isVisible = true;



				textures[n].loadFromFile(paths[n]);
				sprites[n].setTexture(textures[n]);
				sprites[n].setScale(vec2f(float(baseBlockSize) / float(textures[n].getSize().x), float(baseBlockSize) / float(textures[n].getSize().y)));


				// new
				std::string path = paths[n].erase(paths[n].size() - 4, 4);
				path += "Shade.png";
				variants[n].shadeTexture.loadFromFile(path);
				variants[n].shadeScaleToSet = sf::Vector2f(float(baseBlockSize) / float(variants[n].shadeTexture.getSize().x), float(baseBlockSize) / float(variants[n].shadeTexture.getSize().y));



				// gets shade file in format xxxxShade.(!)png(!)
				// path calculation moved
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
	std::string name;std::vector <sf::Sprite> sprites;
	std::vector <sf::Sprite> shadeSprites;
	std::vector <sf::Texture> textures;
	std::vector <sf::Texture> shadeTextures;

	std::vector <blockVariantStruct> variants;

	vec2f scaleToSet;
	bool doesObstruct = false;
	bool isVisible = false;
	unsigned short datapackId, internalId;

	void create(std::vector <str> paths, ushort baseBlockSize, unsigned short idOfDatapack, unsigned short idInDatapack, bool isAir = false) {
		datapackId = idOfDatapack;
		internalId = idInDatapack;

		
			sprites.resize(paths.size());
			shadeSprites.resize(paths.size());

			for (ushort n = 0; n < paths.size(); n++) {
				// new loading for unified storage
				variants[n].texture.loadFromFile(paths[n]);

				variants[n].variantNumber = n;

				variants[n].scaleToSet = sf::Vector2f(float(baseBlockSize) / float(variants[n].texture.getSize().x), float(baseBlockSize) / float(variants[n].texture.getSize().y));

				// temp, to be read from file in the future
				variants[n].doesObstruct = true;
				variants[n].isVisible = true;


				textures[n].loadFromFile(paths[n]);
				sprites[n].setTexture(textures[n]);
				sprites[n].setScale(vec2f(float(baseBlockSize) / float(textures[n].getSize().x), float(baseBlockSize) / float(textures[n].getSize().y)));


				// new
				std::string path = paths[n].erase(paths[n].size() - 4, 4);
				path += "Shade.png";
				variants[n].shadeTexture.loadFromFile(path);
				variants[n].shadeScaleToSet = sf::Vector2f(float(baseBlockSize) / float(variants[n].shadeTexture.getSize().x), float(baseBlockSize) / float(variants[n].shadeTexture.getSize().y));


				// gets shade file in format xxxxShade.(!)png(!)
				// path calculation moved
				shadeTextures[n].loadFromFile(path);
				shadeSprites[n].setTexture(shadeTextures[n]);
				shadeSprites[n].setScale(vec2f(float(baseBlockSize) / float(shadeTextures[n].getSize().x), float(baseBlockSize) / float(shadeTextures[n].getSize().y)));
			}
			doesObstruct = true;
			isVisible = true;

			scaleToSet = vec2f(asFloat(baseBlockSize) / asFloat(textures[0].getSize().x), asFloat(baseBlockSize) / asFloat(textures[0].getSize().y));
		
	}
};



// ultimate rendering solution brought to me by ... me!
struct renderContainer {
	// contains data to display sprites of all types, made to work with hybrid rendering with a fixed amount of these
	// these are allowed to be bigger due to their amount

	sf::Sprite background, floor, floorShade, wall, wallShade;

	bool isBackgroundVisible = false;
	bool isFloorVisible = false;
	bool isWallVisible = false;

	// creates sprites and updates visibility, always give layer pointers in b f w order, coordinates in grid ones without block size
	void create(sf::Vector2i coordinates, blockVariantStruct* backgroundPointer,
		blockVariantStruct* floorPointer, blockVariantStruct* wallPointer) {

		if (backgroundPointer != nullptr) {
			background.setTexture(backgroundPointer->texture);
			background.setPosition(sf::Vector2f(blockBaseSize * coordinates.x, blockBaseSize * coordinates.y));
			background.setScale(backgroundPointer->scaleToSet);
		}
		
		if (floorPointer != nullptr) {
			floor.setTexture(floorPointer->texture);
			floor.setPosition(sf::Vector2f(blockBaseSize * coordinates.x, blockBaseSize * coordinates.y));
			floor.setScale(floorPointer->scaleToSet);

			floorShade.setTexture(floorPointer->shadeTexture);
			floorShade.setPosition(sf::Vector2f(blockBaseSize * coordinates.x, blockBaseSize * coordinates.y));
			floorShade.setScale(floorPointer->shadeScaleToSet);
		}
		
		if (wallPointer != nullptr) {
			wall.setTexture(wallPointer->texture);
			wall.setPosition(sf::Vector2f(blockBaseSize * coordinates.x, blockBaseSize * coordinates.y));
			wall.setScale(wallPointer->scaleToSet);

			wallShade.setTexture(wallPointer->shadeTexture);
			wallShade.setPosition(sf::Vector2f(blockBaseSize * coordinates.x, blockBaseSize * coordinates.y));
			wallShade.setScale(wallPointer->shadeScaleToSet);
		}
		

		// and checking visibility here


		if (wallPointer != nullptr and wallPointer->isVisible and wallPointer->doesObstruct) {
			isWallVisible = true;
			isFloorVisible = false;
			isBackgroundVisible = false;

		}
		else if (floorPointer != nullptr and floorPointer->isVisible and floorPointer->doesObstruct) {

			if (wallPointer == nullptr or !wallPointer->isVisible) {
				isWallVisible = false;
			}
			else {
				isWallVisible = true;
			}

			isFloorVisible = true;
			isBackgroundVisible = false;
		}
		else if (backgroundPointer != nullptr and backgroundPointer->isVisible and backgroundPointer->doesObstruct) {

			if (wallPointer == nullptr or !wallPointer->isVisible) {
				isWallVisible = false;
			}
			else {
				isWallVisible = true;
			}

			if (floorPointer == nullptr or !floorPointer->isVisible) {
				isFloorVisible = false;
			}
			else {
				isFloorVisible = true;
			}

			isBackgroundVisible = true;
		}
		else {

			if (wallPointer == nullptr or !wallPointer->isVisible) {
				isWallVisible = false;
			}
			else {
				isWallVisible = true;
			}

			if (floorPointer == nullptr or !floorPointer->isVisible) {
				isFloorVisible = false;
			}
			else {
				isFloorVisible = true;
			}

			if (backgroundPointer == nullptr or !backgroundPointer->isVisible) {
				isBackgroundVisible = false;
			}
			else {
				isBackgroundVisible = true;
			}
		}
	}
};

// ultimate map storage solution that seems ... just too small, but works!
struct cellContainer {
	
	renderContainer* renderPointer;
	blockVariantStruct* background;
	blockVariantStruct* floor;
	blockVariantStruct* wall;

	void createBackground(blockVariantStruct* pointer) {
		background = pointer;
	}

	void createFloor(blockVariantStruct* pointer) {
		floor = pointer;
	}

	void createWall(blockVariantStruct* pointer) {
		wall = pointer;
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