#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <iostream>

#include "quickWrite.h"
#include "client.h"
#include "serverMisc.h"
#include "map.h"

// the server file, has absolute authority over all gameplay logic and can correct future prediction inaccuracies

unsigned short clientCap = 8;

//		THREAD RELATED

std::vector<clientStruct> clientThreads(clientCap);
std::mutex lock;
std::thread masterThread;
std::vector<bool> socketsToUse(clientCap);

bool* isAllAlright;
bool stDebug = true;
bool* stIsFrozen;
bool* stIsRunning;
unsigned short freezeInstances = 0;

ushort connectedSockets = 0;
ushort maxSockets = 1;

//		SERVER RELATED
sf::TcpListener listener;

//		CLIENTS

bool stCommunication(clientStruct* pointer) {

	// temp test packet
	char packet[5] = { 'h','e','l','l','o' };

	if (!pointer->socket.send(packet, 5)) {
		if (stDebug) {
			"[ CRITICAL ] CT Debug: Could not communicate with client! \n";
		}
		return 1;
	}

	std::size_t received;

	if (!pointer->socket.receive(packet, 5, received)) {
		if (stDebug) {
			"[ CRITICAL ] CT Debug: Could not communicate with client! \n";
		}
		return 1;
	}

	return 0;
}

// a main thread of a socket
char stSocketThread(clientStruct* pointer) {

	// starting a connection with client

	// connection type

	std::size_t received;

	char auth[1];

	if (!pointer->socket.receive(auth, 1, received)) {
		return 1;
	}

	if (stDebug) {
		if (auth[0] == 0) {
			std::cout << "[ NEW CLIENT ] ST Debug: Accepted new local client! \n";
		}
		else {
			std::cout << "[ NEW CLIENT ] ST Debug: Accepted new external client! \n";
		}
	}
	// add datapack checking here


	// freezing the game

	if (stDebug) {
		std::cout << "ST Debug: Freezing the game... \n";
	}

	freezeInstances++;
	*stIsFrozen = true;

	while (*stIsRunning) {
		sf::sleep(sf::milliseconds(5));
	}

	// transfer map only to external clients
	if (auth[0]) {
		if (stDebug) {
			std::cout << "ST Debug: Starting map transfer... \n";
		}
	}
	


	freezeInstances--;
	if (!freezeInstances) {
		if (stDebug) {
			std::cout << "ST Debug: Unfreezing the game... \n";
		}
		*stIsFrozen = false;
	}


	// main socket loop
	while (*isAllAlright) {

		if (stCommunication(pointer)) {
			if (stDebug) {
				"[ WARNING ] ST Debug: Could not communicat with client! \n";
			}
			break;
		}

	}

	// disconnection

	pointer->socket.disconnect();
	pointer->isUsed = false;

	return 0;
}

//		LISTENER

ushort getSocketToUse() {
	for (ushort n = 0; n < maxSockets; n++) {
		if (!clientThreads[n].isUsed) {
			return n;
		}
	}
}

void stListenerThread() {
	bool isSocketFound = false;
	ushort socketToUse = NULL;

	while (*isAllAlright) {

		// check if max connection amount was not reached
		if (connectedSockets < maxSockets) {
			if (!isSocketFound) {
				isSocketFound = true;
				socketToUse = getSocketToUse();
			}

			if (listener.accept(clientThreads[socketToUse].socket) == sf::Socket::Done) {
				if (clientThreads[socketToUse].wasUsed) {
					clientThreads[socketToUse].wasUsed = false;
					clientThreads[socketToUse].thread.join();
				}
				clientThreads[socketToUse].thread = std::thread(stSocketThread, &clientThreads[socketToUse]);
				clientThreads[socketToUse].isUsed = true;
				clientThreads[socketToUse].wasUsed = true;
			}

		}
		
		// 5 connection attempts per second, more than enough
		sf::sleep(sf::milliseconds(200));
	}

	for (unsigned short n = 0; n < clientCap; n++) {
		if (clientThreads[n].isUsed) {
			clientThreads[n].thread.join();
		}
	}

}

//		MAIN

bool stPrepareBaseFunctions() {

	listener.setBlocking(false);

	// binding the socket to port * 21370 *
	if (!listener.listen(21370) != sf::Socket::Done) {
		return 0;

		if (stDebug) {
			std::cout << "[ CRITICAL ] ST Debug: Listener setup failed! \n";
		}
	}

	masterThread = std::thread(stListenerThread);

	return 1;
}

// main function thar runs the server in a seperate thread, send serverStatus as a true bool
void serverFunction(mapContainer* map, bool* serverStatusPtr, bool* isFrozenPtr, bool* isRunningPtr, ushort clientAmount = 8, bool debug = true) {

	stIsFrozen = isFrozenPtr;
	stIsRunning = isRunningPtr;
	maxSockets = clientAmount;
	clientCap = clientAmount;
	stDebug = debug;
	isAllAlright = serverStatusPtr;

	if (stDebug) {
		std::cout << "[ STARTING ] ST debug: Starting server thread... \n";
	}


	if (stPrepareBaseFunctions()) {
		if (stDebug) {
			std::cout << "[ MILESTONE ] ST debug: Server functions prepared succesfully, starting the loop... \n";
		}

		while (*isAllAlright) {



		}
	}

	masterThread.join();
}
