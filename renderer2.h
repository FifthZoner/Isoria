#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include "map.h"
#include "misc.h"
#include "declarations.h"
#include "math.h"

// updates shader for given time with given time value
void updateShadowAngle(unsigned short* value) {
	// 0 - 24000

	// day
	if (*value >= 5000 and *value <= 19000) {
		// sun rising
		if (*value < 6000) {
			// 0 - pi/2

			globalShader.setUniform("sunColor", sf::Glsl::Vec4(0.125 * sin((*value - 5000) * PI / 500), 0.0625 * sin((*value - 5000) * PI / 500), 0, 0.85 * cos((*value - 5000) * PI / 2000)));
			globalShader.setUniform("shadeColor", sf::Glsl::Vec4(0, 0, 0, 0.5 + (0.45 * cos((*value - 5000) * PI / 2000))));
			
		}
		// evening
		else if (*value > 18000) {
			globalShader.setUniform("sunColor", sf::Glsl::Vec4(0.125 * sin((*value - 18000) * PI / 500), 0.0625 * sin((*value - 18000) * PI / 500), 0, 0.85 * sin((*value - 18000) * PI / 2000)));
			globalShader.setUniform("shadeColor", sf::Glsl::Vec4(0, 0, 0, 0.5 + (0.45 * sin((*value - 18000) * PI / 2000))));

		}
		// midday
		else {
			
		}

		// sun angle

		short revert = 1;
		if (*value > 12000) {
			revert = -1;
		}

		globalShader.setUniform("moveValue", sf::Vector2f(revert * angleHelperValue.x * abs(cos((*value - 5000) * PI / 14000)), 0.5 * abs(cos((*value + 2000) * PI / 28000)) * angleHelperValue.y));
	}
	// night
	else {

		globalShader.setUniform("sunColor", sf::Glsl::Vec4(0, 0, 0, 0.85));
		globalShader.setUniform("shadeColor", sf::Glsl::Vec4(0, 0, 0, 0.95));

		unsigned short temp = *value + 5000;
		if (temp > 24000) {
			temp -= 24000;
		}

		// moon angle
		short revert = -1;
		if (temp > 5000) {
			revert = 1;
		}

		globalShader.setUniform("moveValue", sf::Vector2f(revert * angleHelperValue.x * abs(cos(temp * PI / 10000)), 0.5 * abs(cos((temp + 5000) * PI / 20000)) * angleHelperValue.y));
	}



}

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
	globalShadowWindow.clear(sf::Color::Black);

	// shadows are a mess lmao
	for (uint y = limit.lower.y; y < limit.upper.y; y++) {
		for (uint x = limit.lower.x; x < limit.upper.x; x++) {

			// floor
			if (pointer->renderGrid.grid[y][x].floor) {
				globalShadowWindow.draw(pointer->floors.blocks[y][x].shadeSprite, sf::RenderStates::Default);
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

	window->draw(mapMainSprite);

	globalShader.setUniform("shade", globalShadowWindow.getTexture());

	mapShadeSprite.setTexture(globalShadowWindow.getTexture());

	window->draw(mapMainSprite, &globalShader);

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