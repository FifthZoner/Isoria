#pragma once
#include "declarations.h"
#include "map.h"
#include <math.h>

// hybrid renderer updating service is supposed to be run in a seperate thread! 

std::mutex fixLock;

// imported to be used here too
// do not input value or do it, it makes no difference and just makes code cleaner

class chunk {
private:
	std::vector<std::vector<renderContainerFixed>> virtualTable;
	std::vector<std::vector<renderContainer*>> table;
	sf::RenderTexture texture;
	sf::RenderTexture shadeTexture;
	sf::Sprite sprite;
	sf::Sprite shadeSprite;
	sf::View view;
	bool isPrepared = false;
	
public:
	// this should create a chunk with correct values
	void create(){
		// resizes tables
		texture.create(blockBaseSize * chunkSize, blockBaseSize * chunkSize);
		shadeTexture.create(blockBaseSize * chunkSize, blockBaseSize * chunkSize);
		virtualTable.resize(chunkSize);
		table.resize(chunkSize);

		// assigns necessary pointers for virtual functions
		for (unsigned short y = 0; y < chunkSize; y++) {
			table[y].resize(chunkSize);
			virtualTable[y].resize(chunkSize);
			for (unsigned short x = 0; x < chunkSize; x++) {
				table[y][x] = &virtualTable[y][x];
			}
		}
	}

	// this function updates the chunk with given blocks
	void update(sf::Vector2i upperLeft, dimension* dimensionPtr, unsigned short number) {

		texture.clear(sf::Color::Green);
		shadeTexture.clear(sf::Color::Black);
		view.setSize(sf::Vector2f(chunkSize * blockBaseSize, chunkSize * blockBaseSize));
		// position
		view.setCenter(sf::Vector2f(float(blockBaseSize) * float(upperLeft.x + float(chunkSize) / 2), float(blockBaseSize) * float(upperLeft.y + float(chunkSize) / 2)));
		texture.setView(view);
		shadeTexture.setView(view);

		// creating and drawing
		for (unsigned short y = 0; y < chunkSize; y++) {
			for (unsigned short x = 0; x < chunkSize; x++) {
				virtualTable[y][x].create(sf::Vector2i(upperLeft.x + x, upperLeft.y + y), dimensionPtr->grid[upperLeft.y + y][upperLeft.x + x].background, dimensionPtr->grid[upperLeft.y + y][upperLeft.x + x].floor, dimensionPtr->grid[upperLeft.y + y][upperLeft.x + x].wall);
				currentMap->dimensions[currentDimension].grid[y + upperLeft.y][x + upperLeft.x].chunkNumber = number;

				if (virtualTable[y][x].isBackgroundVisible) {
					texture.draw(virtualTable[y][x].background);
				}
				if (virtualTable[y][x].isFloorVisible) {
					texture.draw(virtualTable[y][x].floor);
					shadeTexture.draw(virtualTable[y][x].floorShade);
				}
				if (virtualTable[y][x].isWallVisible) {
					texture.draw(virtualTable[y][x].wall);
					shadeTexture.draw(virtualTable[y][x].wallShade);
				}
				
			}
		}
		
		
		// displaying
		texture.display();
		shadeTexture.display();

		// preparing sprites
		sprite.setTexture(texture.getTexture());
		sprite.setPosition(upperLeft.x * blockBaseSize, upperLeft.y * blockBaseSize);
		shadeSprite.setTexture(shadeTexture.getTexture());
		shadeSprite.setPosition(upperLeft.x * blockBaseSize, upperLeft.y * blockBaseSize);

		isPrepared = true;
		// now this chunk SHOULD be ready to be displayed to map renderTextures
	}

	// for preparing to be reassigned
	void clear() {
		isPrepared = false;
	}

	// draws the sprite to given renderTexture
	void draw(sf::RenderTexture* targetTexture) {
		if (isPrepared) {
			targetTexture->draw(sprite);
		}
	}

	// draws the shade sprite to given renderTexture
	void drawShade(sf::RenderTexture* targetTexture) {
		if (isPrepared) {
			targetTexture->draw(shadeSprite);
		}
	}
};

std::vector<chunk> chunks(chunkAmount);

// calculates required chunk size for the fixed amount of chunks with sime redundanties
void getChunkSize() {
	sf::Vector2i value = sf::Vector2i(5, 5);
	
	value.x = ceil(float(gameRes.x) / float(chunkScreenRatio.x) / float(blockBaseSize));
	value.y = ceil(float(gameRes.y) / float(chunkScreenRatio.y) / float(blockBaseSize));

	if (value.x >= value.y) {
		chunkSize = value.x;
	}
	else {
		chunkSize = value.y;
	}

	debugMsg(std::string("HR Debug: Chunk size calculated at: " + std::to_string(chunkSize)));
}

// creates viewing distance and render tables, they reduce amount of sprites
void createRenderTables() {

	// calculating chunk size
	getChunkSize();

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

// calculates chunk rendering borders
void getChunkBorders() {

	renderLimit temp;

	threadLock.lock();
	sf::Vector2i coord = getViewCoodrinates();

	// initial coords
	temp.lower.x = coord.x - chunkRenderDistanceConverted.x;
	temp.lower.y = coord.y - chunkRenderDistanceConverted.y;
	temp.upper.x = coord.x + chunkRenderDistanceConverted.x;
	temp.upper.y = coord.y + chunkRenderDistanceConverted.y;

	// border check
	if (temp.lower.x < 0) {
		temp.lower.x = 0;
	}

	if (temp.lower.y < 0) {
		temp.lower.y = 0;
	}

	if (temp.upper.x >= currentMap->dimensions[currentDimension].size.x) {
		temp.upper.x = currentMap->dimensions[currentDimension].size.x - 1;
	}

	if (temp.upper.y >= currentMap->dimensions[currentDimension].size.y) {
		temp.upper.y = currentMap->dimensions[currentDimension].size.y - 1;
	}

	// convert to chunk left upper corners
	temp.lower.x = temp.lower.x - (temp.lower.x % chunkSize);
	temp.lower.y = temp.lower.y - (temp.lower.y % chunkSize);
	temp.upper.x = temp.upper.x - (temp.upper.x % chunkSize);
	temp.upper.y = temp.upper.y - (temp.upper.y % chunkSize);

	chunkBorder = temp;

	threadLock.unlock();

	//std::cout << "Current chunk borders: " << chunkBorder.lower.x << " " << chunkBorder.lower.y << " " << chunkBorder.upper.x << " " << chunkBorder.upper.y << "\n";
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
	
	getChunkBorders();
	chunkCurrent = chunkBorder;

	// ah yes, much nicer than with bare renderContainers
	for (unsigned short y = chunkBorder.lower.y; y <= chunkBorder.upper.y; y += chunkSize) {
		for (unsigned short x = chunkBorder.lower.x; x <= chunkBorder.upper.x; x += chunkSize) {
			chunks[chunkQueue.front()].update(sf::Vector2i(x, y), &currentMap->dimensions[currentDimension], chunkQueue.front());
			chunkQueue.pop();
		}
	}

	// to be retired
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

//		MOVING FUNCTIONS
// will only give pointers back to queue as that might actually prevent crashes more than causing them

// vertical clear
void chunkClearVertical(unsigned short x, unsigned short lowerY, unsigned short upperY) {
	//std::cout << "V Clear: " << x << " " << lowerY << " " << upperY << "\n";
	for (unsigned short y = lowerY; y <= upperY; y+= chunkSize) {
		chunkQueue.push(currentMap->dimensions[currentDimension].grid[y][x].chunkNumber);
		chunks[currentMap->dimensions[currentDimension].grid[y][x].chunkNumber].clear();
		//cleaned++;
	}
}

// horizontal clear
void chunkClearHorizontal(unsigned short y, unsigned short lowerX, unsigned short upperX) {
	//std::cout << "H Clear: " << y << " " << lowerX << " " << upperX << "\n";
	for (unsigned short x = lowerX; x <= upperX; x+= chunkSize) {
		chunkQueue.push(currentMap->dimensions[currentDimension].grid[y][x].chunkNumber);
		chunks[currentMap->dimensions[currentDimension].grid[y][x].chunkNumber].clear();

		//cleaned++;
	}
}

// vertical assign
void chunkAssignVertical(unsigned short x, unsigned short lowerY, unsigned short upperY) {
	//std::cout << "V Assign: " << x << " " << lowerY << " " << upperY << "\n";
	for (unsigned short y = lowerY; y <= upperY; y+= chunkSize) {
		chunks[chunkQueue.front()].update(sf::Vector2i(x, y), &currentMap->dimensions[currentDimension], chunkQueue.front());
		chunkQueue.pop();
	}
}

// horizontal assign
void chunkAssignHorizontal(unsigned short y, unsigned short lowerX, unsigned short upperX) {
	//std::cout << "H Assign: " << y << " " << lowerX << " " << upperX << "\n";
	for (unsigned short x = lowerX; x <= upperX; x+= chunkSize) {
		chunks[chunkQueue.front()].update(sf::Vector2i(x, y), &currentMap->dimensions[currentDimension], chunkQueue.front());
		chunkQueue.pop();
	}
}

// performs area move of chunks
void moveChunks() {

	std::cout << "Currently used chunks: " << chunks.size() - chunkQueue.size() << " out of: " << chunks.size() << "\n";

	// works
	if (chunkBorder.upper.x > chunkCurrent.upper.x) {
		while (chunkBorder.upper.x > chunkCurrent.upper.x) {
			chunkAssignVertical(chunkCurrent.upper.x + chunkSize, chunkCurrent.lower.y, chunkCurrent.upper.y);
			chunkCurrent.upper.x+= chunkSize;
		}
	}
	else if (chunkBorder.upper.x < chunkCurrent.upper.x) {
		while (chunkBorder.upper.x < chunkCurrent.upper.x) {
			chunkClearVertical(chunkCurrent.upper.x, chunkCurrent.lower.y, chunkCurrent.upper.y);
			chunkCurrent.upper.x-= chunkSize;
		}
	}

	if (chunkBorder.lower.x > chunkCurrent.lower.x) {
		while (chunkBorder.lower.x > chunkCurrent.lower.x) {
			chunkClearVertical(chunkCurrent.lower.x, chunkCurrent.lower.y, chunkCurrent.upper.y);
			chunkCurrent.lower.x+= chunkSize;
		}
	}
	else if (chunkBorder.lower.x < chunkCurrent.lower.x) {
		while (chunkBorder.lower.x < chunkCurrent.lower.x) {
			chunkAssignVertical(chunkCurrent.lower.x - chunkSize, chunkCurrent.lower.y, chunkCurrent.upper.y);
			chunkCurrent.lower.x-= chunkSize;
		}
	}
	// works
	if (chunkBorder.upper.y > chunkCurrent.upper.y) {
		while (chunkBorder.upper.y > chunkCurrent.upper.y) {
			chunkAssignHorizontal(chunkCurrent.upper.y + chunkSize, chunkCurrent.lower.x, chunkCurrent.upper.x);
			chunkCurrent.upper.y+= chunkSize;
		}
	}
	else if (chunkBorder.upper.y < chunkCurrent.upper.y) {
		while (chunkBorder.upper.y < chunkCurrent.upper.y) {
			chunkClearHorizontal(chunkCurrent.upper.y, chunkCurrent.lower.x, chunkCurrent.upper.x);
			chunkCurrent.upper.y-= chunkSize;
		}
	}

	if (chunkBorder.lower.y > chunkCurrent.lower.y) {
		while (chunkBorder.lower.y > chunkCurrent.lower.y) {
			chunkClearHorizontal(chunkCurrent.lower.y, chunkCurrent.lower.x, chunkCurrent.upper.x);
			chunkCurrent.lower.y+= chunkSize;
		}
	}
	else if (chunkBorder.lower.y < chunkCurrent.lower.y) {
		while (chunkBorder.lower.y < chunkCurrent.lower.y) {
			chunkAssignHorizontal(chunkCurrent.lower.y - chunkSize, chunkCurrent.lower.x, chunkCurrent.upper.x);
			chunkCurrent.lower.y-= chunkSize;
		}
	}

	std::cout << "Currently used chunks: " << chunks.size() - chunkQueue.size() << " out of: " << chunks.size() << "\n";
}

// main hybrid renderer function to be started before starting the game
void hybridRenderingService() {

	debugMsg("HR Debug: Hybrid rendering service prepared! Waiting for map...\n");

	while (!canHybridRenderServiceStartWorking and letItBe) {
		sf::sleep(sf::microseconds(100));
	}

	if (letItBe) {
		debugMsg("HR Debug: Hybrid rendering service active! Preparing containers...\n");

		// prepares the map for playing
		initialHybridRenderingFill();

		debugMsg("HR Debug: Hybrid rendering service ready!\n");

		isHybridRenderServiceReady = true;

		// checks if borders moved
		while (letItBe) {

			getChunkBorders();

			if (chunkCurrent.lower.x != chunkBorder.lower.x or chunkCurrent.upper.x != chunkBorder.upper.x
				or chunkCurrent.lower.y != chunkBorder.lower.y or chunkCurrent.upper.y != chunkBorder.upper.y) {

				// moving things
				moveChunks();
			}

			// to be retired
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

	debugMsg("HR Debug: Ending hybrid rendering thread");
}

void drawChunks(sf::RenderTexture* renderTexture, sf::RenderTexture* shadeRenderTexture) {

	renderLimit tempLimit = chunkBorder;
	
	for (unsigned short y = tempLimit.lower.y; y <= tempLimit.upper.y; y += chunkSize) {
		for (unsigned short x = tempLimit.lower.x; x <= tempLimit.upper.x; x += chunkSize) {

			// fix to a bug that plagued me for something like a week
			//if (x < currentMap->dimensions[currentDimension].size.x and y < currentMap->dimensions[currentDimension].size.y) {
				chunks[currentMap->dimensions[currentDimension].grid[y][x].chunkNumber].draw(renderTexture);
				chunks[currentMap->dimensions[currentDimension].grid[y][x].chunkNumber].drawShade(shadeRenderTexture);
			//}
		}
	}
}