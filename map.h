#pragma once

// a file containing functions related to game map and it's management

#include <vector>
#include <string>
#include <iostream>

#include "quickWrite.h"
#include "data.h"
#include "shared.h"

//				STRUCTS

struct renderStruct {
	bool background;
	bool floor;
	bool wall;
};

// struct containing background layer of a dimension
struct backgroundLayer {
	std::vector <std::vector<backgroundBlock>> blocks;
};

// struct containing floor layer of a dimension
struct floorLayer {
	std::vector <std::vector<floorBlock>> blocks;
};

// struct containing wall layer of a dimension
struct wallLayer {
	std::vector <std::vector<wallBlock>> blocks;
};

// struct containing what to draw to optimize rendering
struct renderMap {
	std::vector<std::vector<renderStruct>> grid;
};

// struct containing a whole dimension
struct dimension {

	backgroundLayer backgrounds;
	floorLayer floors;
	wallLayer walls;

	// to be retired
	renderMap renderGrid;


	vec2i size = vec2i(0, 0);
	str name;

	void create(vec2i dimSize, str nameToSet) {

		name = nameToSet;
		size = dimSize;
		
		// resizes tables
		backgrounds.blocks.resize(size.y);
		floors.blocks.resize(size.y);
		walls.blocks.resize(size.y);
		renderGrid.grid.resize(size.y);
		
		for (ushort n = 0; n < size.y; n++) {
			backgrounds.blocks[n].resize(size.x);
			floors.blocks[n].resize(size.x);
			walls.blocks[n].resize(size.x);
			renderGrid.grid[n].resize(size.x);
		}
		
	}

	void draw(sf::RenderWindow* window, renderLimit border, 
		sf::RenderTexture* shadeTexture, renderLimit shadeBorder) {
		for (ushort y = border.lower.y; y < border.upper.y; y++) {
			for (ushort x = border.lower.x; x < border.upper.x; x++) {
				if (backgrounds.blocks[y][x].isVisible) {
					backgrounds.blocks[y][x].pointer->sprites[backgrounds.blocks[y][x].variant].setPosition(backgrounds.blocks[y][x].position);
					window->draw(backgrounds.blocks[y][x].pointer->sprites[backgrounds.blocks[y][x].variant]);
				}
				if (floors.blocks[y][x].isVisible) {

					floors.blocks[y][x].pointer->sprites[floors.blocks[y][x].variant].setPosition(floors.blocks[y][x].position);

					window->draw(floors.blocks[y][x].pointer->sprites[floors.blocks[y][x].variant]);
					
				}
				if (walls.blocks[y][x].isVisible) {

					walls.blocks[y][x].pointer->sprites[walls.blocks[y][x].variant].setPosition(walls.blocks[y][x].position);

					window->draw(walls.blocks[y][x].pointer->sprites[walls.blocks[y][x].variant]);
				}
			}
		 }

		for (ushort y = shadeBorder.lower.y; y < shadeBorder.upper.y; y++) {
			for (ushort x = shadeBorder.lower.x; x < shadeBorder.upper.x; x++) {
				if (backgrounds.blocks[y][x].isVisible) {
					backgrounds.blocks[y][x].pointer->sprites[backgrounds.blocks[y][x].variant].setPosition(sf::Vector2f(x * blockBaseSize, y * blockBaseSize));
					window->draw(backgrounds.blocks[y][x].pointer->sprites[backgrounds.blocks[y][x].variant]);
				}
				if (floors.blocks[y][x].isVisible) {

					floors.blocks[y][x].pointer->shadeSprites[floors.blocks[y][x].variant].setPosition(sf::Vector2f(x * blockBaseSize, y * blockBaseSize));

					shadeTexture->draw(floors.blocks[y][x].pointer->shadeSprites[floors.blocks[y][x].variant]);


				}
				if (walls.blocks[y][x].isVisible) {

					walls.blocks[y][x].pointer->shadeSprites[walls.blocks[y][x].variant].setPosition(sf::Vector2f(x * blockBaseSize, y * blockBaseSize));

					shadeTexture->draw(walls.blocks[y][x].pointer->shadeSprites[walls.blocks[y][x].variant]);
				}
			}
		}

	}
};

// struct containing whole map
struct mapContainer {
	std::string name = "N/A";
	std::vector <dimension> dimensions;

	//	0 - 24000
	unsigned short time = 8000;

	void create(std::vector<vec2i> sizes, std::vector<std::string> names, std::string savedName, unsigned short savedTime = 8000) {

		time = savedTime;
		name = savedName;

		// prepares dimensions
		dimensions.resize(names.size());
		for (ushort n = 0; n < names.size(); n++) {
			dimensions[n].create(sizes[n], names[n]);
		}
	}
};