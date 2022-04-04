#pragma once

#include <vector>
#include <string>
#include "map.h"

// for 8192 byte with 8192 chars
bool sendPacket(sf::TcpSocket* socket, char packet[8192], bool debug = 1) {

	if (socket->send(packet, 8192)) {
		if (debug) {
			std::cout << "[ CRITICAL ] MS Debug: Could not send data! \n";
		}
		return 1;
	}

	return 0;
}

// for 8192 byte with 2048 unsigned ints
bool sendPacket(sf::TcpSocket* socket, unsigned int packet[2048], bool debug = 1) {

	if (socket->send(packet, 8192)) {
		if (debug) {
			std::cout << "[ CRITICAL ] MS Debug: Could not send data! \n";
		}
		return 1;
	}

	return 0;
}

// for 8192 byte with 2048 ints
bool sendPacket(sf::TcpSocket* socket, int packet[2048], bool debug = 1) {

	if (socket->send(packet, 8192)) {
		if (debug) {
			std::cout << "[ CRITICAL ] MS Debug: Could not send data! \n";
		}
		return 1;
	}

	return 0;
}

// for 8192 byte with 4096 unsigned shorts
bool sendPacket(sf::TcpSocket* socket, unsigned short packet[4096], bool debug = 1) {

	if (socket->send(packet, 8192)) {
		if (debug) {
			std::cout << "[ CRITICAL ] MS Debug: Could not send data! \n";
		}
		return 1;
	}

	return 0;
}

// for 8192 byte with 4096 shorts
bool sendPacket(sf::TcpSocket* socket, short packet[4096], bool debug = 1) {

	if (socket->send(packet, 8192)) {
		if (debug) {
			std::cout << "[ CRITICAL ] MS Debug: Could not send data! \n";
		}
		return 1;
	}

	return 0;
}

bool getConfirmation(sf::TcpSocket* socket, bool msDebug = 1) {
	std::size_t received;

	// 0 for correct
	char auth[1];
	if (socket->receive(auth, 1, received)) {
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


struct mtConvertInfo {
	unsigned short datapackNumber = NULL;
	unsigned int sendNumber = NULL;
	std::string name = "";
};

struct mtConvertToTransferStruct {
	std::vector<std::vector<unsigned int>> backgrounds;
	std::vector<std::vector<unsigned int>> floors;
	std::vector<std::vector<unsigned int>> walls;
};

struct mrConvertStruct {
	unsigned short datapackNumber = NULL;
	unsigned short id = NULL;
};