#pragma once

// moved in a shoddy way as of right now but it's outside the main file

#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <filesystem>

#include "misc.h"
#include "buttons.h"
#include "graphics.h"
#include "convert.h"

std::vector <datapackInfo>* datapacksLPresentL;
std::vector <str> datapacksL;
std::vector <ushort> datapackIdsL;

bool* letItBeL;
bool* isMouseLeftClickedL;
vec2i* gameResL;

//				LAUNCHER

bool laDebug = 0;

// loads info about certain datapack
datapackInfo loadDatapackInfo(std::string path) {

	if (laDebug) {
		std::cout << "LA debug: Found folder: " << path << "\n";
	}

	datapackInfo temp;
	std::string finalPath = path + "/desc.txt";
	temp.name = path.erase(0, 5);
	std::ifstream file;
	file.open(finalPath);
	if (file.is_open()) {
		std::string readString;
		while (file >> readString) {


			if (readString == "version") {
				file >> temp.version;
			}
			else if (readString == "desc") {
				std::getline(file, readString);
				std::getline(file, temp.desc);
			}
		}

		if (laDebug) {
			std::cout << "LA debug: Loaded info, version: " << temp.version << "\n" << "LA debug: Description: " << temp.desc << "\n";
		}


		file.close();
	}
	else if (laDebug) {
		std::cout << "LA debug: Could not load desc.txt from: " << finalPath << "\n";
	}


	return temp;
}

// loads datapack info from the datapacksL/... folder
void checkdatapacksL() {

	std::string path = "data/";
	for (const auto& entry : fs::directory_iterator(path)) {
		datapackInfo temp;
		datapacksLPresentL->push_back(loadDatapackInfo(entry.path().string()));
	}
}

// checks which datapacksL from set file are present in the folder or not and assigns proper values for them
void aredatapacksLPresentL() {
	std::ifstream file;

	file.open("launcher/datapacksL.txt");

	for (std::string inputString; file >> inputString;) {
		bool found = false;
		for (unsigned short n = 0; n < datapacksLPresentL->size(); n++) {
			if (inputString == datapacksLPresentL->at(n).name) {
				datapacksLPresentL->at(n).used = true;
				datapacksL.push_back(inputString);
				datapackIdsL.push_back(n);
				found = true;
				if (laDebug) {
					std::cout << "LA debug: Marked " << inputString << " as active at id " << n << "\n";
				}
				break;
			}
		}
		if (!found) {
			std::cout << "LA debug: Could not find datapack named " << inputString << " in files!" << "\n";
		}
	}


	file.close();
}

// swap two datapacksL in a table
void swapdatapacksL(ushort first, ushort second) {

	if (datapacksLPresentL->at(second).used) {
		for (unsigned short n = 0; n < datapackIdsL.size(); n++) {
			if (datapackIdsL[n] == first) {
				datapackIdsL[n] = second;
				if (laDebug) {
					std::cout << "LA debug: Swapped values of active datapacksL " << first << " and " << second << "\n";
				}
				break;
			}
		}
	}

	datapackInfo temp = datapacksLPresentL->at(first);
	datapacksLPresentL->at(first) = datapacksLPresentL->at(second);
	datapacksLPresentL->at(second) = temp;

	if (laDebug) {
		std::cout << "LA debug: Swapped values of datapacksL " << first << " and " << second << "\n";
	}
}

// sorts datapacksL by giving active ones proper priority
void sortdatapacksL() {
	for (unsigned short n = 0; n < datapackIdsL.size(); n++) {
		swapdatapacksL(n, datapackIdsL[n]);
	}
	datapackIdsL.clear();
	datapacksL.clear();

}

// saves current datapack load order in file
void saveDatapackOrder() {

	datapacksL.clear();
	datapackIdsL.clear();

	std::ofstream file;
	file.open("launcher/datapacksL.txt");

	for (unsigned short n = 0; n < datapacksLPresentL->size(); n++) {
		if (datapacksLPresentL->at(n).used) {
			datapacksL.push_back(datapacksLPresentL->at(n).name);
			file << datapacksLPresentL->at(n).name << "\n";
		}
	}

	file.close();

}

// launcher main function, for the love of titan do NOT expand
void launcher(std::vector <datapackInfo>* datapacksLPresentLPtr, bool* letItBeLPtr, bool* isMouseLeftClickedLPtr, vec2i* gameResLPtr, bool* mainDebugPtr) {
	gameResL = gameResLPtr;
	letItBeL = letItBeLPtr;
	datapacksLPresentL = datapacksLPresentLPtr;
	isMouseLeftClickedL = isMouseLeftClickedLPtr;

	// the initial loading before the game even starts

	// variables to create with default values
	unsigned short launcherFramerate = 60;
	std::string launcherFontPath = "launcher/arial.ttf";
	sf::Font launcherFont;
	unsigned short launcherTextSize = 20;
	unsigned short resolutionNumber = 0;
	ushort framerateNumber = 0;

	// other things
	sf::Texture launcherBackTex;
	sf::Sprite launcherBackSprite;

	sf::Texture lCheckboxChecked;
	sf::Texture lCheckboxUnchecked;

	sf::Texture lLeftButton;
	sf::Texture lRightButton;

	infobox lInfobox;

	// buttons

	buttonTextOnly launcherStartButton;
	buttonTextOnly launcherdatapacksLButton;
	buttonTextOnly launcherSettingsButton;
	buttonTextOnly launcherExitButton;

	buttonTextOnly launcherDBackButton;
	buttonTextCheckbox ldatapacksLListButton;

	buttonTextOnly launcherSBackButton;
	buttonMoveSwitch launcherSResolutionButton;
	buttonMoveSwitch launcherSFramerateButton;




	// settings loading
	std::ifstream file;
	file.open("launcher/settings.txt");
	;
	for (std::string inputString; file >> inputString;) {
		if (inputString == "debug") {
			file >> laDebug;
			*mainDebugPtr = laDebug;
			if (laDebug) {
				std::cout << "LA debug: Debug mode active! Have a nice spam filled console!" << "\n";
			}
		}
		else if (inputString == "framerate") {
			file >> launcherFramerate;
			if (laDebug) {
				std::cout << "LA debug: Loaded launcher framerate as " << launcherFramerate << "\n";
			}
		}
		else if (inputString == "fontPath") {
			std::getline(file, inputString);
			std::getline(file, launcherFontPath);
			if (laDebug) {
				std::cout << "LA debug: Loaded launcher font path as " << launcherFontPath << "\n";
			}
		}
		else if (inputString == "textSize") {
			file >> launcherTextSize;
			if (laDebug) {
				std::cout << "LA debug: Loaded launcher text size as " << launcherTextSize << "\n";
			}
		}
	}

	file.close();

	if (laDebug) {
		std::cout << "LA debug: Loading launcher assets..." << "\n";
	}

	// launcher font
	if (launcherFont.loadFromFile(launcherFontPath) and laDebug) {
		std::cout << "LA debug: Loaded launcher font" << "\n";
	}
	else if (laDebug) {
		std::cout << "LA debug: Launcher font loading error!" << "\n";
		*letItBeL = false;
	}

	// checkbox textures
	if (lCheckboxChecked.loadFromFile("launcher/gfx/checkboxChecked.png") and lCheckboxUnchecked.loadFromFile("launcher/gfx/checkboxUnchecked.png") and laDebug) {
		std::cout << "LA debug: Loaded launcher checkbox textures" << "\n";
	}
	else if (laDebug) {
		std::cout << "LA debug: Launcher checkbox texture loading error!" << "\n";
	}

	// left/right buttons textures
	if (lLeftButton.loadFromFile("launcher/gfx/nextLeft.png") and lRightButton.loadFromFile("launcher/gfx/nextRight.png") and laDebug) {
		std::cout << "LA debug: Loaded launcher next buttons textures" << "\n";
	}
	else if (laDebug) {
		std::cout << "LA debug: Launcher next buttons texture loading error!" << "\n";
	}

	// resolution settings options
	file.open("launcher/content/resolutions.txt");

	std::vector<sf::Vector2i> lResolutionOptions;
	std::vector<std::string> lResolutionNames;

	for (sf::Vector2i temp; file >> temp.x, file >> temp.y;) {
		lResolutionNames.push_back(std::to_string(temp.y) + "x" + std::to_string(temp.x));
		lResolutionOptions.push_back(temp);
	}

	file.close();

	// framerate setting options
	file.open("launcher/content/framerates.txt");

	std::vector<ushort> lFramerateOptions;
	std::vector<std::string> lFramerateNames;

	for (ushort temp; file >> temp;) {
		lFramerateNames.push_back(toString(temp) + " fps");
		lFramerateOptions.push_back(temp);
	}

	file.close();

	file.open("launcherSettings.txt");

	for (std::string readString; file >> readString;) {
		if (readString == "resolution") {
			file >> resolutionNumber;
		}
		if (readString == "framerate") {
			file >> framerateNumber;
		}
	}

	file.close();

	// settings buttons
	launcherSResolutionButton.create(sf::Vector2f(20, 20), &launcherFont, "Resolution", lResolutionNames, &lLeftButton, &lRightButton, resolutionNumber);
	launcherSFramerateButton.create(sf::Vector2f(20, 60), &launcherFont, "Framerate", lFramerateNames, &lLeftButton, &lRightButton, framerateNumber);

	launcherSBackButton.create(sf::Vector2f(780, 550), "BACK", &launcherFont, launcherTextSize);

	// menu buttons
	launcherStartButton.create(sf::Vector2f(750, 50), "START", &launcherFont, launcherTextSize);
	launcherdatapacksLButton.create(sf::Vector2f(750, 100), "DATAPACKS", &launcherFont, launcherTextSize);
	launcherSettingsButton.create(sf::Vector2f(750, 150), "SETTINGS", &launcherFont, launcherTextSize);
	launcherExitButton.create(sf::Vector2f(750, 200), "EXIT", &launcherFont, launcherTextSize);

	// datapack buttons
	launcherDBackButton.create(sf::Vector2f(780, 550), "BACK", &launcherFont, launcherTextSize);
	ldatapacksLListButton.create(&launcherFont, 15, &lCheckboxChecked, &lCheckboxUnchecked);



	// others

	lInfobox.create(&launcherFont, 11);

	// launcher background
	if (launcherBackTex.loadFromFile("launcher/gfx/background.png")) {
		launcherBackSprite.setTexture(launcherBackTex);
		launcherBackSprite.setPosition(0, 0);
		if (laDebug) {
			std::cout << "LA debug: Loaded launcher background texture and created sprite" << "\n";
		}
	}
	else if (laDebug) {
		std::cout << "LA debug: Launcher background loading error!" << "\n";
	}

	if (laDebug) {
		std::cout << "LA debug: Checking datapacks in directory..." << "\n";
	}

	checkdatapacksL();

	if (laDebug) {
		std::cout << "LA debug: Checking datapacks marked as active..." << "\n";
	}

	aredatapacksLPresentL();

	if (laDebug) {
		std::cout << "LA debug: Sorting datapacks by load order..." << "\n";

	}

	sortdatapacksL();

	if (laDebug) {
		std::cout << "LA debug: All datapacks present and their characteristics:" << "\n";
		for (ushort n = 0; n < datapacksLPresentL->size(); n++) {
			std::cout << "LA debug: Name: " << datapacksLPresentL->at(n).name << " and version " << datapacksLPresentL->at(n).version << "\n";
			std::cout << "LA debug: Description: " << datapacksLPresentL->at(n).desc << "\n";
		}
	}

	/*
	Stages
	0 - main launcher window
	1 - datapacksL
	2 - settings
	*/
	unsigned short launcherStage = 0;

	// launcher
	if (letItBeL) {
		double launcherFrameTime = 1000000.0 / launcherFramerate;
		sf::RenderWindow launcherWindow;


		launcherWindow.setActive(true);
		launcherWindow.create(sf::VideoMode(800, 600, 32), "Isoria launcher");

		sf::Clock launcherClock;

		// click detection variables
		bool lIsButtonClicked = false;
		bool lMainOrSecond;
		unsigned short lButtonNumberMain, lButtonNumberSecond;

		*letItBeL = false;
		while (launcherWindow.isOpen())
		{
			sf::Event event;
			while (launcherWindow.pollEvent(event)) {

				if (event.type == sf::Event::Closed) {
					launcherWindow.close();
				}
			}

			if (launcherClock.getElapsedTime().asMicroseconds() > launcherFrameTime) {
				launcherClock.restart();

				launcherWindow.clear();

				// background
				launcherWindow.draw(launcherBackSprite);

				// buttons
				switch (launcherStage) {
				case 0:

					launcherStartButton.draw(&launcherWindow);
					launcherdatapacksLButton.draw(&launcherWindow);
					launcherSettingsButton.draw(&launcherWindow);
					launcherExitButton.draw(&launcherWindow);

					break;

				case 1:

					launcherDBackButton.draw(&launcherWindow);

					for (ushort n = 0; n < datapacksLPresentL->size(); n++) {
						ldatapacksLListButton.draw(&launcherWindow, sf::Vector2f(20, 20 + (20 * n)), datapacksLPresentL->at(n).name, datapacksLPresentL->at(n).used);
					}

					for (ushort n = 0; n < datapacksLPresentL->size(); n++) {
						if (ldatapacksLListButton.isPointOnText(sf::Mouse::getPosition(launcherWindow), sf::Vector2f(20, 20 + (20 * n)))) {

							// displays infobox at the corner of cursor (that's why there's 10 and 19 in coords)
							lInfobox.draw(&launcherWindow, std::vector<std::string> { "Version: " + datapacksLPresentL->at(n).version, datapacksLPresentL->at(n).desc },
								sf::Vector2f(sf::Mouse::getPosition(launcherWindow).x + 10, sf::Mouse::getPosition(launcherWindow).y + 19));

							break;
						}
					}

					break;

				case 2:

					launcherSBackButton.draw(&launcherWindow);

					launcherSResolutionButton.draw(&launcherWindow);
					launcherSFramerateButton.draw(&launcherWindow);

					break;

				default:
					break;
				}





				launcherWindow.display();
			}


			// check mouse click

			switch (launcherStage) {
			case 0: // main menu
				if (!*isMouseLeftClickedL and sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
					*isMouseLeftClickedL = true;

					if (launcherStartButton.checkClickStart(sf::Mouse::getPosition(launcherWindow))) {
						if (laDebug) {
							std::cout << "LA debug: Press started on launcher start button" << "\n";
						}
						lIsButtonClicked = true;
						lButtonNumberMain = 0;
					}
					else if (launcherdatapacksLButton.checkClickStart(sf::Mouse::getPosition(launcherWindow))) {
						if (laDebug) {
							std::cout << "LA debug: Press started on launcher datapacksL button" << "\n";
						}
						lIsButtonClicked = true;
						lButtonNumberMain = 1;
					}
					else if (launcherSettingsButton.checkClickStart(sf::Mouse::getPosition(launcherWindow))) {
						if (laDebug) {
							std::cout << "LA debug: Press started on launcher settings button" << "\n";
						}
						lIsButtonClicked = true;
						lButtonNumberMain = 2;
					}
					else if (launcherExitButton.checkClickStart(sf::Mouse::getPosition(launcherWindow))) {
						if (laDebug) {
							std::cout << "LA debug: Press started on launcher exit button" << "\n";
						}
						lIsButtonClicked = true;
						lButtonNumberMain = 3;
					}
				}
				else if (*isMouseLeftClickedL and !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
					*isMouseLeftClickedL = false;

					if (laDebug) {
						std::cout << "LA debug: Mouse left released" << "\n";
					}
					if (lIsButtonClicked) {
						lIsButtonClicked = false;
						switch (lButtonNumberMain) {
						case 0:
							if (launcherStartButton.checkClickEnd(sf::Mouse::getPosition(launcherWindow))) {
								if (laDebug) {
									std::cout << "LA debug: Launcher start button clicked!" << "\n";
								}

								// saving datapack order
								saveDatapackOrder();

								launcherWindow.close();

								*letItBeL = true;

								// saving settings from settings menu
								std::ofstream save;
								save.open("launcherSettings.txt");

								// saving resolution number and applying resolution to game
								save << "resolution " << resolutionNumber << "\n";
								*gameResL = lResolutionOptions[resolutionNumber];
								if (laDebug) {
									std::cout << "LA debug: Game starting with resolution: " << gameResL->y << " x " << gameResL->x << "\n";
								}

								save << "framerate " << framerateNumber << "\n";

								save.close();



							}

							break;

						case 1:
							if (launcherdatapacksLButton.checkClickEnd(sf::Mouse::getPosition(launcherWindow))) { // back button
								if (laDebug) {
									std::cout << "LA debug: Launcher datapacksL button clicked!" << "\n";
								}

								launcherStage = 1;
							}
							break;

						case 2:
							if (launcherSettingsButton.checkClickEnd(sf::Mouse::getPosition(launcherWindow))) {
								if (laDebug) {
									std::cout << "LA debug: Launcher settings button clicked!" << "\n";
								}

								launcherStage = 2;
							}
							break;

						case 3:
							if (launcherExitButton.checkClickEnd(sf::Mouse::getPosition(launcherWindow))) {
								if (laDebug) {
									std::cout << "LA debug: Launcher exit button clicked!" << "\n";
								}

								launcherWindow.close();

							}
							break;

						default:
							break;
						}
					}
					break;

			case 1: // datapacksL menu

				if (!*isMouseLeftClickedL and sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
					*isMouseLeftClickedL = true;


					if (launcherDBackButton.checkClickStart(sf::Mouse::getPosition(launcherWindow))) {
						if (laDebug) {
							std::cout << "LA debug: Press started on launcher datapacksL back button" << "\n";
						}
						lMainOrSecond = false;
						lIsButtonClicked = true;
						lButtonNumberMain = 0;
					}
					else {
						for (unsigned short n = 0; n < datapacksLPresentL->size(); n++) {
							if (ldatapacksLListButton.checkCheckboxClickStart(sf::Mouse::getPosition(launcherWindow), sf::Vector2f(20, 20 + (20 * n)))) {
								lIsButtonClicked = true;
								lButtonNumberSecond = n;
								lMainOrSecond = true;
								if (laDebug) {
									std::cout << "LA debug: Click started on launcher datapack checkbox " << lButtonNumberSecond << "\n";
								}
							}
						}
					}
				}
				else if (*isMouseLeftClickedL and !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
					*isMouseLeftClickedL = false;

					if (laDebug) {
						std::cout << "LA debug: Mouse left released" << "\n";
					}

					if (!lMainOrSecond) {
						switch (lButtonNumberMain) {
						case 0:

							if (launcherDBackButton.checkClickEnd(sf::Mouse::getPosition(launcherWindow))) {
								if (laDebug) {
									std::cout << "LA debug: Launcher datapacksL back button clicked!" << "\n";
								}

								launcherStage = 0;
							}
							break;

						default:
							break;
						}
					}
					else {
						if (ldatapacksLListButton.checkCheckboxClickEnd(sf::Mouse::getPosition(launcherWindow), sf::Vector2f(20, 20 + (20 * lButtonNumberSecond)))) {
							lIsButtonClicked = false;

							if (laDebug) {
								std::cout << "LA debug: Clicked on launcher datapack checkbox " << lButtonNumberSecond << "\n";
							}

							if (datapacksLPresentL->at(lButtonNumberSecond).used) { // deactivate checkbox
								datapacksLPresentL->at(lButtonNumberSecond).used = !datapacksLPresentL->at(lButtonNumberSecond).used; // switch bool
								for (unsigned short n = lButtonNumberSecond; n < datapacksLPresentL->size() - 1, datapacksLPresentL->at(n + 1).used; n++) {
									datapackInfo temp = datapacksLPresentL->at(n + 1);
									datapacksLPresentL->at(n + 1) = datapacksLPresentL->at(n);
									datapacksLPresentL->at(n) = temp;
								}
							}
							else { // activate checkbox, sorting back
								datapacksLPresentL->at(lButtonNumberSecond).used = !datapacksLPresentL->at(lButtonNumberSecond).used; // switch bool
								for (unsigned short n = lButtonNumberSecond; n > 0 and !datapacksLPresentL->at(n - 1).used; n--) {
									datapackInfo temp = datapacksLPresentL->at(n - 1);
									datapacksLPresentL->at(n - 1) = datapacksLPresentL->at(n);
									datapacksLPresentL->at(n) = temp;
								}
							}
						}
					}

				}

				break;

			case 2: // settings menu

				if (!*isMouseLeftClickedL and sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
					*isMouseLeftClickedL = true;

					if (launcherSBackButton.checkClickStart(sf::Mouse::getPosition(launcherWindow))) {
						if (laDebug) {
							std::cout << "LA debug: Press started on launcher settings back button" << "\n";
						}
						lIsButtonClicked = true;
						lMainOrSecond = false;
						lButtonNumberMain = 0;
					}
					else if (launcherSResolutionButton.checkClickStart(sf::Mouse::getPosition(launcherWindow))) {
						if (laDebug) {
							std::cout << "LA debug: Press started on launcher settings resolution button" << "\n";
						}
						lIsButtonClicked = true;
						lMainOrSecond = true;
						lButtonNumberSecond = 0;
					}
					else if (launcherSFramerateButton.checkClickStart(sf::Mouse::getPosition(launcherWindow))) {
						if (laDebug) {
							std::cout << "LA debug: Press started on launcher settings framerate button" << "\n";
						}
						lIsButtonClicked = true;
						lMainOrSecond = true;
						lButtonNumberSecond = 1;
					}
				}
				else if (*isMouseLeftClickedL and !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
					*isMouseLeftClickedL = false;

					if (laDebug) {
						std::cout << "LA debug: Mouse left released" << "\n";
					}


					if (!lMainOrSecond) {
						switch (lButtonNumberMain) {
						case 0:

							if (launcherSBackButton.checkClickEnd(sf::Mouse::getPosition(launcherWindow))) {
								if (laDebug) {
									std::cout << "LA debug: Launcher settings back button clicked!" << "\n";
								}
								lIsButtonClicked = false;
								launcherStage = 0;
							}
							break;

						default:
							break;
						}
					}
					else {
						switch (lButtonNumberSecond) {
						case 0:

							if (launcherSResolutionButton.checkClickEnd(sf::Mouse::getPosition(launcherWindow), &resolutionNumber)) {
								if (laDebug) {
									std::cout << "LA debug: Launcher settings resolution button clicked!" << "\n";
								}

								lIsButtonClicked = false;
							}
							break;
						case 1:

							if (launcherSFramerateButton.checkClickEnd(sf::Mouse::getPosition(launcherWindow), &framerateNumber)) {
								if (laDebug) {
									std::cout << "LA debug: Launcher settings framerate button clicked!" << "\n";
								}

								lIsButtonClicked = false;
							}
							break;

						default:
							break;
						}
					}

				}

				break;

			default:
				break;
				}
			}
		}
	}
}
