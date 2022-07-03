#pragma once
// contains elements required by everything without specific classes etc

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <queue>
#include <string>
#include <iostream>

//		MAP MAIN GRAPHICS

sf::RenderWindow gameWindow;

//				BOOLS
bool sfDebug = false;
bool debug = false;
bool showVersion = true;
bool letItBe = true;
bool isInFocus = true;
bool isSingleplayer = true;

bool terminateServer = false;
bool terminateClient = false;

//		KEYBOARD

// queue containing keys pressed in the given frame
std::queue<char> keysPressedQueue;

bool isKeyboardActive = false;
bool isKeyboardInTypeMode = false;

std::string keyboardInputString = "";


void debugMsg(std::string msg) {
	if (debug) {
		std::cout << msg << "\n";
	}
}