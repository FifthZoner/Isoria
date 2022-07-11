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

		globalShader.setUniform("moveValue", sf::Vector2f(revert * angleHelperValue.x * abs(cos((*value - 5000) * PI / 14000)), 0.5 * abs(cos((*value + 2000) * PI / 28000)) * angleHelperValue.y + (angleHelperValue.y * 0.15)));
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

		globalShader.setUniform("moveValue", sf::Vector2f(revert * angleHelperValue.x * abs(cos(temp * PI / 10000)), 0.5 * abs(cos((temp + 5000) * PI / 20000)) * angleHelperValue.y + (angleHelperValue.y * 0.15)));
	}



}


renderLimit getRenderLimit(dimension* pointer) {
	renderLimit value;

	// temp!
	value.lower = vec2i(0, 0);
	value.upper = pointer->size;


	
	// proper code
	sf::Vector2i coord = getViewCoodrinates();

	value.lower.x = coord.x - mainRenderDistance.x;
	value.lower.y = coord.y - mainRenderDistance.y;
	value.upper.x = coord.x + mainRenderDistance.x;
	value.upper.y = coord.y + mainRenderDistance.y;

	// out of range checks

	if (value.lower.x < 0) {
		value.lower.x = 0;
	}

	if (value.lower.y < 0) {
		value.lower.y = 0;
	}

	if (value.upper.x >= pointer->size.x) {
		value.upper.x = pointer->size.x - 1;
	}

	if (value.upper.y >= pointer->size.y) {
		value.upper.y = pointer->size.y - 1;
	}
	
	

	return value;
}

renderLimit getShadeRenderLimit(dimension* pointer) {
	renderLimit value;

	// temp!
	value.lower = vec2i(0, 0);
	value.upper = pointer->size;

	// proper code
	sf::Vector2i coord = getViewCoodrinates();

	value.lower.x = coord.x - shadeRenderDistance.x;
	value.lower.y = coord.y - shadeRenderDistance.y;
	value.upper.x = coord.x + shadeRenderDistance.x;
	value.upper.y = coord.y + shadeRenderDistance.y;

	// out of range checks

	if (value.lower.x < 0) {
		value.lower.x = 0;
	}

	if (value.lower.y < 0) {
		value.lower.y = 0;
	}

	if (value.upper.x >= pointer->size.x) {
		value.upper.x = pointer->size.x - 1;
	}

	if (value.upper.y >= pointer->size.y) {
		value.upper.y = pointer->size.y - 1;
	}

	//std::cout << value.lower.x << " " << value.lower.y << " " << value.upper.x << " " << value.upper.y << "\n";

	return value;
}

// calculates and saves basic render distances to limit the amount of rendered blocks, makes maps of any reasonable size possible without causing massive framerate decreases
void prepareRenderLimits() {

	mainRenderDistance = sf::Vector2i(gameRes.x / 2 / blockBaseSize + 2, gameRes.y / 2 / blockBaseSize + 2);
	shadeRenderDistance = sf::Vector2i(((gameRes.x / 2) + (angleMultiplier * 25)) / blockBaseSize + 2, ((gameRes.y / 2) + (angleMultiplier * 25)) / blockBaseSize + 1);

	
}

void render2x0(dimension* pointer, sf::RenderWindow* window) {

	// will be split into threads for simultaneus shading and other things

	mapMainTexture.clear();
	globalShadowWindow.clear();

	// pls work
	//pointer->draw(&mapMainTexture, getRenderLimit(pointer), &globalShadowWindow, getShadeRenderLimit(pointer));
	drawMap(&mapMainTexture, getRenderLimit(pointer), &globalShadowWindow, getShadeRenderLimit(pointer));

	mapMainTexture.setView(mapMainView);
	globalShadowWindow.setView(shadeView);

	//mainRender(pointer, getRenderLimit(pointer));

	mapMainSprite.setTexture(mapMainTexture.getTexture());

	// and here comes the shading to handle in another heretic thread

	window->draw(mapMainSprite);

	//shadeRender(pointer, getShadeRenderLimit(pointer));

	globalShadowWindow.display();

	globalShader.setUniform("shade", globalShadowWindow.getTexture());


	window->draw(mapMainSprite, &globalShader);

	globalShadowWindow.clear(sf::Color::Black);
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