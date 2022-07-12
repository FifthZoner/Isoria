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


	vec2i size = vec2i(0, 0);
	str name;

	void create(vec2i dimSize, str nameToSet) {

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
	sf::Sprite floor = sf::Sprite();
	sf::Sprite floorCorners = sf::Sprite();
	sf::Sprite floorShade = sf::Sprite();
	sf::Sprite floorCornersShade = sf::Sprite();
	sf::Sprite wall = sf::Sprite();
	sf::Sprite wallShade = sf::Sprite();



	bool isBackgroundVisible = false;
	bool isFloorVisible = false;
	bool isWallVisible = false;


	void create(sf::Vector2i coordinates, blockVariantStruct* backgroundPointer,
		blockVariantStruct* floorPointer, blockVariantStruct* wallPointer) {


		isWallVisible = wallPointer->isVisible;
		isFloorVisible = floorPointer->isVisible;
		isBackgroundVisible = backgroundPointer->isVisible;

		// center->top->right->down->left
		// up left -> up right -> down left -> down right

		// here comes a monstrocity for now for floors only, sorry
		bool floorCenterBools[5] = { true, false, false, false, false };
		bool floorCornerBools[4] = { false, false, false, false };

		if (coordinates.x == 39) {
			std::cout << floorPointer->datapackId << " " << floorPointer->internalId << "\n";
		}


		// finally works
		if (coordinates.x > 0) {
			if (coordinates.x < currentMap->dimensions[currentDimension].size.x - 1) {
				if (coordinates.y > 0) {
					if (coordinates.y < currentMap->dimensions[currentDimension].size.y - 1) {
						// top
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->internalId == floorPointer->internalId) {
							floorCenterBools[1] = true;
						}

						// right
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
							floorCenterBools[2] = true;
						}

						// down
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->internalId == floorPointer->internalId) {
							floorCenterBools[3] = true;
						}

						// left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCenterBools[4] = true;
						}

						// up left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[0] = true;
						}

						// up right
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[1] = true;
						}

						// down left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[2] = true;
						}

						// down right
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[3] = true;
						}
					}
					else {
						// at lower border

						// top
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->internalId == floorPointer->internalId) {
							floorCenterBools[1] = true;
						}

						// right
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
							floorCenterBools[2] = true;
						}

						// down
						floorCenterBools[3] = true;

						// left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCenterBools[4] = true;
						}

						// up left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[0] = true;
						}

						// up right
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[1] = true;
						}

						// down left
						floorCornerBools[2] = true;

						// down right
						floorCornerBools[3] = true;
					}
				}
				else {
					// at upper border

					// top
					floorCenterBools[1] = true;

					// right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCenterBools[2] = true;
					}

					// down
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->internalId == floorPointer->internalId) {
						floorCenterBools[3] = true;
					}

					// left
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
						floorCenterBools[4] = true;
					}

					// up left
					floorCornerBools[0] = true;

					// up right
					floorCornerBools[1] = true;

					// down left
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
						floorCornerBools[2] = true;
					}

					// down right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCornerBools[3] = true;
					}

				}
			}
			else {
				if (coordinates.y > 0) {
					if (coordinates.y < currentMap->dimensions[currentDimension].size.y - 1) {
						// on right wall

						// top
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->internalId == floorPointer->internalId) {
							floorCenterBools[1] = true;
						}

						// right
						floorCenterBools[2] = true;

						// down
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->internalId == floorPointer->internalId) {
							floorCenterBools[3] = true;
						}

						// left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCenterBools[4] = true;
						}

						// up left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[0] = true;
						}

						// up right
						floorCornerBools[1] = true;

						// down left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[2] = true;
						}

						// down right
						floorCornerBools[3] = true;

					}
					else {
						// on lower right corner

						// top
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->internalId == floorPointer->internalId) {
							floorCenterBools[1] = true;
						}

						// right
						floorCenterBools[2] = true;

						// down
						floorCenterBools[3] = true;

						// left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCenterBools[4] = true;
						}

						// up left
						if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
							floorCornerBools[0] = true;
						}

						// up right
						floorCornerBools[1] = true;

						// down left
						floorCornerBools[2] = true;

						// down right
						floorCornerBools[3] = true;


					}
				}
				else {
					// on upper right corner

					// top
					floorCenterBools[1] = true;

					// right
					floorCenterBools[2] = true;

					// down
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->internalId == floorPointer->internalId) {
						floorCenterBools[3] = true;
					}

					// left
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
						floorCenterBools[4] = true;
					}

					// up left
					floorCornerBools[0] = true;

					// up right
					floorCornerBools[1] = true;

					// down left
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x - 1].floor->internalId == floorPointer->internalId) {
						floorCornerBools[2] = true;
					}

					// down right
					floorCornerBools[3] = true;

				}
			}
		}
		else {
			if (coordinates.y > 0) {
				if (coordinates.y < currentMap->dimensions[currentDimension].size.y - 1) {
					// on left wall

					// top
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->internalId == floorPointer->internalId) {
						floorCenterBools[1] = true;
					}

					// right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCenterBools[2] = true;
					}

					// down
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->internalId == floorPointer->internalId) {
						floorCenterBools[3] = true;
					}

					// left
					floorCenterBools[4] = true;

					// up left
					floorCornerBools[0] = true;

					// up right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCornerBools[1] = true;
					}

					// down left
					floorCornerBools[2] = true;

					// down right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCornerBools[3] = true;
					}

				}
				else {
					// on lower left corner

					// top
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x].floor->internalId == floorPointer->internalId) {
						floorCenterBools[1] = true;
					}

					// right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCenterBools[2] = true;
					}

					// down
					floorCenterBools[3] = true;

					// left
					floorCenterBools[4] = true;

					// up left
					floorCornerBools[0] = true;

					// up right
					if (currentMap->dimensions[currentDimension].grid[coordinates.y - 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
						floorCornerBools[1] = true;
					}

					// down left
					floorCornerBools[2] = true;

					// down right
					floorCornerBools[3] = true;

				}
			}
			else {
				// on upper left corner

				// top
				floorCenterBools[1] = true;

				// right
				if (currentMap->dimensions[currentDimension].grid[coordinates.y][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
					floorCenterBools[2] = true;
				}

				// down
				if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x].floor->internalId == floorPointer->internalId) {
					floorCenterBools[3] = true;
				}

				// left
				floorCenterBools[4] = true;

				// up left
				floorCornerBools[0] = true;

				// up right
				floorCornerBools[1] = true;

				// down left
				floorCornerBools[2] = true;

				// down right
				if (currentMap->dimensions[currentDimension].grid[coordinates.y + 1][coordinates.x + 1].floor->internalId == floorPointer->internalId) {
					floorCornerBools[3] = true;
				}
			}
		}


		//std::cout << coordinates.x << " " << coordinates.y << " " << backgroundPointer->isVisible << "\n";

		if (backgroundPointer->isVisible) {
			background.setTexture(backgroundPointer->texture);
			background.setPosition(sf::Vector2f(blockBaseSize * coordinates.x, blockBaseSize * coordinates.y));
			background.setScale(backgroundPointer->scaleToSet);
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
			wall.setTexture(wallPointer->texture);
			wall.setPosition(sf::Vector2f(blockBaseSize * coordinates.x, blockBaseSize * coordinates.y));
			wall.setScale(wallPointer->scaleToSet);

			wallShade.setTexture(wallPointer->shadeTexture);
			wallShade.setPosition(sf::Vector2f(blockBaseSize * coordinates.x, blockBaseSize * coordinates.y));
			wallShade.setScale(wallPointer->shadeScaleToSet);
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

void drawMap(sf::RenderTexture* window, renderLimit border,
	sf::RenderTexture* shadeTexture, renderLimit shadeBorder) {

	for (ushort y = border.lower.y; y <= border.upper.y; y++) {
		for (ushort x = border.lower.x; x <= border.upper.x; x++) {
			//std::cout << x << " " << y << "\n";
			if (renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].isBackgroundVisible) {
				window->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].background);
			}
			if (renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].isFloorVisible) {
				window->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].floor);
				window->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].floorCorners);
			}
			if (renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].isWallVisible) {
				window->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].wall);
			}
		}
	}

	for (ushort y = shadeBorder.lower.y; y <= shadeBorder.upper.y; y++) {
		for (ushort x = shadeBorder.lower.x; x <= shadeBorder.upper.x; x++) {
			//std::cout << x << " " << y << "\n";

			if (renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].isFloorVisible) {
				shadeTexture->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].floorShade);
				shadeTexture->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].floorCornersShade);
			}
			if (renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].isWallVisible) {
				shadeTexture->draw(renderContainerVirtualTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].wallShade);
			}
		}
	}

}