#pragma once

#include "misc.h"
#include "buttons.h"
#include "graphics.h"
#include "convert.h"
#include "map.h"
#include "data.h"
#include "mapGenerator.h"
#include "loadMap.h"
#include "loadDatapacks.h"
#include "server.h"
#include "launcher.h"
#include "renderer2.h"
#include "declarations.h" 

// moved declarations here to have then avalaible everywhere

namespace fs = std::filesystem;



const std::string version = "0.0.2.2";





//				BOOLS
bool sfDebug = false;
bool showVersion = true;
bool letItBe = true;

bool terminateServer = false;
bool terminateClient = false;


bool isLastWorldPresent = false;
str lastWorldPlayed;

//		NETWORK RELATED
bool isFrozen = false;
bool isCurrentlyRunning = false;
bool isWaitingForFreeze = false;
bool startGame = false;
bool clientStatus = true;
bool serverStatus = true;

//				IMPORTANT VARIABLES
ushort currentDimension = 0;

//				MOUSE BUTTONS
bool isMouseLeftClicked = false;
bool isMouseRightClicked = false;
bool isMouseMiddleClicked = false;

//		MOUSE CLICK VALUES
// whether main or secondary button was clicked, false - main, true - secondary
bool clickSwitch = false;
ushort clickNumber;
bool isButtonClicked = false;

//				THREADS
std::thread serverThread;

clientClass client;

std::mutex threadLock;

//				GAMEPLAY

// gameplay variables
unsigned short timeStep = 20;
unsigned short timeCounter = 0;
unsigned short timeCount = 10;

//				NETWORKING

sf::IpAddress mainIp;
unsigned short mainPort = 21370;


const ushort maxClients = 8;


//				GRAPHICS

//		MAP MAIN GRAPHICS

sf::RenderWindow gameWindow;

//		MAP GLOBAL SHADING GRAPHICS

//		MAP LOCAL SHADING GRAPHICS

//		USER INTERFACE GRAPHICS

//		MISC

sf::Vector2i gameRes = sf::Vector2i(1920, 1080);

ushort stage = 0;
ushort subStage = 0;
ushort framerate = 60;
ushort tickRate = 240;

bool gThreadSetLoadString = false;
str gThreadStringToSet;
str mapGeneratorString = "Starting map generation...";

//		GRAPHICAL ELEMENTS



sf::Font mainFont;

centeredTextbox lsText;
centeredTextbox versionTextbox;

//		MAIN MENU BUTTONS
buttonTextOnly mmContinue, mmNewWorld, mmLoadGame, mmSettings, mmExit, mmJoin;
buttonTextOnly mmsBack;
buttonTextOnly mmlgBack;
buttonTextOnly mmnwBack, mmnwStart;

spriteBackgroundChangeable lsBackground;
spriteBackgroundChangeable mmBackground;

//				FILE STRUCTURE

std::vector <datapackInfo> datapacksPresent;
std::vector <str> datapacks;
std::vector <ushort> datapackIds;

//				MAP

// two added for early tests of map download

mapContainer gameMap;

//				DATAPACKS

datapackContainer mDatapacks;
lDatapackPathsContainer mDatapackPathsContainer;



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