#pragma once

#include <string>
#include <vector>

#include "quickWrite.h"

// a file containing map generator specific functions



// a basic map generator block data struct
struct MGBlock {
	ushort datapackNumber = 0;
	uint blockNumber = 0;
};

// quick get function
MGBlock getMGBlock(ushort datapack, uint number) {
	MGBlock temp;
	temp.datapackNumber = datapack;
	temp.blockNumber = number;
	return temp;
}

// a struct containing ids for conversion to save file
struct MGDatapack {
	std::vector<ulong> backgroundConversion;
	std::vector<ulong> floorConversion;
	std::vector<ulong> wallConversion;
};

// a map generator layer container
class MGLayer {
public:
	std::vector<std::vector<MGBlock>> blocks;
	
	void create(vec2i size) {
		blocks.resize(size.y);
		for (ushort n = 0; n < size.y; n++) {
			blocks[n].resize(size.x);
		}
	}
};

// a map generator dimension container
class MGDimension {
public:
	MGLayer background, floor, wall;
	str name;
	vec2i storedSize;

	void create (vec2i size,  str *dimensionName){
		name = *dimensionName;
		storedSize = size;

		background.create(size);
		floor.create(size);
		wall.create(size);
	}
};

// a map generator map container
class MGMap {
public:
	std::vector<MGDimension> dimensions;

	void create(std::vector<vec2i> sizes, std::vector<str> names) {
		dimensions.resize(sizes.size());
		for (ushort n = 0; n < sizes.size(); n++) {
			dimensions[n].create(sizes[n], &names[n]);
		}
	}
};