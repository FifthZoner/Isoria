#pragma once

// a file containing functions related to game map and it's management

#include <vector>
#include <string>

#include "quickWrite.h"
#include "data.h"

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

	void draw(sf::RenderWindow* window, vec2i lowBorder, vec2i highBorder) {
		for (ushort y = lowBorder.y; y <= highBorder.y; y++) {
			for (ushort x = lowBorder.x; x <= highBorder.x; x++) {
				if (renderGrid.grid[y][x].background) {
					window->draw(backgrounds.blocks[y][x].sprite);
				}
				if (renderGrid.grid[y][x].floor) {
					window->draw(floors.blocks[y][x].sprite);
				}
				if (renderGrid.grid[y][x].wall) {
					window->draw(walls.blocks[y][x].sprite);
				}
			}
		 }
	}
};

// struct containing whole map
struct mapContainer {
	std::string name = "N/A";
	std::vector <dimension> dimensions;

	void create(std::vector<vec2i> sizes, std::vector<std::string> names, std::string name) {

		// prepares dimensions
		dimensions.resize(names.size());
		for (ushort n = 0; n < names.size(); n++) {
			dimensions[n].create(sizes[n], names[n]);
		}
	}
};