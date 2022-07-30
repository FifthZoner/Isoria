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

// stores a single frame of an object and it's info
struct objectInfoVariantFrame {
	bool doesHaveLight = false;
	std::vector<localLightSourceInfo> lights;
	sf::IntRect rectangle = sf::IntRect(0, 0, 0, 0);
};

struct objectGridStruct {
	// not used for now, to be used when inventory or something introduced
};

// stores a variant of an object
struct objectInfoVariant {

	// this will only be used for debug and is normally not to be shown
	std::string debugSubName = "N/A";

	// grid that will containg things like different inputs, inventory slots, etc.
	std::vector<std::vector<objectGridStruct>> grid;

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

	// calculates parts of texture used for each frame, give amount of frames horizontally and amount of them in general
	void assignRectangles(unsigned short width, unsigned short amount) {

		frameInfos.resize(amount);

		sf::Vector2f oneSize = sf::Vector2f(texture.getSize().x / width, texture.getSize().y / int(ceil(float(amount) / float(width))));

		unsigned short row = 0;
		unsigned short column = 0;
		for (unsigned short n = 0; n < amount; n++) {
			column++;

			// if reached the end of the row moves to next one
			if (column == width) {
				row++;
				column = 0;
			}

			frameInfos[n].rectangle = sf::IntRect(column * oneSize.x, row * oneSize.y, oneSize.x, oneSize.y);
		}
	}

	// sets lights in a SINGLE FRAME
	// lights are to be set to individual object grid cells
	void setLights(std::vector<localLightSourceInfo> lights, unsigned short frameNumber) {
		if (lights.size() > 0) {
			frameInfos[frameNumber].lights = lights;
			frameInfos[frameNumber].doesHaveLight = true;
		}
		
	}
};

struct objectGridLoadingStruct {
	sf::Vector2i coords = sf::Vector2i(0, 0);
	bool doesHaveLight = false;
	sf::Color color = sf::Color::Green;
};

// main object class
class objectInfo {
private:
	
	// variants of the objects, multiple will be required mostly for trees and such
	std::vector<objectInfoVariant> variants;

public:
	std::string name = "N/A";

	// the only function that needs to open object files
	// before using it get amount of object definitions from files in their respective folders
	void loadFromFile(std::string definitionPath) {
		
		std::ifstream file;

		std::vector<objectGridLoadingStruct> gridLoad;

		sf::Vector2i size = sf::Vector2i(0, 0);

		for (std::string input; file >> input;) {

			if (input == "name") {
				// name input and to next line cuz it's strange or somethin'
				std::getline(file, input);
				std::getline(file, input);
				name = input;
			}
			else if (input == "path") {
				// loading here
				unsigned short variantAmount = 0;
				file >> variantAmount;

				variants.resize(variantAmount);

				for (unsigned short n = 0; n < variantAmount; n++){
					
					
					std::getline(file, input);
					std::getline(file, input);
					variants[n].texture.loadFromFile(input);
					// gets extension and adds shade to name
					std::string extension = input.erase(0, input.size() - input.find('.'));
					std::string temp = input.erase(input.find('.'), extension.size());
					temp += "Shade" + extension;
					variants[n].shadeTexture.loadFromFile(temp);

					file >> variants[n].frameTicks;

					unsigned short amount = 0;
					unsigned short width = 0;
					file >> amount;
					file >> width;
					
					variants[n].assignRectangles(width, amount);
				}

			}
			else if (input == "size") {
				// size for later
				file >> size.x >> size.y;
			}
			else if (input == "cell") {
				objectGridLoadingStruct temp;
				file >> temp.coords.x >> temp.coords.y;
				file >> temp.doesHaveLight;
				if (temp.doesHaveLight) {
					file >> temp.color.r >> temp.color.g >> temp.color.b >> temp.color.a;
				}
				gridLoad.push_back(temp);
			}
		}



		for (unsigned short n = 0; n < variants.size(); n++) {
			variants[n].debugSubName = name + "-" + std::to_string(n);
			variants[n].grid.resize(size.y);

			// grid resize
			for (unsigned short y = 0; y < size.y; y++) {
				variants[n].grid[y].resize(size.x);
			}
		}


		file.close();
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

		sprite.setScale(pointer->scale);
		shadeSprite.setScale(pointer->scale);

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