#pragma once

#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include "map.h"
#include <vector>
#include <unordered_map>
#include "transferMisc.h"

bool msDebug;

std::vector<mtConvertInfo> msBackgroundConvert;
std::vector<mtConvertInfo> msFloorConvert;
std::vector<mtConvertInfo> msWallConvert;

mtConvertToTransferStruct msConvert;

// sends information about current datapacks to client
bool sendDatapacks(std::string saveName, sf::TcpSocket* socket, std::vector<std::string> names) {


	// converts to sendable packet(s)

	bool ready = false;
	unsigned int index = 0;
	unsigned int current = 1;
	unsigned short packetNumber = 0;

	// for now 255 datapack limit and 255 character name limit for resource conservation with 8kB packet, can be changed to other size easly
	// sending over multiple packets *should* work but was not tested properly
	char packet[8192];

	packet[0] = names.size();

	for (unsigned short n = 1; n <= names.size(); n++) {
		packet[n] = names[n - 1].size();
		current = n + 1;
	}

	unsigned short currentString = 0;
	unsigned short currentIndex = 0;

	while (!ready) {
		for (; current < 8192; current++) {
			if (names[currentString].size() > currentIndex and currentString != names.size()) {
				packet[current] = names[currentString][currentIndex];
				currentIndex++;
			}
			else {
				currentString++;
				currentIndex = 0;
				if (currentString == names.size()) {
					current--;
					if (msDebug) {
						std::cout << "MS Debug: Packed all datapack info into: " << 1 + current + packetNumber * 1024 << " bytes \n";
					}
					if (sendPacket(socket, packet)) {
						return 1;
					}
					ready = true;
					break;
				}
			}
		}

		if (current == 8192 and !ready) {
			current = 0;
			packetNumber++;
			if (sendPacket(socket, packet)) {
				return 1;
			}
		}
	}
	return 0;
}

//				CONVERSIONS

void createBackgroundConversion(datapackContainer* pointer) {
	// <number sent> <datapack number> <object name>
	unsigned int number = 0;
	for (unsigned short n = 0; n < pointer->datapacks.size(); n++) {
		msConvert.backgrounds.resize(msConvert.backgrounds.size() + 1);
		for (unsigned short m = 0; m < pointer->datapacks[n].backgroundBlocks.size(); m++) {
			mtConvertInfo temp;
			temp.datapackNumber = n;
			temp.name = pointer->datapacks[n].backgroundBlocks[m].name;
			if (!m) {
				temp.name = "air";
			}
			temp.sendNumber = number;
			msBackgroundConvert.push_back(temp);

			if (msDebug) {
				std::cout << "MS Debug: Added background send conversion: " << temp.datapackNumber << " and " << temp.name << " -> " << temp.sendNumber << "\n";
				std::cout << "MS Debug: And internal conversion: " << n << " " << m << " -> " << number << "\n";
			}
			msConvert.backgrounds[n].push_back(number);
			number++;
		}
	}
	
}

void createFloorConversion(datapackContainer* pointer) {
	// <number sent> <datapack number> <object name>
	unsigned int number = 0;
	for (unsigned short n = 0; n < pointer->datapacks.size(); n++) {
		msConvert.floors.resize(msConvert.floors.size() + 1);
		for (unsigned short m = 0; m < pointer->datapacks[n].floorBlocks.size(); m++) {
			mtConvertInfo temp;
			temp.datapackNumber = n;
			temp.name = pointer->datapacks[n].floorBlocks[m].name;
			if (!m) {
				temp.name = "air";
			}
			temp.sendNumber = number;
			msFloorConvert.push_back(temp);

			if (msDebug) {
				std::cout << "MS Debug: Added floor send conversion: " << temp.datapackNumber << " and " << temp.name << " -> " << temp.sendNumber << "\n";
				std::cout << "MS Debug: And internal conversion: " << n << " " << m << " -> " << number << "\n";
			}
			msConvert.floors[n].push_back(number);
			number++;
		}
	}

}

void createWallConversion(datapackContainer* pointer) {
	// <number sent> <datapack number> <object name>
	unsigned int number = 0;
	for (unsigned short n = 0; n < pointer->datapacks.size(); n++) {
		msConvert.walls.resize(msConvert.walls.size() + 1);
		for (unsigned short m = 0; m < pointer->datapacks[n].wallBlocks.size(); m++) {
			mtConvertInfo temp;
			temp.datapackNumber = n;
			temp.name = pointer->datapacks[n].wallBlocks[m].name;
			if (!m) {
				temp.name = "air";
			}
			temp.sendNumber = number;
			msWallConvert.push_back(temp);

			if (msDebug) {
				std::cout << "MS Debug: Added wall send conversion: " << temp.sendNumber << " -> " << temp.datapackNumber << " and " << temp.name << "\n";
				std::cout << "MS Debug: And internal conversion: " << n << " " << m << " -> " << number << "\n";
			}
			msConvert.walls[n].push_back(number);
			number++;
		}
	}

}

// packs and sends the conversion in an unsigned int packet
bool sendConversionProper(sf::TcpSocket* socket, std::vector<mtConvertInfo>* pointer) {

	unsigned int packet[2048];
	// the amount of conversions in this one
	packet[0] = pointer->size();
	bool ready = false;

	unsigned int len = pointer->size();
	unsigned short currentIndex = 0;
	unsigned short currentPart = 0;
	short currentStringIndex = -1;
	unsigned short packetsSent = 0;
	// fills the packet
	for (unsigned short current = 1; !ready; current++) {
		// resets to avoid out of range
		if (current == 2048) {
			if (sendPacket(socket, packet, msDebug)) {
				return 1;
			}
			packetsSent++;
			current = 0;
		}

		// datapack number
		if (currentPart == 0) {
			packet[current] = pointer->at(currentIndex).datapackNumber;
			currentPart++;
		}

		// name
		else  {
			if (currentStringIndex == -1) {
				packet[current] = pointer->at(currentIndex).name.size();
			}
			else {
				packet[current] = pointer->at(currentIndex).name[currentStringIndex];
			}

			currentStringIndex++;

			if (currentStringIndex == pointer->at(currentIndex).name.size()) {
				currentPart = 0;
				currentStringIndex = -1;

				currentIndex++;

				// checks if it's the end
				if (currentIndex == len) {
					ready = true;
					if (current) {
						if (sendPacket(socket, packet, msDebug)) {
							return 1;
						}
					}

					if (msDebug) {
						std::cout << "MS Debug: Conversion tables sent using: " << (current + 2048 * packetsSent) * 4 << " bytes \n";
					}

					break;
				}
			}
		}
	}

	return 0;
}

// creates and sends conversions for data on server side
bool sendConversion(sf::TcpSocket* socket, datapackContainer* datapackPtr, std::vector<std::string> names) {

	// backgrounds

	if (msDebug) {
		std::cout << "[ STARTING ] MS Debug: Creating background conversion... \n";
	}

	createBackgroundConversion(datapackPtr);

	if (msDebug) {
		std::cout << "[ STARTING ] MS Debug: Sending background conversion... \n";
	}

	if (sendConversionProper(socket, &msBackgroundConvert)) {
		return 1;
	}

	// floors

	if (msDebug) {
		std::cout << "[ STARTING ] MS Debug: Creating floor conversion... \n";
	}

	createFloorConversion(datapackPtr);

	if (msDebug) {
		std::cout << "[ STARTING ] MS Debug: Sending floor conversion... \n";
	}

	if (sendConversionProper(socket, &msFloorConvert)) {
		return 1;
	}

	// walls

	if (msDebug) {
		std::cout << "[ STARTING ] MS Debug: Creating wall conversion... \n";
	}

	createWallConversion(datapackPtr);

	if (msDebug) {
		std::cout << "[ STARTING ] MS Debug: Sending wall conversion... \n";
	}

	if (sendConversionProper(socket, &msWallConvert)) {
		return 1;
	}

	return 0;
}

//				MAP TRANSFER

bool sendMapProper(mapContainer* map, sf::TcpSocket* socket) {

	if (msDebug) {
		std::cout << "MS Debug: Sending map contents... \n";
	}

	// packet info:
	// 0 - current time
	// 1 - no of dimensions
	// ... length of name, len * char, size x, size y
	// ... size x * size y converted block types for each dimension, backgrounds, floors, walls

	int packet[2048];
	packet[0] = map->time;
	packet[1] = map->dimensions.size();

	bool ready = false;
	bool wasInfoMade = false;
	unsigned int len = map->dimensions.size();
	unsigned short currentIndex = 0;
	unsigned short currentPart = 0;
	short currentStringIndex = 0;
	unsigned short packetsSent = 0;
	// dimension navigation
	sf::Vector2i mapVec = sf::Vector2i(0, 0);
	unsigned short currentLayer = 0;

	for (unsigned short current = 2; !ready; current++) {
		// resetting current index
		if (current == 2048) {
			current = 0;
			packetsSent++;

			if (sendPacket(socket, packet, msDebug)) {
				return 1;
			}
		}

		// for the first part
		if (!wasInfoMade) {
			// name length
			if (currentPart == 0) {
				packet[current] = map->dimensions[currentIndex].name.size();
				currentPart++;

			}
			// name chars
			else if (currentPart == 1) {
				packet[current] = map->dimensions[currentIndex].name[currentStringIndex];
				currentStringIndex++;


				if (currentStringIndex == map->dimensions[currentIndex].name.size()) {
					currentStringIndex = 0;
					currentPart++;
				}
			}
			// size x
			else if(currentPart == 2) {
				packet[current] = map->dimensions[currentIndex].size.x;
				currentPart++;

			}
			// size y
			else {
				packet[current] = map->dimensions[currentIndex].size.y;
				currentPart = 0;
				currentIndex++;


				// ends info writing and starts block writing
				if (currentIndex == len) {
					wasInfoMade = true;
					currentIndex = 0;

				}
			}
		}
		// for map content
		else {

			// background
			if (currentLayer == 0) {

				// gets converted number from datapack number and internal id
				packet[current] = msConvert.backgrounds[map->dimensions[currentIndex].grid[mapVec.y][mapVec.x].background->datapackId][map->dimensions[currentIndex].grid[mapVec.y][mapVec.x].background->internalId];
				mapVec.x++;

				if (mapVec.x == map->dimensions[currentIndex].size.x) {
					mapVec.x = 0;
					mapVec.y++;


					if (mapVec.y == map->dimensions[currentIndex].size.y) {
						mapVec.y = 0;
						currentLayer++;

					}
				}
			}
			// floor
			else if (currentLayer == 1) {

				// gets converted number from datapack number and internal id
				packet[current] = msConvert.floors[map->dimensions[currentIndex].grid[mapVec.y][mapVec.x].floor->datapackId][map->dimensions[currentIndex].grid[mapVec.y][mapVec.x].floor->internalId];

				mapVec.x++;


				if (mapVec.x == map->dimensions[currentIndex].size.x) {
					mapVec.x = 0;
					mapVec.y++;



					if (mapVec.y == map->dimensions[currentIndex].size.y) {
						mapVec.y = 0;
						currentLayer++;

					}
				}
			}
			// wall
			else {

				// gets converted number from datapack number and internal id
				packet[current] = msConvert.walls[map->dimensions[currentIndex].grid[mapVec.y][mapVec.x].wall->datapackId][map->dimensions[currentIndex].grid[mapVec.y][mapVec.x].wall->internalId];

				mapVec.x++;


				if (mapVec.x == map->dimensions[currentIndex].size.x) {
					mapVec.x = 0;
					mapVec.y++;


					if (mapVec.y == map->dimensions[currentIndex].size.y) {
						mapVec.y = 0;
						currentLayer = 0;
						currentIndex++;

						if (currentIndex == len) {
							ready = true;

							// send final packet
							if (sendPacket(socket, packet, msDebug)) {
								return 1;
							}

							if (msDebug) {
								std::cout << "MS Debug: Map content sent using: " << (current + 2048 * packetsSent) * 4 << " bytes, with " << packetsSent + 1 << " packets \n";
							}

						}
					}
				}
			}
		}
	}


	return 0;
}

bool sendMap(mapContainer* map, sf::TcpSocket* socket, bool debug, std::vector<std::string> names, datapackContainer* datapackPtr) {
	msDebug = debug;

	// send active datapack list
	if (sendDatapacks(map->name, socket, names)) {
		return 1;
	}

	// get a compability confirmation byte
	if (getConfirmation(socket, msDebug)) {
		return 1;
	}

	if (sendConversion(socket, datapackPtr, names)) {
		return 1;
	}

	// get a compability confirmation byte
	if (getConfirmation(socket, msDebug)) {
		return 1;
	}

	// sends map content to client
	if (sendMapProper(map, socket)) {
		return 1;
	}

	// get a compability confirmation byte
	if (getConfirmation(socket, msDebug)) {
		return 1;
	}

	return 0;
}