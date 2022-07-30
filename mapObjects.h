#pragma once
#include "shared.h"
#include "localLight.h"

// this file contains things related to objects displayed on the map
// objects are to be stored in chunks but not displayed as static objects, they shall be animated
// objects will have their centers at x/2 and y/2, these coords are divided like ints so for example: 2 / 2 = 1, 1 / 2 = 0, 3 / 2 = 1
// note that upper left corners will have coords (0,0)
// this means that center wil be in the middle with an upper left bias like:
//	O O O O  O O O  C O  C O  etc.
//  O C O O  O C O  O O
//  O O O O  O O O
//  O O O O



// this map will contain names of paths to replace and the paths that replace them for easy replacement
std::unordered_map<std::string, std::string> objectReplaceMap;

// stores a single frame of an object and it's info
struct objectInfoVariantFrame {
	bool doesHaveLight = false;
	std::vector<localLightSourceInfo> lights;
	sf::IntRect rectangle = sf::IntRect(0, 0, 0, 0);
};

// stores a variant of an object
struct objectInfoVariant {

	std::vector<objectInfoVariantFrame> frameInfos;
	sf::Texture texture;
	sf::Texture shadeTexture;
	sf::Vector2i offset = sf::Vector2i(0, 0);
	sf::Vector2f scale = sf::Vector2f(1, 1);

	bool isAnimated = false;
	// this one means that the animation is not dependent on the state of the object, for example smelting something
	bool isAnimationFree = true;
	// amount of ticks between changing frame to next
	unsigned short frameTicks = 0;

};

// main object class
class objectInfo {
private:
	
	// variants of the objects, multiple will be required mostly for trees and such
	std::vector<objectInfoVariant> variants;

public:

	// the only function that needs to open object files
	// before using it get amount of object definitions from files in their respective folders
	void loadFromFile(std::string definitionPath) {
		

		
	}
};

// class that stores an object in a chunk, containing things like animations
class objectRenderContainer {
private:
	objectInfoVariant* pointer = nullptr;
	unsigned short currentFrame = 0;
	unsigned short currentTick = 0;
	sf::Sprite sprite, shadeSprite;

	// sets current frame textures
	void setFrame() {
		sprite.setTextureRect(pointer->frameInfos[currentFrame].rectangle);
		shadeSprite.setTextureRect(pointer->frameInfos[currentFrame].rectangle);
	}

public:

	// creates an object 
	void create(objectInfoVariant* objectPtr, sf::Vector2i coords) {
		pointer = objectPtr;
		currentFrame = 0;
		currentTick = 0;
		
		sprite.setPosition(blockBaseSize * coords.x, blockBaseSize * coords.y);
		shadeSprite.setPosition(blockBaseSize * coords.x, blockBaseSize * coords.y);

		sprite.setTexture(pointer->texture);
		shadeSprite.setTexture(pointer->shadeTexture);

		sprite.setOrigin(sf::Vector2f(pointer->offset));
		shadeSprite.setOrigin(sf::Vector2f(pointer->offset));

		setFrame();
	}

	// adds a tick to the object and if required changes frames
	void addTick() {

		if (pointer->isAnimated and pointer->isAnimationFree) {
			currentTick++;
			if (currentTick == pointer->frameTicks) {
				currentFrame++;
				currentTick = 0;
				if (currentFrame == pointer->frameInfos.size()) {
					currentFrame = 0;
				}
				setFrame();
			}
		}
	}

	void draw(sf::RenderTexture* texture) {
		texture->draw(sprite);
	}

	void drawShade(sf::RenderTexture* texture) {
		texture->draw(shadeSprite);
	}
};