#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include "map.h"
#include "misc.h"

// defines horizontal to vertical ratio of the global shadows
const unsigned short angleMultiplier = 6;


sf::RenderTexture globalShadowWindow, localShadowWindow; 

sf::RenderTexture mapMainTexture;
sf::View mapMainView;
sf::Sprite mapMainSprite;
sf::Sprite mapShadeSprite;
sf::Shader globalShader;

renderLimit getRenderLimit(dimension* pointer) {
	renderLimit value;

	// temp!
	value.lower = vec2i(0, 0);
	value.upper = pointer->size;

	return value;
}

renderLimit getShadeRenderLimit(dimension* pointer) {
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

void shadeRender(dimension* pointer, renderLimit limit) {
	globalShadowWindow.clear();

	// shadows are a mess lmao
	for (uint y = limit.lower.y; y < limit.upper.y; y++) {
		for (uint x = limit.lower.x; x < limit.upper.x; x++) {

			// floor
			if (pointer->renderGrid.grid[y][x].floor) {
				globalShadowWindow.draw(pointer->floors.blocks[y][x].shadeSprite);
			}

			// wall
			if (pointer->renderGrid.grid[y][x].wall) {
				globalShadowWindow.draw(pointer->walls.blocks[y][x].shadeSprite);
			}

		}
	}

	globalShadowWindow.display();
}

void render2x0(dimension* pointer, sf::RenderWindow* window) {

	// will be split into threads for simultaneus shading and other things

	mainRender(pointer, getRenderLimit(pointer));

	mapMainSprite.setTexture(mapMainTexture.getTexture());

	// and here comes the shading to handle in another heretic thread

	shadeRender(pointer, getShadeRenderLimit(pointer));

	mapShadeSprite.setTexture(globalShadowWindow.getTexture());

	window->draw(mapShadeSprite);

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