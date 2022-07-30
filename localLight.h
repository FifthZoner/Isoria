#pragma once
#include "shared.h"

// this file contains all things required for local lighting to work

// shader that creates a sphere of not a sphere of light
sf::Shader localLightShader;

// info about a light source
struct localLightSourceInfo {
	// to be used to set position of the light in shader
	sf::Vector2i relativeCoords = sf::Vector2i(0, 0);

	// rgb values will be added to shading map so don't make them too high
	// t value will be subtracted from the map 
	// remember that this is color for the center and it will get smaller with planned formula: 
	// intensity (that is all these values) = color * (1 - (pixelDistanceFromLightSource / radius))
	sf::Color color = sf::Color(0, 0, 0, 0);
	unsigned short radius = 0;
	// height of the light in the map, can be used for interesting effects using planned skipLocalLight parameter for shade textures: <0, 25>
	unsigned short height = 0;

	// skipLocalLight will skip all local light checking but not block it from having on origin there, allowing for things like a light from a furnace
	// it will use g or b parameter of the textures, to be decided
};

class localLightContainer {
private:

public:
};