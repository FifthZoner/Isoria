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

//				CLASSES

// class containing background layer of a dimension
class backgroundLayer {
public:
	std::vector <std::vector<backgroundBlock>> blocks;
};

// class containing floor layer of a dimension
class floorLayer {
public:
	std::vector <std::vector<floorBlock>> blocks;
};

// class containing wall layer of a dimension
class wallLayer {
public:
	std::vector <std::vector<wallBlock>> blocks;
};

// class containing what to draw to optimize rendering
class renderMap {
public:
	std::vector<std::vector<renderStruct>> grid;
};

// class containing a whole dimension
class dimension {
public:
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

// class containing whole map
class mapContainer {
public:
	std::vector <dimension> dimensions;

	void create(std::vector<vec2i> sizes, std::vector<str> names) {

		// prepares dimensions
		dimensions.resize(names.size());
		for (ushort n = 0; n < names.size(); n++) {
			dimensions[n].create(sizes[n], names[n]);
		}
	}
};