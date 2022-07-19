#pragma once
#include "declarations.h"
#include "map.h"

// hybrid renderer updating service is supposed to be run in a seperate thread! 

// imported to be used here too
// do not input value or do it, it makes no difference and just makes code cleaner

class chunk {
private:
	std::vector <renderContainerFixed> virtualTable;
	std::vector<std::vector<renderContainer*>> table;
	sf::RenderTexture texture;
	sf::RenderTexture shadeTexture;
	sf::Sprite sprite;
	sf::Sprite shadeSprite;
	sf::View view;
	
public:
	

	// this should create a chunk with correct values
	void create(){
		// resizes tables
		texture.create(blockBaseSize * chunkSize, blockBaseSize * chunkSize);
		shadeTexture.create(blockBaseSize * chunkSize, blockBaseSize * chunkSize);
		virtualTable.resize(chunkSize * chunkSize);
		table.resize(chunkSize);

		// assigns necessary pointers for virtual functions
		unsigned short n = 0;
		for (unsigned short y = 0; y < chunkSize; y++) {
			table[y].resize(chunkSize);
			for (unsigned short x = 0; n < chunkSize; x++) {
				table[y][x] = &virtualTable[n];
				n++;
			}
		}
	}

	// this function updates the chunk with given blocks
	void update(sf::Vector2i upperLeft, dimension* dimensionPtr) {

		texture.clear(sf::Color::Black);
		shadeTexture.clear(sf::Color::Black);

		// creating and drawing
		for (unsigned short y = 0; y < chunkSize; y++) {
			for (unsigned short x = 0; x < chunkSize; x++) {
				table[y][x]->create(sf::Vector2i(upperLeft.x + x, upperLeft.y + y), dimensionPtr->grid[upperLeft.y + y][upperLeft.x + x].background, dimensionPtr->grid[upperLeft.y + y][upperLeft.x + x].floor, dimensionPtr->grid[upperLeft.y + y][upperLeft.x + x].wall);
				
				if (table[y][x]->isBackgroundVisible) {
					texture.draw(table[y][x]->background);
					shadeTexture.draw(table[y][x]->background);
				}
				if (table[y][x]->isFloorVisible) {
					texture.draw(table[y][x]->floor);
					shadeTexture.draw(table[y][x]->floorShade);
				}
				if (table[y][x]->isWallVisible) {
					texture.draw(table[y][x]->wall);
					shadeTexture.draw(table[y][x]->wallShade);
				}
				
			}
		}
		// position
		view.setCenter(sf::Vector2f(blockBaseSize * float(upperLeft.x + float(chunkSize) / 2), blockBaseSize * float(upperLeft.y + float(chunkSize) / 2)));
		texture.setView(view);
		shadeTexture.setView(view);
		
		// displaying
		texture.display();
		shadeTexture.display();

		// preparing sprites
		sprite.setTexture(texture.getTexture());
		sprite.setPosition(upperLeft.x * blockBaseSize, upperLeft.y * blockBaseSize);
		shadeSprite.setTexture(shadeTexture.getTexture());
		shadeSprite.setPosition(upperLeft.x * blockBaseSize, upperLeft.y * blockBaseSize);

		// now this chunk SHOULD be ready to be displayed to map renderTextures
	}
};

std::vector<chunk> chunks(chunkAmount);

// creates viewing distance and render tables, they reduce amount of sprites
void createRenderTables() {

	// calculating chunk size
	

	// resizing the table
	renderContainerTable.resize(((shadeRenderDistance.x * 2) + (hybridRenderOffset * 2)) * ((shadeRenderDistance.y * 2) + (hybridRenderOffset * 2)) * hybridRenderTableMultiplier);
	renderContainerVirtualTable.resize(((shadeRenderDistance.x * 2) + (hybridRenderOffset * 2)) * ((shadeRenderDistance.y * 2) + (hybridRenderOffset * 2)) * hybridRenderTableMultiplier);

	// other
	//currentDimensionPointer = &currentMap->dimensions[currentDimension];

	// prepares chunk containers and queue for their assignment
	for (short n = 0; n < chunkAmount; n++) {
		chunks[n].create();
		chunkQueue.push(n);
	}


	// filling the queue
	for (unsigned short n = 0; n < renderContainerTable.size(); n++) {
		renderContainerTable[n] = &renderContainerVirtualTable[n];
		
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

// gets borders for chunks
void getChunkBorders() {

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
			renderContainerTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer]->create(sf::Vector2i(x, y), currentMap->dimensions[currentDimension].grid[y][x].background,
				currentMap->dimensions[currentDimension].grid[y][x].floor, currentMap->dimensions[currentDimension].grid[y][x].wall);
		}
	}

	
}

//		MOVING FUNCTIONS
// will only give pointers back to queue as that might actually prevent crashes more than causing them

// vertical clear
void hybridClearVertical(unsigned short x, unsigned short lowerY, unsigned short upperY) {
	//std::cout << "V Clear: " << x << " " << lowerY << " " << upperY << "\n";
	for (unsigned short y = lowerY; y <= upperY; y++) {
		renderContainerQueue.push(currentMap->dimensions[currentDimension].grid[y][x].renderPointer);
		
		//cleaned++;
	}
}

// horizontal clear
void hybridClearHorizontal(unsigned short y, unsigned short lowerX, unsigned short upperX) {
	//std::cout << "H Clear: " << y << " " << lowerX << " " << upperX << "\n";
	for (unsigned short x = lowerX; x <= upperX; x++) {
		renderContainerQueue.push(currentMap->dimensions[currentDimension].grid[y][x].renderPointer);

		//cleaned++;
	}
}

// vertical assign
void hybridAssignVertical(unsigned short x, unsigned short lowerY, unsigned short upperY) {
	//std::cout << "V Assign: " << x << " " << lowerY << " " << upperY << "\n";
	for (unsigned short y = lowerY; y <= upperY; y++) {
		currentMap->dimensions[currentDimension].grid[y][x].renderPointer = renderContainerQueue.front();
		renderContainerQueue.pop();
		renderContainerTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer]->create(sf::Vector2i(x, y), currentMap->dimensions[currentDimension].grid[y][x].background,
			currentMap->dimensions[currentDimension].grid[y][x].floor, currentMap->dimensions[currentDimension].grid[y][x].wall);
	}
}

// horizontal assign
void hybridAssignHorizontal(unsigned short y, unsigned short lowerX, unsigned short upperX) {
	//std::cout << "H Assign: " << y << " " << lowerX << " " << upperX << "\n";
	for (unsigned short x = lowerX; x <= upperX; x++) {
		currentMap->dimensions[currentDimension].grid[y][x].renderPointer = renderContainerQueue.front();
		renderContainerQueue.pop();
		renderContainerTable[currentMap->dimensions[currentDimension].grid[y][x].renderPointer]->create(sf::Vector2i(x, y), currentMap->dimensions[currentDimension].grid[y][x].background,
			currentMap->dimensions[currentDimension].grid[y][x].floor, currentMap->dimensions[currentDimension].grid[y][x].wall);
	}
}

// performs area move of containers
void moveHybridRender() {

	//std::cout << "Current border: " << hybridRenderBorder.lower.x << " - " << hybridRenderBorder.upper.x << " " << hybridRenderBorder.lower.y << " - " << hybridRenderBorder.upper.y << "\n";


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



	
	
	//std::cout << "Current table size: " << renderContainerTable.size() << "\n";
	//std::cout << "Current cleaned and assigned: " << cleaned << " " << assigned << "\n";
	//std::cout << "Current area: " << hybridRenderCurrent.lower.x << " - " << hybridRenderCurrent.upper.x << " " << hybridRenderCurrent.lower.y << " - " << hybridRenderCurrent.upper.y << "\n";
	//std::cout << "Current buffer size: " << renderContainerQueue.size() << "\n";
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