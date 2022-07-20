#pragma once

// a file containing functions related to game map and it's management

#include <vector>
#include <string>
#include <iostream>

#include "quickWrite.h"
#include "data.h"
#include "shared.h"



//				STRUCTS




// struct containing a whole dimension
struct dimension {

	std::vector<std::vector<cellContainer>> grid;


	sf::Vector2i size = vec2i(0, 0);
	std::string name;

	void create(sf::Vector2i dimSize, std::string nameToSet) {

		name = nameToSet;
		size = dimSize;
		
		// resizes tables
		grid.resize(size.y);

		for (ushort n = 0; n < size.y; n++) {
			grid[n].resize(size.x);

		}
		
	}

	void draw(sf::RenderTexture* window, renderLimit border,
		sf::RenderTexture* shadeTexture, renderLimit shadeBorder) {
		
		for (ushort y = border.lower.y; y < border.upper.y; y++) {
			for (ushort x = border.lower.x; x < border.upper.x; x++) {
				//std::cout << x << " " << y << "\n";
				if (renderContainerTable[grid[y][x].renderPointer]->isBackgroundVisible) {
					window->draw(renderContainerTable[grid[y][x].renderPointer]->background);
				}
				if (renderContainerTable[grid[y][x].renderPointer]->isFloorVisible) {
					window->draw(renderContainerTable[grid[y][x].renderPointer]->floor);
				}
				if (renderContainerTable[grid[y][x].renderPointer]->isWallVisible) {
					window->draw(renderContainerTable[grid[y][x].renderPointer]->wall);
				}
			}
		}

		for (ushort y = shadeBorder.lower.y; y < shadeBorder.upper.y; y++) {
			for (ushort x = shadeBorder.lower.x; x < shadeBorder.upper.x; x++) {
				//std::cout << x << " " << y << "\n";

				if (renderContainerTable[grid[y][x].renderPointer]->isFloorVisible) {
					shadeTexture->draw(renderContainerTable[grid[y][x].renderPointer]->floorShade);
				}
				if (renderContainerTable[grid[y][x].renderPointer]->isWallVisible) {
					shadeTexture->draw(renderContainerTable[grid[y][x].renderPointer]->wallShade);
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


mapContainer* currentMap;


struct renderContainerFixed : renderContainer {

	sf::Sprite background = sf::Sprite();
	sf::Sprite backgroundCorners = sf::Sprite();
	sf::Sprite floor = sf::Sprite();
	sf::Sprite floorCorners = sf::Sprite();
	sf::Sprite floorShade = sf::Sprite();
	sf::Sprite floorCornersShade = sf::Sprite();
	sf::Sprite wall = sf::Sprite();
	sf::Sprite wallCorners = sf::Sprite();
	sf::Sprite wallShade = sf::Sprite();
	sf::Sprite wallCornersShade = sf::Sprite();



	bool isBackgroundVisible = false;
	bool isFloorVisible = false;
	bool isWallVisible = false;


	void create(sf::Vector2i coordinates, blockVariantStruct* backgroundPointer,
		blockVariantStruct* floorPointer, blockVariantStruct* wallPointer) {

		isWallVisible = wallPointer->internalId;
		isFloorVisible = floorPointer->internalId;
		isBackgroundVisible = backgroundPointer->internalId;

		// center->top->right->down->left
		// up left -> up right -> down left -> down right

		// here comes a huuuuuge but neatly organized and collapasble monster of a function, feck off critics, 
		// it's my code and I'm not motivated enough nor do I give a feck about it, it works
		bool backgroundCenterBools[5] = { true, false, false, false, false };
		bool backgroundCornerBools[4] = { false, false, false, false };
		bool floorCenterBools[5] = { true, false, false, false, false };
		bool floorCornerBools[4] = { false, false, false, false };
		bool wallCenterBools[5] = { true, false, false, false, false };
		bool wallCornerBools[4] = { false, false, false, false };

		

		// finally works
		if (coordinates.x > 0) {
			if (coordinates.x < currentMap->dimensions[currentDimension].size.x - 1) {
				if (coordinates.y > 0) {
					if (coordinates.y < currentMap->dimensions[currentDimension].size.y - 1) {
						// top
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].background->internalId == backgroundPointer->internalId) {
							backgroundCenterBools[1] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->internalId == floorPointer->internalId) {
							floorCenterBools[1] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].wall->internalId == wallPointer->internalId) {
							wallCenterBools[1] = true;
						}

						// right
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
							backgroundCenterBools[2] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
							floorCenterBools[2] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
							wallCenterBools[2] = true;
						}

						// down
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].background->internalId == backgroundPointer->internalId) {
							backgroundCenterBools[3] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->internalId == floorPointer->internalId) {
							floorCenterBools[3] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].wall->internalId == wallPointer->internalId) {
							wallCenterBools[3] = true;
						}

						// left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
							backgroundCenterBools[4] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCenterBools[4] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
							wallCenterBools[4] = true;
						}

						// up left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
							backgroundCornerBools[0] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[0] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
							wallCornerBools[0] = true;
						}

						// up right
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
							backgroundCornerBools[1] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[1] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
							wallCornerBools[1] = true;
						}

						// down left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
							backgroundCornerBools[2] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[2] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
							wallCornerBools[2] = true;
						}

						// down right
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
							backgroundCornerBools[3] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[3] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
							wallCornerBools[3] = true;
						}


						if (wallPointer->doesObstruct and
							currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].wall->doesObstruct and
							currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].wall->doesObstruct and
							currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].wall->doesObstruct and
							currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].wall->doesObstruct) {

							isBackgroundVisible = false;
							isFloorVisible = false;
						}
						else if (floorPointer->doesObstruct and
							currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->doesObstruct and
							currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->doesObstruct and
							currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->doesObstruct and
							currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->doesObstruct) {

							isBackgroundVisible = false;
						}
						

					}
					else {
						// at lower border

						// top
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].background->internalId == backgroundPointer->internalId) {
							backgroundCenterBools[1] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->internalId == floorPointer->internalId) {
							floorCenterBools[1] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].wall->internalId == wallPointer->internalId) {
							wallCenterBools[1] = true;
						}

						// right
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
							backgroundCenterBools[2] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
							floorCenterBools[2] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
							wallCenterBools[2] = true;
						}

						// down
						backgroundCenterBools[3] = true;
						floorCenterBools[3] = true;
						wallCenterBools[3] = true;

						// left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
							backgroundCenterBools[4] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCenterBools[4] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
							wallCenterBools[4] = true;
						}

						// up left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
							backgroundCornerBools[0] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[0] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
							wallCornerBools[0] = true;
						}

						// up right
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
							backgroundCornerBools[1] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[1] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
							wallCornerBools[1] = true;
						}

						// down left
						backgroundCornerBools[2] = true;
						floorCornerBools[2] = true;
						wallCornerBools[2] = true;

						// down right
						backgroundCornerBools[3] = true;
						floorCornerBools[3] = true;
						wallCornerBools[3] = true;
					}
				}
				else {
					// at upper border

					// top
					backgroundCenterBools[1] = true;
					floorCenterBools[1] = true;
					wallCenterBools[1] = true;

					// right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
						backgroundCenterBools[2] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCenterBools[2] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
						wallCenterBools[2] = true;
					}

					// down
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].background->internalId == backgroundPointer->internalId) {
						backgroundCenterBools[3] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->internalId == floorPointer->internalId) {
						floorCenterBools[3] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].wall->internalId == wallPointer->internalId) {
						wallCenterBools[3] = true;
					}

					// left
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
						backgroundCenterBools[4] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
						floorCenterBools[4] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
						wallCenterBools[4] = true;
					}

					// up left
					backgroundCornerBools[0] = true;
					floorCornerBools[0] = true;
					wallCornerBools[0] = true;

					// up right
					backgroundCornerBools[1] = true;
					floorCornerBools[1] = true;
					wallCornerBools[1] = true;

					// down left
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
						backgroundCornerBools[2] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
						floorCornerBools[2] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
						wallCornerBools[2] = true;
					}

					// down right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
						backgroundCornerBools[3] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCornerBools[3] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
						wallCornerBools[3] = true;
					}

				}
			}
			else {
				if (coordinates.y > 0) {
					if (coordinates.y < currentMap->dimensions[currentDimension].size.y - 1) {
						// on right wall

						// top
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].background->internalId == backgroundPointer->internalId) {
							backgroundCenterBools[1] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->internalId == floorPointer->internalId) {
							floorCenterBools[1] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].wall->internalId == wallPointer->internalId) {
							wallCenterBools[1] = true;
						}

						// right
						backgroundCenterBools[2] = true;
						floorCenterBools[2] = true;
						wallCenterBools[2] = true;

						// down
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].background->internalId == backgroundPointer->internalId) {
							backgroundCenterBools[3] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->internalId == floorPointer->internalId) {
							floorCenterBools[3] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].wall->internalId == wallPointer->internalId) {
							wallCenterBools[3] = true;
						}

						// left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
							backgroundCenterBools[4] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCenterBools[4] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
							wallCenterBools[4] = true;
						}

						// up left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
							backgroundCornerBools[0] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[0] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
							wallCornerBools[0] = true;
						}

						// up right
						backgroundCornerBools[1] = true;
						floorCornerBools[1] = true;
						wallCornerBools[1] = true;

						// down left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
							backgroundCornerBools[2] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[2] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
							wallCornerBools[2] = true;
						}

						// down right
						backgroundCornerBools[3] = true;
						floorCornerBools[3] = true;
						wallCornerBools[3] = true;

					}
					else {
						// on lower right corner

						// top
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].background->internalId == backgroundPointer->internalId) {
							backgroundCenterBools[1] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->internalId == floorPointer->internalId) {
							floorCenterBools[1] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].wall->internalId == wallPointer->internalId) {
							wallCenterBools[1] = true;
						}

						// right
						backgroundCenterBools[2] = true;
						floorCenterBools[2] = true;
						wallCenterBools[2] = true;

						// down
						backgroundCenterBools[3] = true;
						floorCenterBools[3] = true;
						wallCenterBools[3] = true;

						// left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
							backgroundCenterBools[4] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCenterBools[4] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
							wallCenterBools[4] = true;
						}

						// up left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
							backgroundCornerBools[0] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[0] = true;
						}
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
							wallCornerBools[0] = true;
						}

						// up right
						backgroundCornerBools[1] = true;
						floorCornerBools[1] = true;
						wallCornerBools[1] = true;

						// down left
						backgroundCornerBools[2] = true;
						floorCornerBools[2] = true;
						wallCornerBools[2] = true;

						// down right
						backgroundCornerBools[3] = true;
						floorCornerBools[3] = true;
						wallCornerBools[3] = true;
					}
				}
				else {
					// on upper right corner

					// top
					backgroundCenterBools[1] = true;
					floorCenterBools[1] = true;
					wallCenterBools[1] = true;

					// right
					backgroundCenterBools[2] = true;
					floorCenterBools[2] = true;
					wallCenterBools[2] = true;

					// down
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].background->internalId == backgroundPointer->internalId) {
						backgroundCenterBools[3] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->internalId == floorPointer->internalId) {
						floorCenterBools[3] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].wall->internalId == wallPointer->internalId) {
						wallCenterBools[3] = true;
					}

					// left
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
						backgroundCenterBools[4] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
						floorCenterBools[4] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
						wallCenterBools[4] = true;
					}

					// up left
					backgroundCornerBools[0] = true;
					floorCornerBools[0] = true;
					wallCornerBools[0] = true;

					// up right
					backgroundCornerBools[1] = true;
					floorCornerBools[1] = true;
					wallCornerBools[1] = true;

					// down left
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].background->internalId == backgroundPointer->internalId) {
						backgroundCornerBools[2] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
						floorCornerBools[2] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].wall->internalId == wallPointer->internalId) {
						wallCornerBools[2] = true;
					}

					// down right
					backgroundCornerBools[3] = true;
					floorCornerBools[3] = true;
					wallCornerBools[3] = true;
				}
			}
		}
		else {
			if (coordinates.y > 0) {
				if (coordinates.y < currentMap->dimensions[currentDimension].size.y - 1) {
					// on left wall

					// top
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].background->internalId == backgroundPointer->internalId) {
						backgroundCenterBools[1] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->internalId == floorPointer->internalId) {
						floorCenterBools[1] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].wall->internalId == wallPointer->internalId) {
						wallCenterBools[1] = true;
					}

					// right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
						backgroundCenterBools[2] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCenterBools[2] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
						wallCenterBools[2] = true;
					}

					// down
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].background->internalId == backgroundPointer->internalId) {
						backgroundCenterBools[3] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->internalId == floorPointer->internalId) {
						floorCenterBools[3] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].wall->internalId == wallPointer->internalId) {
						wallCenterBools[3] = true;
					}

					// left
					backgroundCenterBools[4] = true;
					floorCenterBools[4] = true;
					wallCenterBools[4] = true;

					// up left
					backgroundCornerBools[0] = true;
					floorCornerBools[0] = true;
					wallCornerBools[0] = true;

					// up right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
						backgroundCornerBools[1] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCornerBools[1] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
						wallCornerBools[1] = true;
					}

					// down left
					backgroundCornerBools[2] = true;
					floorCornerBools[2] = true;
					wallCornerBools[2] = true;

					// down right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
						backgroundCornerBools[3] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCornerBools[3] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
						wallCornerBools[3] = true;
					}

				}
				else {
					// on lower left corner

					// top
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].background->internalId == backgroundPointer->internalId) {
						backgroundCenterBools[1] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->internalId == floorPointer->internalId) {
						floorCenterBools[1] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].wall->internalId == wallPointer->internalId) {
						wallCenterBools[1] = true;
					}

					// right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
						backgroundCenterBools[2] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCenterBools[2] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
						wallCenterBools[2] = true;
					}

					// down
					backgroundCenterBools[3] = true;
					floorCenterBools[3] = true;
					wallCenterBools[3] = true;

					// left
					backgroundCenterBools[4] = true;
					floorCenterBools[4] = true;
					wallCenterBools[4] = true;

					// up left
					backgroundCornerBools[0] = true;
					floorCornerBools[0] = true;
					wallCornerBools[0] = true;

					// up right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
						backgroundCornerBools[1] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCornerBools[1] = true;
					}
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
						wallCornerBools[1] = true;
					}

					// down left
					backgroundCornerBools[2] = true;
					floorCornerBools[2] = true;
					wallCornerBools[2] = true;

					// down right
					backgroundCornerBools[3] = true;
					floorCornerBools[3] = true;
					wallCornerBools[3] = true;

				}
			}
			else {
				// on upper left corner

				// top
				backgroundCenterBools[1] = true;
				floorCenterBools[1] = true;
				wallCenterBools[1] = true;

				// right
				if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
					backgroundCenterBools[2] = true;
				}
				if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
					floorCenterBools[2] = true;
				}
				if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
					wallCenterBools[2] = true;
				}

				// down
				if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].background->internalId == backgroundPointer->internalId) {
					backgroundCenterBools[3] = true;
				}
				if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->internalId == floorPointer->internalId) {
					floorCenterBools[3] = true;
				}
				if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].wall->internalId == wallPointer->internalId) {
					wallCenterBools[3] = true;
				}

				// left
				backgroundCenterBools[4] = true;
				floorCenterBools[4] = true;
				wallCenterBools[4] = true;

				// up left
				backgroundCornerBools[0] = true;
				floorCornerBools[0] = true;
				wallCornerBools[0] = true;

				// up right
				backgroundCornerBools[1] = true;
				floorCornerBools[1] = true;
				wallCornerBools[1] = true;

				// down left
				backgroundCornerBools[2] = true;
				floorCornerBools[2] = true;
				wallCornerBools[2] = true;

				// down right
				if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].background->internalId == backgroundPointer->internalId) {
					backgroundCornerBools[3] = true;
				}
				if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
					floorCornerBools[3] = true;
				}
				if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].wall->internalId == wallPointer->internalId) {
					wallCornerBools[3] = true;
				}
			}
		}


		//std::cout << coordinates.x << " " << coordinates.y << " " << backgroundPointer->isVisible << "\n";

		if (backgroundPointer->isVisible) {
			background.setTexture(backgroundPointer->center.variantTable[backgroundCenterBools[0]][backgroundCenterBools[1]][backgroundCenterBools[2]][backgroundCenterBools[3]][backgroundCenterBools[4]]);
			background.setPosition(sf::Vector2f(blockBaseSize* coordinates.x, blockBaseSize* coordinates.y));
			background.setScale(backgroundPointer->scaleToSet);
			backgroundCorners.setTexture(backgroundPointer->corners.variantTable[backgroundCornerBools[0]][backgroundCornerBools[1]][backgroundCornerBools[2]][backgroundCornerBools[3]]);
			backgroundCorners.setPosition(sf::Vector2f(blockBaseSize* coordinates.x, blockBaseSize* coordinates.y));
			backgroundCorners.setScale(backgroundPointer->scaleToSet);
		}

	
		if (floorPointer->isVisible) {
			floor.setTexture(floorPointer->center.variantTable[floorCenterBools[0]][floorCenterBools[1]][floorCenterBools[2]][floorCenterBools[3]][floorCenterBools[4]]);
			floor.setPosition(sf::Vector2f(blockBaseSize * coordinates.x, blockBaseSize * coordinates.y));
			floor.setScale(floorPointer->scaleToSet);
			floorCorners.setTexture(floorPointer->corners.variantTable[floorCornerBools[0]][floorCornerBools[1]][floorCornerBools[2]][floorCornerBools[3]]);
			floorCorners.setPosition(sf::Vector2f(blockBaseSize* coordinates.x, blockBaseSize* coordinates.y));
			floorCorners.setScale(floorPointer->scaleToSet);
			

			floorShade.setTexture(floorPointer->center.variantShadeTable[floorCenterBools[0]][floorCenterBools[1]][floorCenterBools[2]][floorCenterBools[3]][floorCenterBools[4]]);
			floorShade.setPosition(sf::Vector2f(blockBaseSize * coordinates.x, blockBaseSize * coordinates.y));
			floorShade.setScale(floorPointer->shadeScaleToSet);
			floorCornersShade.setTexture(floorPointer->corners.variantShadeTable[floorCornerBools[0]][floorCornerBools[1]][floorCornerBools[2]][floorCornerBools[3]]);
			floorCornersShade.setPosition(sf::Vector2f(blockBaseSize* coordinates.x, blockBaseSize* coordinates.y));
			floorCornersShade.setScale(floorPointer->shadeScaleToSet);
		}
		

		if (wallPointer->isVisible) {
			wall.setTexture(wallPointer->center.variantTable[wallCenterBools[0]][wallCenterBools[1]][wallCenterBools[2]][wallCenterBools[3]][wallCenterBools[4]]);
			wall.setPosition(sf::Vector2f(blockBaseSize* coordinates.x, blockBaseSize* coordinates.y));
			wall.setScale(wallPointer->scaleToSet);
			wallCorners.setTexture(wallPointer->corners.variantTable[wallCornerBools[0]][wallCornerBools[1]][wallCornerBools[2]][wallCornerBools[3]]);
			wallCorners.setPosition(sf::Vector2f(blockBaseSize* coordinates.x, blockBaseSize* coordinates.y));
			wallCorners.setScale(wallPointer->scaleToSet);


			wallShade.setTexture(wallPointer->center.variantShadeTable[wallCenterBools[0]][wallCenterBools[1]][wallCenterBools[2]][wallCenterBools[3]][wallCenterBools[4]]);
			wallShade.setPosition(sf::Vector2f(blockBaseSize* coordinates.x, blockBaseSize* coordinates.y));
			wallShade.setScale(wallPointer->shadeScaleToSet);
			wallCornersShade.setTexture(wallPointer->corners.variantShadeTable[wallCornerBools[0]][wallCornerBools[1]][wallCornerBools[2]][wallCornerBools[3]]);
			wallCornersShade.setPosition(sf::Vector2f(blockBaseSize* coordinates.x, blockBaseSize* coordinates.y));
			wallCornersShade.setScale(wallPointer->shadeScaleToSet);
		}



		// and checking visibility here
		
		
		/*
		if (wallPointer->isVisible and wallPointer->doesObstruct) {
			isWallVisible = true;
			isFloorVisible = false;
			isBackgroundVisible = false;

		}
		else if (floorPointer->isVisible and floorPointer->doesObstruct) {

			if (!wallPointer->isVisible) {
				isWallVisible = false;
			}
			else {
				isWallVisible = true;
			}

			isFloorVisible = true;
			isBackgroundVisible = false;
		}
		else if (backgroundPointer->isVisible and backgroundPointer->doesObstruct) {

			if (!wallPointer->isVisible) {
				isWallVisible = false;
			}
			else {
				isWallVisible = true;
			}

			if (!floorPointer->isVisible) {
				isFloorVisible = false;
			}
			else {
				isFloorVisible = true;
			}

			isBackgroundVisible = true;
		}
		else {

			if (!wallPointer->isVisible) {
				isWallVisible = false;
			}
			else {
				isWallVisible = true;
			}

			if (!floorPointer->isVisible) {
				isFloorVisible = false;
			}
			else {
				isFloorVisible = true;
			}

			if (!backgroundPointer->isVisible) {
				isBackgroundVisible = false;
			}
			else {
				isBackgroundVisible = true;
			}
		}
		*/
		
		
		
		
		
		
		
		
	}
};

// cause internal one does not work now, boy what does a man do to have nicer graphics
void drawMap(sf::RenderTexture* window, renderLimit border) {

	for (unsigned short y = border.lower.y; y <= border.upper.y; y++) {
		for (unsigned short x = border.lower.x; x <= border.upper.x; x++) {
			//std::cout << x << " " << y << "\n";
			if (renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].isBackgroundVisible) {
				window->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].background);
				//window->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].backgroundCorners);
			}
			if (renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].isFloorVisible) {
				window->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].floor);
				//window->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].floorCorners);
			}
			if (renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].isWallVisible) {
				window->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].wall);
				//window->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].wallCorners);
			}
		}
	}

	

}

void drawShadeMap(sf::RenderTexture* shadeTexture, renderLimit shadeBorder) {
	for (unsigned short y = shadeBorder.lower.y; y <= shadeBorder.upper.y; y++) {
		for (unsigned short x = shadeBorder.lower.x; x <= shadeBorder.upper.x; x++) {
			//std::cout << x << " " << y << "\n";

			if (renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].isFloorVisible) {
				shadeTexture->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].floorShade);
				//shadeTexture->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].floorCornersShade);
			}
			if (renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].isWallVisible) {
				shadeTexture->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].wallShade);
				//shadeTexture->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].wallCornersShade);
			}
		}
	}
}