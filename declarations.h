#pragma once

#include "misc.h"
#include "convert.h"
#include "map.h"
#include "data.h"
#include "mapGenerator.h"
#include "server.h"
#include "shared.h"
#include "keyboard.h"
#include "buttons.h"
#include "graphics.h"

// moved declarations here to have them apply everywhere

clientClass client;

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

mapContainer debugMap;

dimension* currentDimensionPointer;
mapContainer* currentMap;

//				DATAPACKS

datapackContainer mDatapacks;
lDatapackPathsContainer mDatapackPathsContainer;

std::queue<unsigned short> renderContainerQueue;

// hybrid renderer expansion to avoid bugs
const unsigned short hybridRenderOffset = 2;
const float hybridRenderTableMultiplier = 1.1;
renderLimit hybridRenderBorder;
renderLimit hybridRenderCurrent;


std::thread hybridRenderServiceThread;

// sets current stage safely
void setStage(ushort setStage, ushort setSubStage = 0) {
	threadLock.lock();
	stage = setStage;
	subStage = setSubStage;
	threadLock.unlock();
}


sf::Vector2i getViewCoodrinates(sf::Vector2i value = sf::Vector2i(0, 0)) {

	value.x = mapMainView.getCenter().x / blockBaseSize;
	value.y = mapMainView.getCenter().y / blockBaseSize;

	return value;
}