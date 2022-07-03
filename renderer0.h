#pragma once
#include "declarations.h"


// renders loading screen graphics
void render0x0LoadingScreen() {
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

void render0x1MapGeneration() {
	lsBackground.draw(&gameWindow);
	versionTextbox.draw(&gameWindow);



}


void render0LoadingScreens() {

	if (subStage == 0) {
		render0x0LoadingScreen();
	}
	else if (subStage == 1) {

	}
}