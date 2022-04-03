#pragma once

#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include "map.h"

bool msDebug;

bool sendDatapackPacket(sf::TcpSocket* socket, char packet[1024]) {

	if (socket->send(packet, 1024)) {
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

	// for now 255 datapack limit and 255 character name limit for resource conservation with 1kB packet, can be changed to other size easly
	// sending over multiple packets *should* work but was not tested properly
	char packet[1024];

	packet[0] = names.size();

	for (unsigned short n = 1; n <= names.size(); n++) {
		packet[n] = names[n - 1].size();
		current = n + 1;
	}

	unsigned short currentString = 0;
	unsigned short currentIndex = 0;

	while (!ready) {
		for (; current < 1024; current++) {
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

		if (current == 1024 and !ready) {
			current = 0;
			packetNumber++;
			if (sendDatapackPacket(socket, packet)) {
				return 1;
			}
		}
	}
	return 0;
}

bool sendMap(mapContainer* map, sf::TcpSocket* socket, bool debug, std::vector<std::string> names) {
	msDebug = debug;

	sendDatapacks(map->name, socket, names);

	return 0;
}