#pragma once

#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include "map.h"
#include "transferMisc.h"
#include <unordered_map>
#include "loadMap.h"
#include "shared.h"
#include "declarations.h"


bool mrDebug;

std::vector<std::string> mrNames;

// conversion tables
std::vector<unsigned short> mrDatapackConvert;
std::vector<std::unordered_map<std::string, unsigned int>> mrBackgroundNames;
std::vector<std::unordered_map<std::string, unsigned int>> mrFloorNames;
std::vector<std::unordered_map<std::string, unsigned int>> mrWallNames;

std::vector<mrConvertStruct> mrBackgroundConvert;
std::vector<mrConvertStruct> mrFloorConvert;
std::vector<mrConvertStruct> mrWallConvert;

//				DATAPACK INFO

// checks if all datapacks required to connect to server are present
bool checkNames(datapackContainer* pointer) {
	for (unsigned short n = 0; n < mrNames.size(); n++) {
		bool wasFound = false;
		for (unsigned short m = 0; m < pointer->datapacks.size(); m++) {
			if (pointer->datapacks[m].name == mrNames[n]) {
				wasFound = true;
				break;
			}
		}
		if (!wasFound) {
			return 1;
		}
	}
	return 0;
}

// false for positive
bool sendConfirmation(sf::TcpSocket* socket, bool isNegative) {
	if (mrDebug) {
		std::cout << "MR Debug: Sending confirmation... \n";
	}
	char auth[1] = { isNegative };
	if (socket->send(auth, 1)) {
		if (mrDebug) {
			std::cout << "[ CRITICAL ] MR Debug: Could not send confirmation! \n";
		}
		return 1;
	}
	return auth[0];
}

bool getDatapackInfo(sf::TcpSocket* socket, datapackContainer* datapackPtr) {
	std::size_t received;
	char packet[8192];

	// get datapack data
	if (socket->receive(packet, 8192, received)) {
		if (mrDebug) {
			std::cout << "[ CRITICAL ] MR Debug: Could not receive datapack data packet! \n";
		}
		return 1;
	}
	if (mrDebug) {
		std::cout << "MR Debug: Primary datapack acquired with " << unsigned short(packet[0]) << " strings to interpret \n";
	}
	// vector for datapack names
	std::vector<std::string> names(packet[0], "");

	// checks if there will be multiple packets
	unsigned short len = 1;

	for (unsigned short n = 1; n <= packet[0]; n++) {
		len += packet[n];
	}

	if (len > 8192) {
		unsigned short count = len / 8192;
		if (mrDebug) {
			std::cout << "MR Debug: " << count << " additional datapack packets detected!\n";
		}

	

	}
	else {
		unsigned short currentString = 0;
		unsigned short currentIndex = 0;
		for (unsigned short n = 1 + packet[0]; n <= len; n++) {
			if (currentIndex < packet[currentString + 1]) {
				names[currentString].push_back(packet[n]);
			}
			else {
				currentIndex = 0;
				currentString++;
			}
		}
	}

	if (mrDebug) {
		std::cout << "MR Debug: Received datapack names: \n";
		for (unsigned short n = 0; n < names.size(); n++) {
			std::cout << "MR Debug: " << names[n] << "\n";
		}
	}

	mrNames = names;

	return 0;
}

//				CONVERSION TABLES

// prepares table that gives received datapack names proper equivalents on client side
void mrPrepareDatapackConversion(datapackContainer* pointer) {
	mrDatapackConvert.resize(mrNames.size());

	for (unsigned short n = 0; n < mrDatapackConvert.size(); n++) {
		for (unsigned short m = 0; n < pointer->datapacks.size(); m++) {
			if (pointer->datapacks[m].name == mrNames[n]) {
				mrDatapackConvert[n] = m;
				break;
			}
		}
	}
}

//		BACKGROUND

void mrPrepareBackgroundConversion(datapackContainer* pointer) {
	// prepares unordered maps converting string names to proper ids ingame for final conversion table
	mrBackgroundNames.resize(mrDatapackConvert.size());
	for (unsigned short n = 0; n < mrNames.size(); n++) {
		for (unsigned short m = 0; m < pointer->datapacks[mrDatapackConvert[n]].backgroundBlocks.size(); m++) {
			if (m != 0) {
				mrBackgroundNames[n][pointer->datapacks[mrDatapackConvert[n]].backgroundBlocks[m].name] = m;

				if (mrDebug) {
					std::cout << "MR Debug: Created string conversion for background: " << pointer->datapacks[mrDatapackConvert[n]].backgroundBlocks[m].name << " -> " << m << "\n";
				}
			}
			else {
				mrBackgroundNames[n]["air"] = m;
				if (mrDebug) {
					std::cout << "MR Debug: Created string conversion for background: air -> " << m << "\n";
				}
			}
		}
	}
}

// gets the conversion table packets from server
bool mrGetBackgroundConversion(sf::TcpSocket* socket, bool* error) {

	// required packet stuff
	std::size_t received;
	unsigned int packet[2048];

	if (socket->receive(packet, 8192, received)) {
		if (mrDebug) {
			std::cout << "[ CRITICAL ] MR Debug: Could not receive conversion table packet! \n";
		}
		return 1;
	}
	else if (mrDebug) {
		std::cout << "MR Debug: Received a packet with: " << packet[0] << " conversions \n";
	}

	unsigned int len = packet[0];
	bool ready = false;
	bool lengthAcquired = false;
	unsigned int currentIndex = 0;
	unsigned short currentStringLen;
	unsigned short currentStringIndex = 0;
	bool currentPart = 0;
	std::string currentString = "";

	mrConvertStruct temp;

	for (unsigned short current = 1; !ready; current++) {
		// resetting data and getting new packet
		if (current == 2048) {
			current = 0;
			
			if (socket->receive(packet, 8192, received)) {
				if (mrDebug) {
					std::cout << "[ CRITICAL ] MR Debug: Could not receive conversion table packet! \n";
				}
				return 1;
			}
			else if (mrDebug) {
				std::cout << "MR Debug: Received a packet with: " << packet[0] << " conversions \n";
			}
			
		}

		// gets datapack number
		if (!currentPart) {
			temp.datapackNumber = packet[current];

			currentPart = true;
		}
		// gets string length and content
		else {
			// if length is known
			if (!lengthAcquired) {
				currentStringLen = packet[current];
				lengthAcquired = true;
			}
			else {
				currentString += char(packet[current]);
				currentStringIndex++;

				if (currentStringIndex == currentStringLen) {
					lengthAcquired = false;
					currentPart = false;
					currentStringIndex = 0;
					currentIndex++;

					// presence check
					if (mrBackgroundNames[temp.datapackNumber].find(currentString) != mrBackgroundNames[temp.datapackNumber].end()) {
						temp.id = mrBackgroundNames[temp.datapackNumber][currentString];
						mrBackgroundConvert.push_back(temp);

						if (mrDebug) {
							std::cout << "MR Debug: Created conversion: " << currentIndex - 1 << " -> " << temp.datapackNumber << " " << temp.id << "\n";
						}

						currentString = "";

						if (currentIndex == len) {
							ready = true;
							break;
						}
					}
					else {
						if (mrDebug) {
							std::cout << "[ CRITICAL ] MR Debug: Missing elements, joining failed! \n";
						}

						*error = true;
						return 0;
					}
					

					
				}
			}
		}
	}

	return 0;
}

//		FLOOR

void mrPrepareFloorConversion(datapackContainer* pointer) {
	// prepares unordered maps converting string names to proper ids ingame for final conversion table
	mrFloorNames.resize(mrDatapackConvert.size());
	for (unsigned short n = 0; n < mrNames.size(); n++) {
		for (unsigned short m = 0; m < pointer->datapacks[mrDatapackConvert[n]].floorBlocks.size(); m++) {
			if (m != 0) {
				mrFloorNames[n][pointer->datapacks[mrDatapackConvert[n]].floorBlocks[m].name] = m;

				if (mrDebug) {
					std::cout << "MR Debug: Created string conversion for floor: " << pointer->datapacks[mrDatapackConvert[n]].floorBlocks[m].name << " -> " << m << "\n";
				}
			}
			else {
				mrFloorNames[n]["air"] = m;
				if (mrDebug) {
					std::cout << "MR Debug: Created string conversion for floor: air -> " << m << "\n";
				}
			}
		}
	}
}

// gets the conversion table packets from server
bool mrGetFloorConversion(sf::TcpSocket* socket, bool* error) {

	// required packet stuff
	std::size_t received;
	unsigned int packet[2048];

	if (socket->receive(packet, 8192, received)) {
		if (mrDebug) {
			std::cout << "[ CRITICAL ] MR Debug: Could not receive conversion table packet! \n";
		}
		return 1;
	}
	else if (mrDebug) {
		std::cout << "MR Debug: Received a packet with: " << packet[0] << " conversions \n";
	}

	unsigned int len = packet[0];
	bool ready = false;
	bool lengthAcquired = false;
	unsigned int currentIndex = 0;
	unsigned short currentStringLen;
	unsigned short currentStringIndex = 0;
	bool currentPart = 0;
	std::string currentString = "";

	mrConvertStruct temp;

	for (unsigned short current = 1; !ready; current++) {
		// resetting data and getting new packet
		if (current == 2048) {
			current = 0;

			if (socket->receive(packet, 8192, received)) {
				if (mrDebug) {
					std::cout << "[ CRITICAL ] MR Debug: Could not receive conversion table packet! \n";
				}
				return 1;
			}
			else if (mrDebug) {
				std::cout << "MR Debug: Received a packet with: " << packet[0] << " conversions \n";
			}

		}

		// gets datapack number
		if (!currentPart) {
			temp.datapackNumber = packet[current];

			currentPart = true;
		}
		// gets string length and content
		else {
			// if length is known
			if (!lengthAcquired) {
				currentStringLen = packet[current];
				lengthAcquired = true;
			}
			else {
				currentString += char(packet[current]);
				currentStringIndex++;

				if (currentStringIndex == currentStringLen) {
					lengthAcquired = false;
					currentPart = false;
					currentStringIndex = 0;
					currentIndex++;

					// presence check
					if (mrFloorNames[temp.datapackNumber].find(currentString) != mrFloorNames[temp.datapackNumber].end()) {
						temp.id = mrFloorNames[temp.datapackNumber][currentString];
						mrFloorConvert.push_back(temp);

						if (mrDebug) {
							std::cout << "MR Debug: Created conversion: " << currentIndex - 1 << " -> " << temp.datapackNumber << " " << temp.id << "\n";
						}

						currentString = "";

						if (currentIndex == len) {
							ready = true;
							break;
						}
					}
					else {
						if (mrDebug) {
							std::cout << "[ CRITICAL ] MR Debug: Missing elements, joining failed! \n";
						}

						*error = true;
						return 0;
					}
					
				}
			}
		}
	}

	return 0;
}

//		WALL

void mrPrepareWallConversion(datapackContainer* pointer) {
	// prepares unordered maps converting string names to proper ids ingame for final conversion table
	mrWallNames.resize(mrDatapackConvert.size());
	for (unsigned short n = 0; n < mrNames.size(); n++) {
		for (unsigned short m = 0; m < pointer->datapacks[mrDatapackConvert[n]].wallBlocks.size(); m++) {
			if (m != 0) {
				mrWallNames[n][pointer->datapacks[mrDatapackConvert[n]].wallBlocks[m].name] = m;

				if (mrDebug) {
					std::cout << "MR Debug: Created string conversion for wall: " << pointer->datapacks[mrDatapackConvert[n]].wallBlocks[m].name << " -> " << m << "\n";
				}
			}
			else {
				mrWallNames[n]["air"] = m;
				if (mrDebug) {
					std::cout << "MR Debug: Created string conversion for wall: air -> " << m << "\n";
				}
			}
		}
	}
}

// gets the conversion table packets from server
bool mrGetWallConversion(sf::TcpSocket* socket, bool* error) {

	// required packet stuff
	std::size_t received;
	unsigned int packet[2048];

	if (socket->receive(packet, 8192, received)) {
		if (mrDebug) {
			std::cout << "[ CRITICAL ] MR Debug: Could not receive conversion table packet! \n";
		}
		return 1;
	}
	else if (mrDebug) {
		std::cout << "MR Debug: Received a packet with: " << packet[0] << " conversions \n";
	}

	unsigned int len = packet[0];
	bool ready = false;
	bool lengthAcquired = false;
	unsigned int currentIndex = 0;
	unsigned short currentStringLen;
	unsigned short currentStringIndex = 0;
	bool currentPart = 0;
	std::string currentString = "";

	mrConvertStruct temp;

	for (unsigned short current = 1; !ready; current++) {
		// resetting data and getting new packet
		if (current == 2048) {
			current = 0;

			if (socket->receive(packet, 8192, received)) {
				if (mrDebug) {
					std::cout << "[ CRITICAL ] MR Debug: Could not receive conversion table packet! \n";
				}
				return 1;
			}
			else if (mrDebug) {
				std::cout << "MR Debug: Received a packet with: " << packet[0] << " conversions \n";
			}

		}

		// gets datapack number
		if (!currentPart) {
			temp.datapackNumber = packet[current];

			currentPart = true;
		}
		// gets string length and content
		else {
			// if length is known
			if (!lengthAcquired) {
				currentStringLen = packet[current];
				lengthAcquired = true;
			}
			else {
				currentString += char(packet[current]);
				currentStringIndex++;

				if (currentStringIndex == currentStringLen) {
					lengthAcquired = false;
					currentPart = false;
					currentStringIndex = 0;
					currentIndex++;

					// presence check
					if (mrWallNames[temp.datapackNumber].find(currentString) != mrWallNames[temp.datapackNumber].end()) {
						temp.id = mrWallNames[temp.datapackNumber][currentString];
						mrWallConvert.push_back(temp);

						if (mrDebug) {
							std::cout << "MR Debug: Created conversion: " << currentIndex - 1 << " -> " << temp.datapackNumber << " " << temp.id << "\n";
						}

						currentString = "";

						if (currentIndex == len) {
							ready = true;
							break;
						}
					}
					else {
						if (mrDebug) {
							std::cout << "[ CRITICAL ] MR Debug: Missing elements, joining failed! \n";
						}

						*error = true;
						return 0;
					}
					
				}
			}
		}
	}

	return 0;
}


bool mrgetConversionTables(sf::TcpSocket* socket, datapackContainer* datapackPtr) {

	bool isThereAProblem = false;

	mrPrepareDatapackConversion(datapackPtr);

	// background

	mrPrepareBackgroundConversion(datapackPtr);

	if (mrDebug) {
		std::cout << "[ STARTING ] MR Debug: Getting background conversion table packet... \n";
	}

	if (mrGetBackgroundConversion(socket, &isThereAProblem)) {
		return 1;
	}

	// floor

	mrPrepareFloorConversion(datapackPtr);

	if (mrDebug) {
		std::cout << "[ STARTING ] MR Debug: Getting floor conversion table packet... \n";
	}

	if (mrGetFloorConversion(socket, &isThereAProblem)) {
		return 1;
	}

	// wall

	mrPrepareWallConversion(datapackPtr);

	if (mrDebug) {
		std::cout << "[ STARTING ] MR Debug: Getting wall conversion table packet... \n";
	}

	if (mrGetWallConversion(socket, &isThereAProblem)) {
		return 1;
	}

	return isThereAProblem;
}

//				MAP TRANSFER

void mrPrepareBlockStates(dimension* pointer) {
	if (mrDebug) {
		std::cout << "MR Debug: Creating block states... \n";
	}

	
	for (unsigned short y = 0; y < pointer->size.y; y++) {
		for (unsigned short x = 0; x < pointer->size.x; x++) {
			pointer->backgrounds.blocks[y][x].create();
			pointer->floors.blocks[y][x].create();
			pointer->walls.blocks[y][x].create();
		}
	}
	

	if (mrDebug) {
		std::cout << "MR Debug: Created block states! \n";
	}

}

bool mrGetMapContent(mapContainer* map, sf::TcpSocket* socket, datapackContainer* pointer) {
	// cleans the map container just to be sure
	map->dimensions.clear();

	if (mrDebug) {
		std::cout << "[ STARTING ] MR Debug: Getting map content... \n";
	}


	// required packet stuff
	std::size_t received;
	unsigned int packet[2048];

	


	if (socket->receive(packet, 8192, received)) {
		if (mrDebug) {
			std::cout << "[ CRITICAL ] MR Debug: Could not receive map content packet! \n";
		}
		return 1;
	}
	else if (mrDebug) {
		std::cout << "MR Debug: Received a packet with: " << packet[1] << " dimensions and time: " << packet[0] << "\n";
	}

	// base data
	unsigned short len = packet[1];
	map->time = packet[0];

	bool ready = false;
	unsigned short currentIndex = 0;
	unsigned short currentStringIndex = 0;
	unsigned short currentStringLen;
	unsigned short currentPart = 0;
	unsigned short packetsGotten = 0;
	bool isInfoReady = false;

	sf::Vector2i mapVec = sf::Vector2i(0, 0);

	std::vector<std::string> names(len, "");
	std::vector<sf::Vector2i> sizes(len);

	// time, no of dimensions, <name length, string chars, size.x, size.y> * no, converted data tables * no, y: 0 -> n.y, x 0 -> n.x
	for (unsigned short current = 2; !ready; current++) {
		// another packet
		if (current == 2048) {
			current = 0;

			// gets another packet if size excedded
			if (socket->receive(packet, 8192, received)) {
				if (mrDebug) {
					std::cout << "[ CRITICAL ] MR Debug: Could not receive another map content packet! \n";
				}
				return 1;
			}

			packetsGotten++;
		}
		// map data
		if (!isInfoReady) {
			// str len
			if (currentPart == 0) {
				currentStringLen = packet[current];
				currentPart++;
			}
			// str content
			else if (currentPart == 1) {
				names[currentIndex] += char(packet[current]);

				currentStringIndex++;

				// if string is done
				if (currentStringIndex == currentStringLen) {
					currentStringIndex = 0;
					currentPart++;
				}
			}
			// size.x
			else if (currentPart == 2) {
				sizes[currentIndex].x = packet[current];
				currentPart++;

			}
			// size.y
			else {
				sizes[currentIndex].y = packet[current];

				currentPart = 0;
				currentIndex++;

				if (currentIndex == len) {
					isInfoReady = true;
					currentIndex = 0;


					// prepares map

					if (mrDebug) {
						std::cout << "[ MILESTONE ] MR Debug: Data acquired, creating map to accomodate for data... \n";
					}

					map->create(sizes, names, "External", map->time);
					

				}

			}
		}
		// map numbers
		else {
			
			// background
			if (currentPart == 0) {
				
				// sorry
				map->dimensions[currentIndex].backgrounds.blocks[mapVec.y][mapVec.x].prepare(&pointer->datapacks[mrBackgroundConvert[packet[current]].datapackNumber].backgroundBlocks[mrBackgroundConvert[packet[current]].id], mapVec, blockBaseSize);

				// new and better, temp variant
				map->dimensions[currentIndex].grid[mapVec.y][mapVec.x].createBackground(&pointer->datapacks[mrBackgroundConvert[packet[current]].datapackNumber].backgroundBlocks[mrBackgroundConvert[packet[current]].id].variants[0]);



				mapVec.x++;


				if (mapVec.x == map->dimensions[currentIndex].size.x) {
					mapVec.x = 0;
					mapVec.y++;

					if (mapVec.y == map->dimensions[currentIndex].size.y) {
						mapVec.y = 0;
						currentPart++;

					}
				}
			}
			// floor
			else if (currentPart == 1) {
				// sorry
				map->dimensions[currentIndex].floors.blocks[mapVec.y][mapVec.x].prepare(&pointer->datapacks[mrFloorConvert[packet[current]].datapackNumber].floorBlocks[mrFloorConvert[packet[current]].id], mapVec, blockBaseSize);

				// new and better, temp variant
				map->dimensions[currentIndex].grid[mapVec.y][mapVec.x].createFloor(&pointer->datapacks[mrFloorConvert[packet[current]].datapackNumber].floorBlocks[mrFloorConvert[packet[current]].id].variants[0]);


				mapVec.x++;


				if (mapVec.x == map->dimensions[currentIndex].size.x) {
					mapVec.x = 0;
					mapVec.y++;

					if (mapVec.y == map->dimensions[currentIndex].size.y) {
						mapVec.y = 0;
						currentPart++;

					}
				}
			}
			// wall
			else {
				// sorry
				map->dimensions[currentIndex].walls.blocks[mapVec.y][mapVec.x].prepare(&pointer->datapacks[mrWallConvert[packet[current]].datapackNumber].wallBlocks[mrWallConvert[packet[current]].id], mapVec, blockBaseSize);

				// new and better, temp variant
				map->dimensions[currentIndex].grid[mapVec.y][mapVec.x].createWall(&pointer->datapacks[mrWallConvert[packet[current]].datapackNumber].wallBlocks[mrWallConvert[packet[current]].id].variants[0]);

				mapVec.x++;

				if (mapVec.x == map->dimensions[currentIndex].size.x) {
					mapVec.x = 0;
					mapVec.y++;

					if (mapVec.y == map->dimensions[currentIndex].size.y) {
						mapVec.y = 0;

						mrPrepareBlockStates(&map->dimensions[currentIndex]);

						currentPart = 0;
						currentIndex++;

						// prepares block states (temp?)
						
						if (currentIndex == len) {
							ready = true;

							if (mrDebug) {
								std::cout << "MR Debug: Map content sent using: " << (current + 2048 * packetsGotten) * 4 << " bytes, with " << packetsGotten + 1 << " packets \n";
							}

						}
					}
				}
			}
		}
	}

	return 0;
}

// declared here due to parsing, compilation or some other minor shit
void mlPrepareDimensionRenderGrid(dimension* pointer, bool debug);

bool receiveMap(mapContainer* map, sf::TcpSocket* socket, datapackContainer* datapackPtr, bool debug, bool* startGame, bool* stIsFrozen, bool* stIsRunning) {
	mrDebug = debug;

	*stIsFrozen = true;

	while (*stIsRunning) {
		//std::cout << *stIsFrozen << " " << *stIsRunning << "\n";
		sf::sleep(sf::milliseconds(1));
	}

	// gets datapack names for ones used on server
	if (getDatapackInfo(socket, datapackPtr)) {
		return 1;
	}

	// sends confirmation or not
	if (sendConfirmation(socket, checkNames(datapackPtr))) {
		return 1;
	}

	// confirmation that all conversion tables are correct and present
	if (sendConfirmation(socket, mrgetConversionTables(socket, datapackPtr))) {
		return 1;
	}

	// gets the map container filled with delicious pointers
	if (mrGetMapContent(map, socket, datapackPtr)) {
		return 1;
	}

	// temp here probably
	for (unsigned short n = 0; n < map->dimensions.size(); n++) {
		mlPrepareDimensionRenderGrid(&map->dimensions[n], mrDebug);
		debugMsg(std::string("MR Debug: Finished preparing dimension with size of: " + std::to_string(map->dimensions[n].size.x) + " " + std::to_string(map->dimensions[n].size.y)));
	}


	// waits for hybrid render
	canHybridRenderServiceStartWorking = true;


	while (!isHybridRenderServiceReady) {
		sf::sleep(sf::microseconds(100));
	}

	// sends confirmation or not
	if (sendConfirmation(socket, false)) {
		return 1;
	}

	

	return 0;
}