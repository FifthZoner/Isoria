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
#include "renderer0.h"
#include "renderer2.h"
#include "declarations.h"




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
	unsigned short B1, B2, B3, B4;
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


	// render textures
	globalShadowWindow.create(gameRes.x + (2 * 25 * angleMultiplier), gameRes.y + (2 * 25 * angleMultiplier));
	mapMainTexture.create(gameRes.x, gameRes.y);
	mapMainSprite.setPosition(vec2f(0, 0));

	// shaders
	if (globalShader.loadFromFile("globalShader.frag", sf::Shader::Fragment) and sfDebug) {
		std::cout << "SF Debug: Global shader loaded!\n";
	}

	shaderShape.setSize(sf::Vector2f(gameRes.x, gameRes.y));
	shaderShape.setPosition(0, 0);

	globalShader.setUniform("resolution", sf::Vector2f(float(gameRes.x), float(gameRes.y)));
	globalShader.setUniform("lowerLimit", sf::Vector2f(double(25 * angleMultiplier) / double(gameRes.x + (2 * 25 * angleMultiplier)), double(25 * angleMultiplier) / double(gameRes.y + (2 * 25 * angleMultiplier))));
	globalShader.setUniform("shadeColor", sf::Glsl::Vec4(0, 0, 0, 0.5));
	globalShader.setUniform("sunColor", sf::Glsl::Vec4(0, 0, 0, 0.0));
	globalShader.setUniform("heightStep", (float(250.0) / float(255.0) / float(angleMultiplier) / float(25.0)));
	globalShader.setUniform("range", sf::Vector2f(float(gameRes.x) / float(gameRes.x + (2 * 25 * angleMultiplier)), float(gameRes.y) / float(gameRes.y + (2 * 25 * angleMultiplier))));
	globalShader.setUniform("moveValue", sf::Vector2f(1.0 / float(gameRes.x + (2 * 25 * angleMultiplier)) / angleMultiplier, 1.0 / float(gameRes.y + (2 * 25 * angleMultiplier)) / angleMultiplier));

	// view for shade RenderTexture
	shadeView.setSize(gameRes.x + (2 * 25 * angleMultiplier), gameRes.y + (2 * 25 * angleMultiplier));
	shadeView.setCenter(sf::Vector2f(gameRes.x / 2, gameRes.y / 2));
	globalShadowWindow.setView(shadeView);

	// prepares render distance for rendering range optimization
	prepareRenderLimits();
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



//		MAIN FUNCTIONS



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



// renders all graphics
void graphicsRenderer() {

	// creates window
	
	gameWindow.create(sf::VideoMode(gameRes.x, gameRes.y, 32), "Isoria", sf::Style::Fullscreen);
	gameWindow.setActive();

	// main loop
	sf::Clock gameClock;
	double frameTime = 1000000.0 / framerate;


	while (gameWindow.isOpen() and letItBe) {

		for (sf::Event event; gameWindow.pollEvent(event);) {

			switch (event.type) {

			case sf::Event::Closed:
				gameWindow.close();
				serverStatus = false;
				clientStatus = false;
				letItBe = false;
				break;

			case sf::Event::GainedFocus:
				isInFocus = true;
				break;

			case sf::Event::LostFocus:
				isInFocus = false;
				break;

			case sf::Event::TextEntered:
				if (isKeyboardActive) {
					getPressedKey(&event);
				}


			default:
				break;
			}
		}

		if (gameClock.getElapsedTime().asMicroseconds() > frameTime and isInFocus) {
			gameClock.restart();

			

			gameWindow.clear();

			switch (stage) {
				
			case 0:
				render0LoadingScreens();
				break;

			case 1:
				render1MainMenu();
				break;

			case 2:
				render2Gameplay(subStage, &gameWindow, &debugMap, currentDimension, sfDebug, &letItBe);
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

	if (sfDebug) {
		std::cout << "SF Debug: Ended render thread! \n";
	}
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

	startServer(&serverThread, &gameMap, getDatapackNames(), &serverStatus, &isFrozen, &isCurrentlyRunning, &mDatapacks, 8, sfDebug, mainPort);

	if (sfDebug) {
		std::cout << "[ STARTING ] SF debug: Starting client... \n";
	}

	// CHANGE TO LOCAL AGAIN
	client.start(&client, &debugMap, sf::IpAddress::getLocalAddress(), &clientStatus, &mDatapacks, &startGame, &isFrozen, &isCurrentlyRunning, sfDebug, mainPort, 1);

	isWaitingForFreeze = true;
}

void clickedJoin() {
	// add external connection info

	

	if (sfDebug) {
		std::cout << "[ STARTING ] SF debug: Starting client... \n";
	}

	client.start(&client, &gameMap, mainIp, &clientStatus, &mDatapacks, &startGame, &isFrozen, &isCurrentlyRunning, sfDebug, mainPort, 1);

	isWaitingForFreeze = true;

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
						generateMap("test", &mapGeneratorString, sf::Vector2i(500, 500));
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



// main gameplay
void run2x0() {

	timeCounter++;
	if (timeCount == timeCounter) {
		timeCounter = 0;
		
		gameMap.time += timeStep;
		if (gameMap.time > 24000) {
			gameMap.time -= 24000;
		}


		updateShadowAngle(&gameMap.time);	
	}
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
	isCurrentlyRunning = true;
	while (letItBe) {
		if (tickClock.getElapsedTime().asMicroseconds() > tickTime) {
			if (!isInFocus and isSingleplayer) {
				sf::sleep(sf::microseconds(100));
			}
			else {
				tickClock.restart();
				if (!isFrozen or startGame) {

					if (startGame) {
						setStage(2);

						if (sfDebug) {
							std::cout << "[ DONE ] SF debug: Gameplay is here! \n";
						}

						isFrozen = false;
						startGame = false;
					}


					if (isKeyboardActive) {
						if (isKeyboardInTypeMode) {
							updateText();
						}
						else {
							// put things like inventory keys etc here
						}
					}

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

				}
				else {
					isCurrentlyRunning = false;
					sf::sleep(sf::milliseconds(10));

				}
			}
		}
	}

	if (sfDebug) {
		std::cout << "SF Debug: Ended non graphic loop! \n";
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

	gameWindow.setActive(false);

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

	if (sfDebug) {
		std::cout << "SF Debug: Terminated graphic thread! \n";
	}

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