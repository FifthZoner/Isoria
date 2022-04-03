#pragma once

#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include "map.h"

bool mrDebug;
std::size_t received;

bool getDatapackInfo(sf::TcpSocket* socket, datapackContainer* datapackPtr) {
	char packet[1024];

	// get datapack data
	if (socket->receive(packet, 1024, received)) {
		if (mrDebug) {
			std::cout << "[ CRITICAL ] CT Debug: Could not receive datapack data packet! \n";
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

	if (len > 1024) {
		unsigned short count = len / 1024;
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
}

bool receiveMap(mapContainer* map, sf::TcpSocket* socket, datapackContainer* datapackPtr, bool debug) {
	mrDebug = debug;

	// checks if all necessary datapacks are present to avoid critical issues
	getDatapackInfo(socket, datapackPtr);


	return 0;
}