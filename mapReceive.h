#pragma once

#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include "map.h"

bool mrDebug;

std::vector<std::string> mrNames;

// checks if all datapacks required to connect to server wre present
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

bool receiveMap(mapContainer* map, sf::TcpSocket* socket, datapackContainer* datapackPtr, bool debug) {
	mrDebug = debug;

	// gets datapack names for ones used on server
	if (getDatapackInfo(socket, datapackPtr)) {
		return 1;
	}

	// sends confirmation or not
	if (sendConfirmation(socket, checkNames(datapackPtr))) {
		return 1;
	}

	// gets conversion<something>.txt file contents

	return 0;
}