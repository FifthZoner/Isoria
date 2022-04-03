#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <filesystem>

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
bool clientStatus = 1;
bool serverStatus = 1;

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

//				NETWORKING

sf::IpAddress mainIp;
unsigned short mainPort = 21370;


const ushort maxClients = 8;


//				GRAPHICS

//		MAP MAIN GRAPHICS

sf::RenderTexture mapMainTexture;
sf::View mapMainView;
sf::Sprite mapMainSprite;

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

sf::RenderWindow gameWindow;
sf::RenderTexture globalShadowWindow, localShadowWindow;

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
mapContainer testMap;

//				DATAPACKS

datapackContainer mDatapacks;
lDatapackPathsContainer mDatapackPathsContainer;

// gives a vector containing names of loaded datapacks
std::vector<std::string> getDatapackNames(std::vector<std::string> names = {}) {

	for (unsigned short n = 0; n < mDatapacks.datapacks.size(); n++) {
		names.push_back(mDatapacks.datapacks[n].name);
	}

	if (sfDebug) {
		std::cout << "SF Debug: Scrambled datapack names: \n";
		for (unsigned short n = 0; n < names.size(); n++) {
			std::cout << "SF Debug: " << names[n] << "\n";
		}
	}

	return names;
}

//				LOADING PROCESS

//		UNIVERSAL LOADING FUNCTIONS

// sets the crash-genic loading screen textbox string in a safe way
void setLoadString(str string) {
	threadLock.lock();
	gThreadStringToSet = string;
	gThreadSetLoadString = true;
	threadLock.unlock();
}

// sets current stage safely
void setStage(ushort setStage, ushort setSubStage = 0) {
	threadLock.lock();
	stage = setStage;
	subStage = setSubStage;
	threadLock.unlock();
}

//		PRELOADING

// function to load settings required for loading screen operation
void loadSettings(){

	// loads paths and uses essential ones from pathSettings.txt
	std::ifstream file;
	file.open("pathSettings.txt");

	for (std::string readString; file >> readString;) {
		
		if (readString == "font") {

			// two to skip 1 line
			std::getline(file, readString);
			std::getline(file, readString);
			if (mainFont.loadFromFile(readString)) {
				if (sfDebug) {
					std::cout << "SF debug: Loaded main font style from " << readString << "\n";
				}
			}
			else {
				letItBe = false;
				if (sfDebug) {
					std::cout << "SF debug: Failed to load main font! Exiting..." << "\n";
				}
			}
		}
	}

	file.close();
}

// function to load loading screen elements
void loadLoadingScreen() {

	lsBackground.createFromFolder("gfx/loadingScreen/backgrounds/", gameRes);
	if (sfDebug) {
		std::cout << "SF debug: Loaded and prepared loading screen backgrounds" << "\n";
	}

	lsText.create(&mainFont, sf::Vector2f(gameRes.x / 2, gameRes.y * 0.85), "Loading main menu assets");
	versionTextbox.create(&mainFont, sf::Vector2f(64, 12), "Isoria " + version, 15);
}

// function to load all early elements required for starting the game before loading screen and stages
void preLoading() {

	loadSettings();

	loadLoadingScreen();
}

//		PROPER LOADING

// function to load contents of main menu new world substage 1
void loadMainMenuNewWorld() {
	mmnwBack.create(sf::Vector2f(gameRes.x - 80, gameRes.y - 80), "BACK", &mainFont, 40);
	mmnwStart.create(sf::Vector2f(gameRes.x - 80, gameRes.y - 160), "START", &mainFont, 40);
}

// function to load contents of main menu load game substage 2
void loadMainMenuLoadGame() {
	mmlgBack.create(sf::Vector2f(gameRes.x - 80, gameRes.y - 80), "BACK", &mainFont, 40);
}

// function to load contents of main menu settings substage 3
void loadMainMenuSettings() {
	mmsBack.create(sf::Vector2f(gameRes.x - 80, gameRes.y - 80), "BACK", &mainFont, 40);
}

// checks last played world
void loadMainMenuCheckLast() {
	std::ifstream file;

	file.open("lastPlayed.txt");

	if (file.is_open()) {
		
		if (file >> lastWorldPlayed) {
			isLastWorldPresent = true;

			if (sfDebug) {
				std::cout << "SF debug: Loaded last played world: " << lastWorldPlayed << "\n";
			}
		}

		file.close();
	}
	
}

// function to load contents of main menu substage 0 and start others
void loadMainMenu() {
	mmBackground.createFromFolder("gfx/mainMenu/backgrounds/", gameRes);

	// check if continue is present
	loadMainMenuCheckLast();

	// buttons
	mmJoin.create(sf::Vector2f(gameRes.x - 80, gameRes.y * 0.3 - 80), "JOIN", &mainFont, 40);
	mmContinue.create(sf::Vector2f(gameRes.x - 80, gameRes.y * 0.3), "CONTINUE", &mainFont, 40);
	mmNewWorld.create(sf::Vector2f(gameRes.x - 80, gameRes.y * 0.3 + 80), "NEW WORLD", &mainFont, 40);
	mmLoadGame.create(sf::Vector2f(gameRes.x - 80, gameRes.y * 0.3 + 160), "LOAD GAME", &mainFont, 40);
	mmSettings.create(sf::Vector2f(gameRes.x - 80, gameRes.y * 0.3 + 240), "SETTINGS", &mainFont, 40);
	mmExit.create(sf::Vector2f(gameRes.x - 80, gameRes.y * 0.3 + 320), "EXIT", &mainFont, 40);

	// other loadings
	loadMainMenuNewWorld();
	loadMainMenuLoadGame();
	loadMainMenuSettings();
}

// loads additional data, for example about network
void loadAdditional() {
	// loads server info from file

	std::ifstream file;
	// file in form
	// <Byte 1> <Byte 2> <Byte 3> <Byte 4>
	// <port number>

	file.open("serverData.txt");

	// ip adress bytes in that form per sfml wiki recomendation
	char B1, B2, B3, B4;
	file >> B1 >> B2 >> B3 >> B4;
	mainIp = sf::IpAddress(B1, B2, B3, B4);

	file >> mainPort;
	file.close();
}

// function to load in game interface elements
void loadInterface() {

}

// function to load primary graphics
void loadPrimaryGraphics() {
	if (sfDebug) {
		std::cout << "SF debug: Preparing graphics textures... \n";
	}

	mapMainTexture.create(gameRes.x, gameRes.y);
	mapMainSprite.setPosition(vec2f(0, 0));
}

// collective function to load most of game's data
void loadData() {

	loadMainMenu();

	setLoadString("Loading datapack definitions...");

	preLoadMainGameDatapacks(&mDatapacks, &mDatapackPathsContainer);

	setLoadString("Loading datapack graphics...");

	loadMainGameDatapacks(&mDatapacks, &mDatapackPathsContainer);

	// delete paths to save space
	paths->datapacks.clear();

	setLoadString("Loading primary graphics...");

	loadPrimaryGraphics();

	setLoadString("Loading interface...");

	loadInterface();

	setLoadString("Finishing up...");

	loadAdditional();
}

//				MAIN GRAPHICS

//		SECONDARY FUNCTIONS

renderLimit getRenderLimit(dimension* pointer) {
	renderLimit value;

	// temp!
	value.lower = vec2i(0, 0);
	value.upper = pointer->size;

	return value;
}

// temp!
void mainRender(dimension* pointer, renderLimit limit) {
	mapMainTexture.clear();

	// right now not ready for textures tahat overlap other blocks
	for (uint y = limit.lower.y; y < limit.upper.y; y++) {
		for (uint x = limit.lower.x; x < limit.upper.x; x++) {

			// back
			if (pointer->renderGrid.grid[y][x].background) {
				mapMainTexture.draw(pointer->backgrounds.blocks[y][x].sprite);
			}

			// floor
			if (pointer->renderGrid.grid[y][x].floor) {
				mapMainTexture.draw(pointer->floors.blocks[y][x].sprite);
			}

			// wall
			if (pointer->renderGrid.grid[y][x].wall) {
				mapMainTexture.draw(pointer->walls.blocks[y][x].sprite);
			}

		}
	}

	mapMainTexture.display();
}

void render2x0(dimension* pointer) {

	// will be split into threads for simultaneus shading and other things

	renderLimit limit = getRenderLimit(pointer);

	mainRender(pointer, limit);

	mapMainSprite.setTexture(mapMainTexture.getTexture());

	gameWindow.draw(mapMainSprite);
}

//		MAIN FUNCTIONS

// renders loading screen graphics
void render0LoadingScreen() {
	lsBackground.draw(&gameWindow);

	if (gThreadSetLoadString) {
		lsText.setString(gThreadStringToSet);
		threadLock.lock();
		gThreadSetLoadString = false;
		threadLock.unlock();
	}

	lsText.draw(&gameWindow);
	versionTextbox.draw(&gameWindow);

}

// renders main menu graphics
void render1MainMenu() {
	mmBackground.draw(&gameWindow);

	switch (subStage) {
	case 0:
		// drawing menu buttons
		if (isLastWorldPresent) {
			mmContinue.draw(&gameWindow);
		}
		mmJoin.draw(&gameWindow);
		mmNewWorld.draw(&gameWindow);
		mmLoadGame.draw(&gameWindow);
		mmSettings.draw(&gameWindow);
		mmExit.draw(&gameWindow);

		break;

	case 1:
		mmnwBack.draw(&gameWindow);
		mmnwStart.draw(&gameWindow);
		break;

	case 2:
		mmlgBack.draw(&gameWindow);
		break;

	case 3:
		mmsBack.draw(&gameWindow);
		break;

	default:
		if (sfDebug) {
			std::cout << "SF debug: Out of subStage range in render1 loop! Exiting..." << "\n";
		}
		letItBe = false;
		break;
	}

	if (showVersion) {
		versionTextbox.draw(&gameWindow);
	}

	
}

// renders gameplay
void render2Gameplay() {
	switch (subStage) {

	case 0:
		render2x0(&gameMap.dimensions[currentDimension]);
		break;

	default:
		if (sfDebug) {
			std::cout << "[ CRITICAL ] SF debug: Gameplay graphics loop out of range! Exiting..." << "\n";
		}
		letItBe = false;
		break;
	}
}

// renders all graphics
void graphicsRenderer() {

	// creates window
	gameWindow.setActive();
	gameWindow.create(sf::VideoMode(gameRes.x, gameRes.y, 32), "Isoria", sf::Style::Fullscreen);

	// main loop
	sf::Clock gameClock;
	double frameTime = 1000000.0 / framerate;


	while (gameWindow.isOpen() and letItBe) {

		if (gameClock.getElapsedTime().asMicroseconds() > frameTime) {
			gameClock.restart();

			for (sf::Event event; gameWindow.pollEvent(event);) {

				if (event.type == sf::Event::Closed) {
					gameWindow.close();
					serverStatus = false;
					clientStatus = false;
					letItBe = false;
				}
			}

			gameWindow.clear();

			switch (stage) {
				
			case 0:
				render0LoadingScreen();
				break;

			case 1:
				render1MainMenu();
				break;

			case 2:
				render2Gameplay();
				break;

			default:
				if (sfDebug) {
					std::cout << "SF debug: Render stage out of range! Exiting..." << "\n";
					letItBe = false;
				}
				break;
			}

			gameWindow.display();
		}
	}

	gameWindow.close();
}

//				MAIN NON GRAPHICAL LOOP

//		STAGES

//	MAIN MENU MISC

void clickedContinue() {
	// with info about local connection

	if (sfDebug) {
		std::cout << "SF debug: Starting map loading process... \n";
	}

	loadMap(&gameMap, lastWorldPlayed, &mDatapacks, sfDebug);

	if (sfDebug) {
		std::cout << "SF debug: Loading auxiliary data... \n";
	}

	// anything else needed here
	if (sfDebug) {
		std::cout << "[ STARTING ] SF debug: Starting game server... \n";
	}

	startServer(&serverThread, &gameMap, getDatapackNames(), &serverStatus, &isFrozen, &isCurrentlyRunning, 8, sfDebug, mainPort);

	if (sfDebug) {
		std::cout << "[ STARTING ] SF debug: Starting client... \n";
	}

	// CHANGE TO LOCAL AGAIN
	client.start(&client, &testMap, mainIp, &clientStatus, &mDatapacks, sfDebug, mainPort, 1);


	// setting stage

	setStage(2);

	if (sfDebug) {
		std::cout << "[ DONE ] SF debug: Gameplay is here! \n";
	}
}

void clickedJoin() {
	// add external connection info

	if (sfDebug) {
		std::cout << "[ STARTING ] SF debug: Starting client... \n";
	}

	client.start(&client, &gameMap, mainIp, &clientStatus, &mDatapacks, sfDebug, mainPort, 1);
}

//	MAIN MENU MAIN

// function to run processes for stage 1 main menu subStage 0
void run1x0Menu() {
	// click detection
	if (!isMouseLeftClicked and sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		isMouseLeftClicked = true;
		if (sfDebug) {
			std::cout << "SF debug: Mouse left click started" << "\n";
		}

		// checking each button
		if (mmContinue.checkClickStart(sf::Mouse::getPosition(gameWindow)) and isLastWorldPresent) {
			if (sfDebug) {
				std::cout << "SF debug: Mouse click started on main menu continue button" << "\n";
			}
			isButtonClicked = true;
			clickSwitch = false;
			clickNumber = 0;

			// load last game, to gray out or not draw when not present
		}
		else if (mmNewWorld.checkClickStart(sf::Mouse::getPosition(gameWindow))) {
			if (sfDebug) {
				std::cout << "SF debug: Mouse click started on main menu new world button" << "\n";
			}
			isButtonClicked = true;
			clickSwitch = false;
			clickNumber = 1;
			setStage(1, 1);
		}
		else if (mmLoadGame.checkClickStart(sf::Mouse::getPosition(gameWindow))) {
			if (sfDebug) {
				std::cout << "SF debug: Mouse click started on main menu load game button" << "\n";
			}
			isButtonClicked = true;
			clickSwitch = false;
			clickNumber = 2;
			setStage(1, 2);
		}
		else if (mmSettings.checkClickStart(sf::Mouse::getPosition(gameWindow))) {
			if (sfDebug) {
				std::cout << "SF debug: Mouse click started on main menu settings button" << "\n";
			}
			isButtonClicked = true;
			clickSwitch = false;
			clickNumber = 3;
			setStage(1,3);
		}
		else if (mmExit.checkClickStart(sf::Mouse::getPosition(gameWindow))) {
			if (sfDebug) {
				std::cout << "SF debug: Mouse click started on main menu exit button" << "\n";
			}
			isButtonClicked = true;
			clickSwitch = false;
			clickNumber = 4;
		}
		else if (mmJoin.checkClickStart(sf::Mouse::getPosition(gameWindow))) {
			if (sfDebug) {
				std::cout << "SF debug: Mouse click started on main menu join button" << "\n";
			}
			isButtonClicked = true;
			clickSwitch = false;
			clickNumber = 5;
		}
	}
	else if (isMouseLeftClicked and !sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		isMouseLeftClicked = false;
		if (sfDebug) {
			std::cout << "SF debug: Mouse left click ended" << "\n";
		}
		if (isButtonClicked) {
			isButtonClicked = false;

			switch (clickNumber) {
			case 0:
				if (mmContinue.checkClickEnd(sf::Mouse::getPosition(gameWindow))) {
					if (sfDebug) {
						std::cout << "SF debug: Main menu continue button clicked" << "\n";
					}
					clickedContinue();
				}
				break;

			case 1:
				if (mmNewWorld.checkClickEnd(sf::Mouse::getPosition(gameWindow))) {
					if (sfDebug) {
						std::cout << "SF debug: Main menu new world button clicked" << "\n";
					}
				}
				break;

			case 2:
				if (mmLoadGame.checkClickEnd(sf::Mouse::getPosition(gameWindow))) {
					if (sfDebug) {
						std::cout << "SF debug: Main menu load game button clicked" << "\n";
					}
				}
				break;

			case 3:
				if (mmSettings.checkClickEnd(sf::Mouse::getPosition(gameWindow))) {
					if (sfDebug) {
						std::cout << "SF debug: Main menu settings button clicked" << "\n";
					}
				}
				break;

			case 4:
				if (mmExit.checkClickEnd(sf::Mouse::getPosition(gameWindow))) {
					if (sfDebug) {
						std::cout << "SF debug: Main menu exit button clicked, exiting..." << "\n";
					}
					letItBe = false;
				}
				break;
			case 5:
				if (mmJoin.checkClickEnd(sf::Mouse::getPosition(gameWindow))) {
					if (sfDebug) {
						std::cout << "SF debug: Main menu join button clicked, joining external session..." << "\n";
					}
					clickedJoin();
				}
				break;
			}
		}
	}
}

// function to run processes for stage 1 main menu substage 1 new world
void run1x1NewWorld() {
	// click detection
	if (!isMouseLeftClicked and sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		isMouseLeftClicked = true;
		if (sfDebug) {
			std::cout << "SF debug: Mouse left click started" << "\n";
		}

		// checking each button
		if (mmnwBack.checkClickStart(sf::Mouse::getPosition(gameWindow))) {
			if (sfDebug) {
				std::cout << "SF debug: Mouse click started on main new world back" << "\n";
			}
			isButtonClicked = true;
			clickSwitch = false;
			clickNumber = 0;
		
		}
		else if (mmnwStart.checkClickStart(sf::Mouse::getPosition(gameWindow))) {
			if (sfDebug) {
				std::cout << "SF debug: Mouse click started on main new world start" << "\n";
			}
			isButtonClicked = true;
			clickSwitch = false;
			clickNumber = 1;
		}
	}
	else if (isMouseLeftClicked and !sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		isMouseLeftClicked = false;
		if (sfDebug) {
			std::cout << "SF debug: Mouse left click ended" << "\n";
		}
		if (isButtonClicked) {
			isButtonClicked = false;
			if (!clickSwitch) {
				switch (clickNumber) {
				case 0:
					if (mmnwBack.checkClickEnd(sf::Mouse::getPosition(gameWindow))) {
						if (sfDebug) {
							std::cout << "SF debug: Main menu new world back button clicked" << "\n";
						}
						setStage(1);
					}
					break;
				case 1:
					if (mmnwStart.checkClickEnd(sf::Mouse::getPosition(gameWindow))) {
						if (sfDebug) {
							std::cout << "SF debug: Main menu new world start button clicked" << "\n";
						}
						generateMap("test", &mapGeneratorString);
					}
					break;
				}
				
			}
		}
	}
}

// function to run processes for stage 1 main menu substage 2 load game
void run1x2LoadGame() {
	// click detection
	if (!isMouseLeftClicked and sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		isMouseLeftClicked = true;
		if (sfDebug) {
			std::cout << "SF debug: Mouse left click started" << "\n";
		}

		// checking each button
		if (mmlgBack.checkClickStart(sf::Mouse::getPosition(gameWindow))) {
			if (sfDebug) {
				std::cout << "SF debug: Mouse click started on main load game back" << "\n";
			}
			isButtonClicked = true;
			clickSwitch = false;

		}
	}
	else if (isMouseLeftClicked and !sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		isMouseLeftClicked = false;
		if (sfDebug) {
			std::cout << "SF debug: Mouse left click ended" << "\n";
		}
		if (isButtonClicked) {
			isButtonClicked = false;
			if (!clickSwitch) {
				if (mmlgBack.checkClickEnd(sf::Mouse::getPosition(gameWindow))) {
					if (sfDebug) {
						std::cout << "SF debug: Main menu load game back button clicked" << "\n";
					}
					setStage(1);
				}
			}
		}
	}
}

// function to run processes for stage 1 main menu substage 3 settings
void run1x3Settings() {
	// click detection
	if (!isMouseLeftClicked and sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		isMouseLeftClicked = true;
		if (sfDebug) {
			std::cout << "SF debug: Mouse left click started" << "\n";
		}

		// checking each button
		if (mmsBack.checkClickStart(sf::Mouse::getPosition(gameWindow))) {
			if (sfDebug) {
				std::cout << "SF debug: Mouse click started on main settings back" << "\n";
			}
			isButtonClicked = true;
			clickSwitch = false;

		}
	}
	else if (isMouseLeftClicked and !sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		isMouseLeftClicked = false;
		if (sfDebug) {
			std::cout << "SF debug: Mouse left click ended" << "\n";
		}
		if (isButtonClicked) {
			isButtonClicked = false;
			if (!clickSwitch) {
				if (mmsBack.checkClickEnd(sf::Mouse::getPosition(gameWindow))) {
					if (sfDebug) {
						std::cout << "SF debug: Main menu settings back button clicked" << "\n";
					}
					setStage(1);
				}
			}
		}
	}
}

// function to run internal processes for stage 1 - main menu
void loopStage1() {
	switch (subStage) {

	case 0:
		run1x0Menu();
		break;

	case 1:
		run1x1NewWorld();
		break;

	case 2:
		run1x2LoadGame();
		break;

	case 3:
		run1x3Settings();
		break;

	default:
		if (sfDebug) {
			std::cout << "SF debug: Main menu subStage out of range! Exiting..." << "\n";
		}
		letItBe = false;
	}
}

//	GAMEPLAY

// main gameplay
void run2x0() {

}

// function to run internal processes for stage 2 - gameplay (mostly predictions, server takes care of the real logic)
void loopStage2() {
	switch (subStage) {

	case 0:
		run2x0();
		break;

	default:
		if (sfDebug) {
			std::cout << "[ CRITICAL ] SF debug: Gameplay subStage out of range! Exiting..." << "\n";
		}
		letItBe = false;
	}
}

//		MAIN FUNCTION

// main loop of the game containing non graphical interactions
void nonGraphicLoop() {

	sf::Clock tickClock;

	float tickTime = 1000000 / tickRate;

	while (letItBe) {
		if (tickClock.getElapsedTime().asMicroseconds() > tickTime and !isFrozen) {
			tickClock.restart();
			isCurrentlyRunning = true;

			// stages

			switch (stage) {

			case 1:
				loopStage1();
				break;

			case 2:
				loopStage2();
				break;

			default:
				if (sfDebug) {
					std::cout << "SF debug: Main loop stage out of range! Exiting..." << "\n";
				}
				letItBe = false;
			}

			isCurrentlyRunning = false;
		}
	}
}

//				MAIN

int main() {

	launcher(&datapacksPresent, &letItBe, &isMouseLeftClicked, &gameRes, &sfDebug);

	if (!letItBe) {

		if (sfDebug) {
			std::cout << "[ EXITING ] SF debug: Goodbye!" << "\n";
		}

		return 0;
	}

	preLoading();

	if (!letItBe) {

		if (sfDebug) {
			std::cout << "[ EXITING ] SF debug: Goodbye!" << "\n";
		}

		return 0;
	}

	// start graphics thread
	std::thread graphicsThread(graphicsRenderer);

	loadData();


	if (sfDebug) {
		std::cout << "SF debug: Game preparing success! Entering main menu..." << "\n";
	}

	// changing stage to main menu
	setStage(1);

	if (sfDebug) {
		std::cout << "SF debug: Starting main game loop" << "\n";
	}

	if (sfDebug) {
		std::cout << "\n";
		std::cout << "[ INFO ] SF debug: Welcome to Isoria version " << version << "\n \n";

		std::cout << "[ INFO ] SF debug: Debug prefixes:       \n";
		std::cout << "[ INFO ] SF debug: SF - Source File      \n";
		std::cout << "[ INFO ] SF debug: DL - Datapack Loading \n";
		std::cout << "[ INFO ] SF debug: ST - Server Thread    \n";
		std::cout << "[ INFO ] SF debug: CT - Client Thread    \n";
		std::cout << "[ INFO ] SF debug: ML - Map Loading      \n";
		std::cout << "[ INFO ] SF debug: LT - Launcher	       \n";
		std::cout << "[ INFO ] SF debug: MS - Map Send         \n";
		std::cout << "[ INFO ] SF debug: MR - Map Receive      \n";
		std::cout << "[ INFO ] SF debug: MG - Map Generation \n\n";
	}

	// starts the main game loop
	nonGraphicLoop();

	graphicsThread.join();

	// other threads
	if (terminateServer) {
		serverThread.join();
	}

	if (terminateClient) {
		client.thread.join();
	}

	if (sfDebug) {
		std::cout << "[ EXITING ] SF debug: Goodbye!" << "\n";
	}

	return 0;
}