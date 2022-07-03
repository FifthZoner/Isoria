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
#include "mapSend.h"
#include "shared.h"

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

bool stopListener = true;

ushort connectedSockets = 0;
ushort maxSockets = 1;

//		SERVER RELATED
sf::TcpListener listener;

//		CLIENTS

bool stCommunication(clientStruct* pointer) {

	// temp test packet
	char packet[5] = { 'h','e','l','l','o' };

	if (pointer->socket.send(packet, 5)) {
		if (stDebug) {
			std::cout << "[ CRITICAL ] ST Debug: Could not communicate with client! \n";
		}
		return 1;
	}

	std::size_t received;

	if (pointer->socket.receive(packet, 5, received)) {
		if (stDebug) {
			std::cout << "[ CRITICAL ] ST Debug: Could not communicate with client! \n";
		}
		return 1;
	}

	return 0;
}

// a main thread of a socket
bool stSocketThread(mapContainer* map, clientStruct* pointer, unsigned short number, std::vector<std::string> names, datapackContainer* datapackPtr, unsigned short port) {

	sf::TcpListener socketListener;

	gameWindow.setActive(false);

	// binding the socket to port
	if (socketListener.listen(port + number + 1) != sf::Socket::Done) {
		if (stDebug) {
			std::cout << "[ CRITICAL ] ST Debug: Socket listener setup failed! \n";
		}
		return 1;
	}

	if (socketListener.accept(pointer->socket) == sf::Socket::Done) {
		if (stDebug) {
			std::cout << "ST Debug: Accepted socket connection... \n";
		}
	}
	else {
		if (stDebug) {
			std::cout << "ST Debug: Failed to connect socket! \n";
		}
		return 1;
	}

	// starting a connection with client

	// connection type

	std::size_t received;

	char auth[1];

	if (pointer->socket.receive(auth, 1, received)) {
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
		sf::sleep(sf::milliseconds(1));
	}

	if (stDebug) {
		std::cout << "ST Debug: Game frozen! \n";
	}

	// transfer map only to external clients
	if (auth[0]) {
		if (stDebug) {
			std::cout << "ST Debug: Starting map transfer... \n";
		}

		if (stDebug) {
			std::cout << "[ STARTING ] ST Debug: Starting map transfer from server side... \n";
		}


		sendMap(map, &pointer->socket, stDebug, names, datapackPtr);

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
				std::cout << "[ WARNING ] ST Debug: Could not communicate with client! \n";
			}
			break;
		}

	}

	// disconnection

	pointer->socket.disconnect();
	pointer->isUsed = false;

	if (stDebug) {
		std::cout << "ST Debug: Ended server thread! \n";
	}

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

void stListenerThread(mapContainer* map, std::vector<std::string> names, datapackContainer* datapackPtr, unsigned short port) {
	ushort socketToUse = NULL;
	sf::TcpSocket portSocket;
	char auth[1];

	gameWindow.setActive(false);

	while (*isAllAlright) {

		// check if max connection amount was not reached
		if (connectedSockets < maxSockets) {

			socketToUse = getSocketToUse();

			if (listener.accept(portSocket) == sf::Socket::Done) {
				if (stDebug) {
					std::cout << "ST Debug: Accepting connection at: " << socketToUse << "\n";
				}
				if (clientThreads[socketToUse].wasUsed) {
					clientThreads[socketToUse].wasUsed = false;
					clientThreads[socketToUse].thread.join();
				}
				// gives the client port number
				auth[0] = socketToUse;
				if (portSocket.send(auth, 1)) {
					if (stDebug) {
						std::cout << "[ CRITICAL ] ST Debug: Could not communicate with client! \n";
					}
				}
				else {
					clientThreads[socketToUse].thread = std::thread(stSocketThread, map, &clientThreads[socketToUse], socketToUse, names, datapackPtr, port);
					clientThreads[socketToUse].isUsed = true;
					clientThreads[socketToUse].wasUsed = true;
				}
				portSocket.disconnect();
			}

		}
		
		// 5 connection attempts per second, more than enough
		sf::sleep(sf::milliseconds(200));
	}

	for (unsigned short n = 0; n < clientCap; n++) {
		if (clientThreads[n].isUsed) {
			clientThreads[n].thread.join();
			if (stDebug) {
				std::cout << "ST Debug: Ended client thread " << n << "\n";
			}
		}
	}

}

//		MAIN

bool stPrepareBaseFunctions(unsigned short port, mapContainer* map, std::vector<std::string> names, datapackContainer* datapackPtr) {
	
	listener.setBlocking(false);

	
	// binding the socket to port
	if (listener.listen(port) != sf::Socket::Done) {
		if (stDebug) {
			std::cout << "[ CRITICAL ] ST Debug: Listener setup failed! \n";
		}
		stopListener = false;
		return 0;
	}
	
	masterThread = std::thread(stListenerThread, map, names, datapackPtr, port);

	return 1;
}

// main function thar runs the server in a seperate thread, send serverStatus as a true value bool
void serverFunction(mapContainer* map, std::vector<std::string> names,
	bool* serverStatusPtr, bool* isFrozenPtr, 
	bool* isRunningPtr, datapackContainer* datapackPtr, ushort clientAmount = 8,
	bool debug = true, unsigned short port = 21370) {

	stIsFrozen = isFrozenPtr;
	stIsRunning = isRunningPtr;
	maxSockets = clientAmount;
	clientCap = clientAmount;
	stDebug = debug;
	isAllAlright = serverStatusPtr;

	if (stDebug) {
		std::cout << "[ STARTING ] ST debug: Starting server thread... \n";
	}


	if (stPrepareBaseFunctions(port, map, names, datapackPtr)) {
		if (stDebug) {
			std::cout << "[ MILESTONE ] ST debug: Server functions prepared succesfully, starting the loop... \n";
		}

		while (*isAllAlright) {



		}
	}

	if (stopListener) {
		masterThread.join();
	}

	if (stDebug) {
		std::cout << "ST Debug: Ending server thread... \n";
	}
	
}

void startServer(std::thread* thread, mapContainer* map, std::vector<std::string> names,
	bool* serverStatusPtr, bool* isFrozenPtr,
	bool* isRunningPtr, datapackContainer* datapackPtr, ushort clientAmount = 8,
	bool debug = true, unsigned short port = 21370) {


	*thread = std::thread(serverFunction, map, names,
		serverStatusPtr, isFrozenPtr,
		isRunningPtr, datapackPtr, clientAmount,
		debug, port);
}
