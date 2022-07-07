#pragma once

// header file with classes and structs used by the game

#include <thread>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "quickWrite.h"

//				CLASSES

//		NETWORK

//	OLD
// used to keep a client thread by a server
class client {
public:
	bool isActive = false;
	sf::TcpSocket socket;
	std::thread thread;

	// temp!
	unsigned short packetsSent = 0;

};




//				STRUCTS

//		AUXILIARY

struct renderLimit {
	sf::Vector2i lower = sf::Vector2i(0, 0);
	sf::Vector2i upper = sf::Vector2i(0, 0);
};

//		DATAPACKS

// used to keep information about a datapack for user and game
struct datapackInfo {
	std::string name = "N/A";
	std::string version = "N/A";
	std::string desc = "N/A";
	bool used = false;
};