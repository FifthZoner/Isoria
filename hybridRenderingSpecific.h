#pragma once
#include "declarations.h"

// hybrid renderer updating service is supposed to be run in a seperate thread! 

// imported to be used here too
// do not input value or do it, it makes no difference and just makes code cleaner

// debug values
int cleaned = 0;
int assigned = 0;

// creates viewing distance and render tables, they reduce amount of sprites from x * y so for example 1,000,000 to just 4,806 for full hd with offset 2
void createRenderTables() {

	// resizing the table
	renderContainerTable.resize(((shadeRenderDistance.x * 2) + (hybridRenderOffset * 2)) * ((shadeRenderDistance.y * 2) + (hybridRenderOffset * 2)) * hybridRenderTableMultiplier);

	// other
	//currentDimensionPointer = &currentMap->dimensions[currentDimension];

	// filling the queue
	for (unsigned short n = 0; n < renderContainerTable.size(); n++) {

		renderContainerQueue.push(n);
	}

	debugMsg(std::string("R2 Debug: Created render container table with: " + std::to_string(renderContainerTable.size()) + " elements"));
}

// calculates hybrid rendering desired borders to move to with given offset to avoid rendering unprepared blocks
void getHybridRenderingBorders() {
	// to ensure no shenanigans like reading this while some value is out of limits
	threadLock.lock();
	sf::Vector2i coord = getViewCoodrinates();

	

	hybridRenderBorder.lower.x = coord.x - shadeRenderDistance.x - hybridRenderOffset;
	hybridRenderBorder.lower.y = coord.y - shadeRenderDistance.y - hybridRenderOffset;
	hybridRenderBorder.upper.x = coord.x + shadeRenderDistance.x + hybridRenderOffset;
	hybridRenderBorder.upper.y = coord.y + shadeRenderDistance.y + hybridRenderOffset;

	// border check
	if (hybridRenderBorder.lower.x < 0) {
		hybridRenderBorder.lower.x = 0;
	}

	if (hybridRenderBorder.lower.y < 0) {
		hybridRenderBorder.lower.y = 0;
	}
	
	if (hybridRenderBorder.upper.x >= currentMap->dimensions[currentDimension].size.x) {
		hybridRenderBorder.upper.x = currentMap->dimensions[currentDimension].size.x - 1;
	}
	
	if (hybridRenderBorder.upper.y >= currentMap->dimensions[currentDimension].size.y) {
		hybridRenderBorder.upper.y = currentMap->dimensions[currentDimension].size.y - 1;
	}
	
	threadLock.unlock();
}

// assigns inital area with renderContainers only use this when no renderContainers are assigned
void initialHybridRenderingFill() {

/*
//debug
	for (unsigned short n = 0; n < renderContainerTable.size() * 5; n++) {
		currentMap->dimensions[currentDimension].grid[0][0].renderPointer = &*renderContainerQueue.front();
		renderContainerQueue.pop();
		currentMap->dimensions[currentDimension].grid[0][0].renderPointer->create(sf::Vector2i(0, 0), currentMap->dimensions[currentDimension].grid[0][0].background,
			currentMap->dimensions[currentDimension].grid[0][0].floor, currentMap->dimensions[currentDimension].grid[0][0].wall);

		renderContainerQueue.push(&*currentMap->dimensions[currentDimension].grid[0][0].renderPointer);
	}
*/
	



	getHybridRenderingBorders();
	hybridRenderCurrent = hybridRenderBorder;

	for (unsigned short y = hybridRenderBorder.lower.y; y <= hybridRenderBorder.upper.y; y++) {
		for (unsigned short x = hybridRenderBorder.lower.x; x <= hybridRenderBorder.upper.x; x++) {
			currentMap->dimensions[currentDimension].grid[y][x].renderPointer = renderContainerQueue.front();
			renderContainerQueue.pop();
			renderContainerTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].create(sf::Vector2i(x, y), currentMap->dimensions[currentDimension].grid[y][x].background,
				currentMap->dimensions[currentDimension].grid[y][x].floor, currentMap->dimensions[currentDimension].grid[y][x].wall);
			assigned++;
		}
	}

	
}

//		MOVING FUNCTIONS
// will only give pointers back to queue as that might actually prevent crashes more than causing them

// vertical clear
void hybridClearVertical(unsigned short x, unsigned short lowerY, unsigned short upperY) {
	std::cout << "V Clear: " << x << " " << lowerY << " " << upperY << "\n";
	for (unsigned short y = lowerY; y <= upperY; y++) {
		renderContainerQueue.push(currentMap->dimensions[currentDimension].grid[y][x].renderPointer);
		
		cleaned++;
	}
}

// horizontal clear
void hybridClearHorizontal(unsigned short y, unsigned short lowerX, unsigned short upperX) {
	std::cout << "H Clear: " << y << " " << lowerX << " " << upperX << "\n";
	for (unsigned short x = lowerX; x <= upperX; x++) {
		renderContainerQueue.push(currentMap->dimensions[currentDimension].grid[y][x].renderPointer);

		cleaned++;
	}
}

// vertical assign
void hybridAssignVertical(unsigned short x, unsigned short lowerY, unsigned short upperY) {
	std::cout << "V Assign: " << x << " " << lowerY << " " << upperY << "\n";
	for (unsigned short y = lowerY; y <= upperY; y++) {
		currentMap->dimensions[currentDimension].grid[y][x].renderPointer = renderContainerQueue.front();
		renderContainerQueue.pop();
		renderContainerTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].create(sf::Vector2i(x, y), currentMap->dimensions[currentDimension].grid[y][x].background,
			currentMap->dimensions[currentDimension].grid[y][x].floor, currentMap->dimensions[currentDimension].grid[y][x].wall);
		assigned++;
	}
}

// horizontal assign
void hybridAssignHorizontal(unsigned short y, unsigned short lowerX, unsigned short upperX) {
	std::cout << "H Assign: " << y << " " << lowerX << " " << upperX << "\n";
	for (unsigned short x = lowerX; x <= upperX; x++) {
		currentMap->dimensions[currentDimension].grid[y][x].renderPointer = renderContainerQueue.front();
		renderContainerQueue.pop();
		renderContainerTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer].create(sf::Vector2i(x, y), currentMap->dimensions[currentDimension].grid[y][x].background,
			currentMap->dimensions[currentDimension].grid[y][x].floor, currentMap->dimensions[currentDimension].grid[y][x].wall);
		assigned++;
	}
}

// performs area move of containers
void moveHybridRender() {

	std::cout << "Current border: " << hybridRenderBorder.lower.x << " - " << hybridRenderBorder.upper.x << " " << hybridRenderBorder.lower.y << " - " << hybridRenderBorder.upper.y << "\n";


	// works
	if (hybridRenderBorder.upper.x > hybridRenderCurrent.upper.x) {
		while (hybridRenderBorder.upper.x > hybridRenderCurrent.upper.x) {
			hybridAssignVertical(hybridRenderCurrent.upper.x + 1, hybridRenderCurrent.lower.y, hybridRenderCurrent.upper.y);
			hybridRenderCurrent.upper.x++;
		}
	}
	else if (hybridRenderBorder.upper.x < hybridRenderCurrent.upper.x) {
		while (hybridRenderBorder.upper.x < hybridRenderCurrent.upper.x) {
			hybridClearVertical(hybridRenderCurrent.upper.x, hybridRenderCurrent.lower.y, hybridRenderCurrent.upper.y);
			hybridRenderCurrent.upper.x--;
		}
	}

	if (hybridRenderBorder.lower.x > hybridRenderCurrent.lower.x) {
		while (hybridRenderBorder.lower.x > hybridRenderCurrent.lower.x) {
			hybridClearVertical(hybridRenderCurrent.lower.x, hybridRenderCurrent.lower.y, hybridRenderCurrent.upper.y);
			hybridRenderCurrent.lower.x++;
		}
	}
	else if (hybridRenderBorder.lower.x < hybridRenderCurrent.lower.x) {
		while (hybridRenderBorder.lower.x < hybridRenderCurrent.lower.x) {
			hybridAssignVertical(hybridRenderCurrent.lower.x - 1, hybridRenderCurrent.lower.y, hybridRenderCurrent.upper.y);
			hybridRenderCurrent.lower.x--;
		}
	}
	// works
	if (hybridRenderBorder.upper.y > hybridRenderCurrent.upper.y) {
		while (hybridRenderBorder.upper.y > hybridRenderCurrent.upper.y) {
			hybridAssignHorizontal(hybridRenderCurrent.upper.y + 1, hybridRenderCurrent.lower.x, hybridRenderCurrent.upper.x);
			hybridRenderCurrent.upper.y++;
		}
	}
	else if (hybridRenderBorder.upper.y < hybridRenderCurrent.upper.y) {
		while (hybridRenderBorder.upper.y < hybridRenderCurrent.upper.y) {
			hybridClearHorizontal(hybridRenderCurrent.upper.y, hybridRenderCurrent.lower.x, hybridRenderCurrent.upper.x);
			hybridRenderCurrent.upper.y--;
		}
	}

	if (hybridRenderBorder.lower.y > hybridRenderCurrent.lower.y) {
		while (hybridRenderBorder.lower.y > hybridRenderCurrent.lower.y) {
			hybridClearHorizontal(hybridRenderCurrent.lower.y, hybridRenderCurrent.lower.x, hybridRenderCurrent.upper.x);
			hybridRenderCurrent.lower.y++;
		}
	}
	else if (hybridRenderBorder.lower.y < hybridRenderCurrent.lower.y) {
		while (hybridRenderBorder.lower.y < hybridRenderCurrent.lower.y) {
			hybridAssignHorizontal(hybridRenderCurrent.lower.y - 1, hybridRenderCurrent.lower.x, hybridRenderCurrent.upper.x);
			hybridRenderCurrent.lower.y--;
		}
	}



	
	
	std::cout << "Current table size: " << renderContainerTable.size() << "\n";
	std::cout << "Current cleaned and assigned: " << cleaned << " " << assigned << "\n";
	std::cout << "Current area: " << hybridRenderCurrent.lower.x << " - " << hybridRenderCurrent.upper.x << " " << hybridRenderCurrent.lower.y << " - " << hybridRenderCurrent.upper.y << "\n";
	std::cout << "Current buffer size: " << renderContainerQueue.size() << "\n";
}

// main hybrid renderer function to be started before starting the game
void hybridRenderingService() {

	debugMsg("HR Debug: Hybrid rendering service prepared! Waiting for map...\n");

	while (!canHybridRenderServiceStartWorking) {
		sf::sleep(sf::microseconds(100));
	}

	debugMsg("HR Debug: Hybrid rendering service active! Preparing containers...\n");

	// prepares the map for playing
	initialHybridRenderingFill();

	debugMsg("HR Debug: Hybrid rendering service ready!\n");

	isHybridRenderServiceReady = true;

	// checks if borders moved
	while (letItBe) {

		
		getHybridRenderingBorders();
		

		if (hybridRenderCurrent.lower.x != hybridRenderBorder.lower.x or hybridRenderCurrent.upper.x != hybridRenderBorder.upper.x
			or hybridRenderCurrent.lower.y != hybridRenderBorder.lower.y or hybridRenderCurrent.upper.y != hybridRenderBorder.upper.y) {

			// moving things
			moveHybridRender();
		}
		else {
			sf::sleep(sf::microseconds(100));
		}
	}
}