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


bool sendDatapackPacket(sf::TcpSocket* socket, char packet[8192]) {

	if (socket->send(packet, 8192)) {
		if (msDebug) {
			std::cout << "[ CRITICAL ] MS Debug: Could not send datapack data! \n";
		}
		return 1;
	}

	return 0;
}

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
					if (sendDatapackPacket(socket, packet)) {
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
			if (sendDatapackPacket(socket, packet)) {
				return 1;
			}
		}
	}
	return 0;
}

bool getConfirmation(sf::TcpSocket* socket) {
	std::size_t received;

	// 0 for correct
	char auth[1];
	if (socket->receive(auth, 1024, received)) {
		if (msDebug) {
			std::cout << "[ CRITICAL ] MS Debug: Could not receive confirmation! \n";
		}
		return 1;
	}

	if (msDebug) {
		if (!auth[0]) {
			std::cout << "MS Debug: Positive confirmation acquired! \n";
		}
		else {
			std::cout << "MS Debug: Negative confirmation acquired! \n";
		}
	}

	return auth[0];
}

//				CONVERSIONS

void createBackgroundConversion(datapackContainer* pointer) {
	// <number sent> <datapack number> <object name>
	unsigned int number = 0;
	for (unsigned short n = 0; n < pointer->datapacks.size(); n++) {
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
			}
			number++;
		}
	}
	
}

void createFloorConversion(datapackContainer* pointer) {
	// <number sent> <datapack number> <object name>
	unsigned int number = 0;
	for (unsigned short n = 0; n < pointer->datapacks.size(); n++) {
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
			}
			number++;
		}
	}

}

void createWallConversion(datapackContainer* pointer) {
	// <number sent> <datapack number> <object name>
	unsigned int number = 0;
	for (unsigned short n = 0; n < pointer->datapacks.size(); n++) {
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
				std::cout << "MS Debug: Added wall send conversion: " << temp.datapackNumber << " and " << temp.name << " -> " << temp.sendNumber << "\n";
			}
			number++;
		}
	}

}



// creates and sends conversions for data on server side
bool sendConversion(sf::TcpSocket* socket, datapackContainer* datapackPtr, std::vector<std::string> names) {

	createBackgroundConversion(datapackPtr);

	createFloorConversion(datapackPtr);

	createWallConversion(datapackPtr);

	return 0;
}

bool sendMap(mapContainer* map, sf::TcpSocket* socket, bool debug, std::vector<std::string> names, datapackContainer* datapackPtr) {
	msDebug = debug;

	// send active datapack list
	if (sendDatapacks(map->name, socket, names)) {
		return 1;
	}

	// get a compability confirmation byte
	if (getConfirmation(socket)) {
		return 1;
	}

	if (sendConversion(socket, datapackPtr, names)) {
		return 1;
	}

	return 0;
}