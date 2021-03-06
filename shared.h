#pragma once
// contains elements required by everything without specific classes etc

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <queue>
#include <string>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <fstream>

//		MAP MAIN GRAPHICS

sf::RenderWindow gameWindow;

const unsigned short blockBaseSize = 24;


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

namespace fs = std::filesystem;



const std::string version = "0.0.3.1";



bool isLastWorldPresent = false;
std::string lastWorldPlayed;

//		NETWORK RELATED
bool isFrozen = false;
bool isCurrentlyRunning = false;
bool isWaitingForFreeze = false;
bool startGame = false;
bool clientStatus = true;
bool serverStatus = true;

//				IMPORTANT VARIABLES
unsigned short currentDimension = 0;

//				MOUSE BUTTONS
bool isMouseLeftClicked = false;
bool isMouseRightClicked = false;
bool isMouseMiddleClicked = false;

//		MOUSE CLICK VALUES
// whether main or secondary button was clicked, false - main, true - secondary
bool clickSwitch = false;
unsigned short clickNumber;
bool isButtonClicked = false;

//				THREADS
std::thread serverThread;



std::mutex threadLock;

//				GAMEPLAY

// gameplay variables
unsigned short timeStep = 20;
unsigned short timeCounter = 0;
unsigned short timeCount = 10;

//				NETWORKING

sf::IpAddress mainIp;
unsigned short mainPort = 21370;


const unsigned short maxClients = 8;


//				GRAPHICS

// render distance for all to use
sf::Vector2i mainRenderDistance = sf::Vector2i(0, 0);
sf::Vector2i shadeRenderDistance = sf::Vector2i(0, 0);
sf::Vector2i ChunkRenderDistance = sf::Vector2i(0, 0);
sf::Vector2i chunkShadeRenderDistance = sf::Vector2i(0, 0);


// hybrid renderer
bool isHybridRenderServiceActive = false;
bool isHybridRenderServiceReady = false;
bool canHybridRenderServiceStartWorking = false;
unsigned short chunkSize = 5;
// fixed amount calculated for 16:9 screens
const sf::Vector2i chunkScreenRatio = sf::Vector2i(16, 9);
// this includes shadows so don't set it too low
const unsigned short chunkOffset = 3;
const unsigned short chunkRenderOffset = 2;
const unsigned short chunkAmount = (chunkScreenRatio.x + (2 * chunkOffset)) * (chunkScreenRatio.y + (2 * chunkOffset));
const sf::Vector2i chunkRenderDistance = sf::Vector2i((chunkScreenRatio.x / 2) + chunkRenderOffset, (chunkScreenRatio.y / 2) + chunkRenderOffset);
const sf::Vector2i chunkRenderDistanceConverted = sf::Vector2i(chunkRenderDistance.x * chunkSize, chunkRenderDistance.y * chunkSize);

// block shape
sf::Shader blockShapeCenterShader;
sf::Shader blockShapeCornerShader;
sf::RenderTexture blockShapeTexture;
sf::Sprite blockShapeSprite;



//		MAP GLOBAL SHADING GRAPHICS

//		MAP LOCAL SHADING GRAPHICS

//		USER INTERFACE GRAPHICS

//		MISC

sf::Vector2i gameRes = sf::Vector2i(1920, 1080);

unsigned short stage = 0;
unsigned short subStage = 0;
unsigned short framerate = 120;
unsigned short tickRate = 240;

bool gThreadSetLoadString = false;
std::string gThreadStringToSet;
std::string mapGeneratorString = "Starting map generation...";




// from render2

// defines horizontal to vertical ratio of the global shadows
const unsigned short angleMultiplier = 6;

sf::Vector2f angleHelperValue = sf::Vector2f(1.0 / float(gameRes.x + (2 * 25 * angleMultiplier)), 1.0 / float(gameRes.y + (2 * 25 * angleMultiplier)));


sf::RenderTexture globalShadowWindow, localShadowWindow;

sf::RenderTexture mapMainTexture;
sf::View mapMainView, shadeView;
sf::Sprite mapMainSprite;
sf::Sprite mapShadeSprite;
sf::Shader globalShader;
sf::RectangleShape shaderShape;

#define PI 3.141592

// maps for replacing loaded definitions or textures by other datapacks to allow for non invasive datapack addons
std::unordered_map<std::string, std::string> textureReplaceMap;
std::unordered_map<std::string, std::string> definitionReplaceMap;

void debugMsg(std::string msg) {
	if (debug) {
		std::cout << msg << "\n";
	}
}

