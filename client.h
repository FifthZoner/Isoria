#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <thread>

#include "quickWrite.h"

// a client side file

// a class responsible for a single client
struct clientClass {
	sf::TcpSocket socket;
	bool ctDebug = 1;
	std::thread thread;

	// TEMP
	bool firstText = true;

	bool ctCommunication() {
		std::size_t received;

		// temp test packet
		char packet[5];
		
		if (!socket.receive(packet, 5, received)) {
			if (ctDebug) {
				"[ CRITICAL ] CT Debug: Could not communicate with server! \n";
			}
			return 1;
		}

		// message to confirm correct connection with the server, migt add some other une during the initial contact to ensure no data loss
		if (firstText and ctDebug) {
			std::cout << "CT Debug: Greeting received: " << packet[0] << packet[1] << packet[2] << packet[3] << packet[4] << "\n";
		}

		if (!socket.send(packet, 5)) {
			if (ctDebug) {
				"[ CRITICAL ] CT Debug: Could not communicate with server! \n";
			}
			return 1;
		}

		return 0;
	}

	// start in a new theread with bool coresponding to client status with value of true
	bool start(sf::IpAddress ip, bool* ctClientStatus, bool ctDebugValue = 1, const unsigned short port = 21370) {
		ctDebug = ctDebugValue;

		sf::Socket::Status status = socket.connect(ip, port);
		if (status != sf::Socket::Done)
		{
			return 1;
		}

		// connection start
		char auth[1];

		if (!socket.send(auth, 1)) {
			if (ctDebug) {
				"[ CRITICAL ] CT Debug: Could not authorize connection with server! \n";
			}
			return 1;
		}

		if (ctDebug) {
			"[ STARTING ] CT Debug: Connected to server! \n";
		}

		while (*ctClientStatus) {

			// ends loop if communication was not succesful, might change to catching up in the future
			if (ctCommunication()) {
				*ctClientStatus = false;
				break;
			}
			
		}

		socket.disconnect();

		return 0;
	}
};