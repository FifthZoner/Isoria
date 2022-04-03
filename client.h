#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <thread>
#include <iostream>

#include "quickWrite.h"
#include "map.h"
#include "mapReceive.h"

// a client side file
struct clientClass;
void around(clientClass* ptr, mapContainer* map, bool external, bool* ctClientStatus, sf::IpAddress ip, unsigned short port, datapackContainer* datapackPtr);

// a class responsible for a single client
struct clientClass {
	sf::TcpSocket socket;
	bool ctDebug = 1;
	std::thread thread;
	std::size_t received;

	// TEMP
	bool firstText = true;

	bool ctCommunication() {
		

		// temp test packet
		char packet[5];
		
		if (socket.receive(packet, 5, received)) {
			if (ctDebug) {
				std::cout << "[ CRITICAL ] CT Debug: Could not communicate with server! \n";
			}
			return 1;
		}

		// message to confirm correct connection with the server, migt add some other une during the initial contact to ensure no data loss
		if (firstText and ctDebug) {
			std::cout << "CT Debug: Greeting received: " << packet[0] << packet[1] << packet[2] << packet[3] << packet[4] << "\n";
		}
		firstText = false;

		if (socket.send(packet, 5)) {
			if (ctDebug) {
				std::cout << "[ CRITICAL ] CT Debug: Could not communicate with server! \n";
			}
			return 1;
		}

		return 0;
	}

	bool mainFunction(bool external, mapContainer* map, bool* ctClientStatus, sf::IpAddress ip, unsigned short port, datapackContainer* datapackPtr) {

		sf::Socket::Status status = socket.connect("192.168.0.245", 21370);


		if (status != sf::Socket::Done)
		{
			if (ctDebug) {
				std::cout << "[ CRITICAL ] CT Debug: Could not connect to number server! \n";
			}
			return 1;
		}

		// connection start
		char auth[1] = { 100 };

		// auth now gives connection number at first

		if (socket.receive(auth, 1, received)) {
			if (ctDebug) {
				std::cout << "[ CRITICAL ] CT Debug: Could not get port number! \n";
			}
			return 1;
		}

		socket.disconnect();
		int temp = auth[0];
		std::cout << "CT Debug: Port addition number: " << temp << "\n";
		status = socket.connect("192.168.0.245", (21370 + auth[0] + 1));
		if (status != sf::Socket::Done) {
			if (ctDebug) {
				std::cout << "[ CRITICAL ] CT Debug: Could not connect to server! \n";
			}
			return 1;
		}
		if (ctDebug) {
			std::cout << "CT Debug: Connected to server! \n";
		}

		auth[0] = external;

		// and here is proper check
		if (socket.send(auth, 1)) {
			if (ctDebug) {
				std::cout << "[ CRITICAL ] CT Debug: Could not authorize connection with server! \n";
			}
			return 1;
		}
		
		// map receiving function
		if (ctDebug) {
			std::cout << "[ STARTING ] CT Debug: Starting map transfer from client side... \n";
		}
		receiveMap(map, &socket, datapackPtr, ctDebug);




		if (ctDebug) {
			std::cout << "[ STARTING ] CT Debug: Connected to server! \n";
		}

		while (*ctClientStatus) {

			// ends loop if communication was not succesful, might change to catching up in the future
			if (ctCommunication()) {
				*ctClientStatus = false;
				break;
			}

		}


		if (ctDebug) {
			std::cout << "CT Debug: Disconnecting client... \n";
		}

		socket.disconnect();

		return 0;
	}

	// start in a new theread with bool coresponding to client status with value of true
	void start(clientClass* ptr, mapContainer* map, sf::IpAddress ip, bool* ctClientStatus, datapackContainer* datapackPtr, bool ctDebugValue = 1, const unsigned short port = 21370, bool external = 0) {
		ctDebug = ctDebugValue;

		if (ctDebug) {
			std::cout << "[ STARTING ] CT Debug: Connecting to: " << ip << " " << port << "\n";
		}
		

		thread = std::thread(around, ptr, map, external, ctClientStatus, ip, port, datapackPtr);

		
	}
};

void around(clientClass* ptr, mapContainer* map, bool external, bool* ctClientStatus, sf::IpAddress ip, unsigned short port, datapackContainer* datapackPtr) {
	ptr->mainFunction(external, map, ctClientStatus, ip, port, datapackPtr);
}