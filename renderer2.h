#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include "map.h"
#include "misc.h"


sf::RenderTexture globalShadowWindow, localShadowWindow; 

sf::RenderTexture mapMainTexture;
sf::View mapMainView;
sf::Sprite mapMainSprite;

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

	// right now not ready for textures that overlap other blocks
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

void render2x0(dimension* pointer, sf::RenderWindow* window) {

	// will be split into threads for simultaneus shading and other things

	renderLimit limit = getRenderLimit(pointer);

	mainRender(pointer, limit);

	mapMainSprite.setTexture(mapMainTexture.getTexture());

	window->draw(mapMainSprite);

}

// renders gameplay
void render2Gameplay(unsigned short r2SubStage, sf::RenderWindow* window, mapContainer* map, unsigned short currentDim, bool debug, bool* allToHell) {
	switch (r2SubStage) {

	case 0:
		render2x0(&map->dimensions[currentDim], window);
		break;

	default:
		if (debug) {
			std::cout << "[ CRITICAL ] R2 debug: Gameplay graphics loop out of range! Exiting..." << "\n";
		}
		*allToHell = false;
		break;
	}
}