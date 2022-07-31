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
	sf::IntRect rectangle = sf::IntRect(0, 0, 0, 0);
};

struct objectGridStruct {
	// will contain things like lights and inventory info
	bool doesHaveLight = false;
	localLightSourceInfo lightInfo;
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
	sf::Vector2i size = sf::Vector2i(0, 0);
	sf::Vector2i center = sf::Vector2i(0, 0);

	bool isAnimated = false;
	// this one means that the animation is not dependent on the state of the object, for example smelting something, not yet introduced loading
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
};

struct objectGridLoadingStruct {
	sf::Vector2i offset = sf::Vector2i(0, 0);
	bool doesHaveLight = false;
	sf::Color color = sf::Color::Green;
	unsigned short variantNumber = 0;
	std::vector<unsigned short> frameNumbers;
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
		file.open(definitionPath);

		if (file.is_open()) {
			std::vector<objectGridLoadingStruct> gridLoad;
			// all details regarding this loading are in a seperate text file

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

					for (unsigned short n = 0; n < variantAmount; n++) {

						// next line 'cause it's a dumb computer
						std::getline(file, input);
						std::getline(file, input);
						variants[n].texture.loadFromFile(input);
						// gets extension and adds shade to name
						std::string path = input.erase(input.size() - 4, 4);
						path += "Shade.png";
						variants[n].shadeTexture.loadFromFile(path);
						unsigned short amount = 0;
						unsigned short width = 0;
						file >> amount;
						file >> variants[n].frameTicks;
						// if frame length is 0 then it's not animated
						if (variants[n].frameTicks) {
							variants[n].isAnimated = true;
						}
						file >> width;

						variants[n].assignRectangles(width, amount);
					}

				}
				else if (input == "size") {
					// size for later
					unsigned short tempNumber;
					file >> tempNumber >> variants[tempNumber].size.x >> variants[tempNumber].size.y;

				}
				else if (input == "cell") {

					// data regarding non default (that is none) properties of individual cells of a variant
					objectGridLoadingStruct temp;
					file >> temp.variantNumber >> temp.offset.x >> temp.offset.y;
					file >> temp.doesHaveLight;
					if (temp.doesHaveLight) {
						file >> temp.color.r >> temp.color.g >> temp.color.b >> temp.color.a;
						unsigned short amount = 0;
						file >> amount;
						temp.frameNumbers.resize(amount);
						for (unsigned short n = 0; n < amount; n++) {
							file >> temp.frameNumbers[n];
						}
					}
					gridLoad.push_back(temp);
				}
				else if (input == "offset") {

					// object offset from grid, please don't go too crazy with it
					unsigned short tempNumber = 0;
					file >> tempNumber >> variants[tempNumber].offset.x >> variants[tempNumber].offset.y;
				}
			}

			// things that must be done after all parameters were added
			for (unsigned short n = 0; n < variants.size(); n++) {
				variants[n].debugSubName = name + "-" + std::to_string(n);
				variants[n].grid.resize(variants[n].size.y);

				// grid resize
				for (unsigned short y = 0; y < variants[n].size.y; y++) {
					variants[n].grid[y].resize(variants[n].size.x);
				}

				// center for proper placement
				variants[n].center.x = variants[n].size.x / 2;
				variants[n].center.y = variants[n].size.y / 2;
			}
		}
		else {
			debugMsg(std::string("MO Debug: Failed to open object definition at: " + definitionPath));
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

		sprite.setOrigin(sf::Vector2f(pointer->offset.x - (pointer->center.x * blockBaseSize), pointer->offset.y - (pointer->center.y * blockBaseSize)));
		shadeSprite.setOrigin(sf::Vector2f(pointer->offset.x - (pointer->center.x * blockBaseSize), pointer->offset.y - (pointer->center.y * blockBaseSize)));

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