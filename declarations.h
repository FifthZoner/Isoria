#pragma once

#include "misc.h"
#include "convert.h"
#include "map.h"
#include "data.h"
#include "mapGenerator.h"
#include "server.h"
#include "shared.h"
#include "keyboard.h"

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

//				DATAPACKS

datapackContainer mDatapacks;
lDatapackPathsContainer mDatapackPathsContainer;


// sets current stage safely
void setStage(ushort setStage, ushort setSubStage = 0) {
	threadLock.lock();
	stage = setStage;
	subStage = setSubStage;
	threadLock.unlock();
}