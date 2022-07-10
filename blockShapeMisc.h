#pragma once

#include "shared.h"

// stores block shape variants for transformations
struct blockShapeVariant {
	sf::Texture up, down, right, left, center, upLeft, upRight, downRight, downLeft;

	// give the one string as path from file
	void loadFromFile(std::string path) {
		debugMsg(std::string("GL Debug: Loading block size variants from: " + path));

		unsigned short dotNumber = path.find(".");
		std::string extension = path;
		extension.erase(0, dotNumber);
		path.erase(dotNumber, path.size() - dotNumber);

		debugMsg(std::string("GL Debug: Loading textures with paths: " + path + " and extension: " + extension));

		up.loadFromFile(std::string(path + "up" + extension));
		down.loadFromFile(std::string(path + "down" + extension));
		left.loadFromFile(std::string(path + "left" + extension));
		right.loadFromFile(std::string(path + "right" + extension));
		center.loadFromFile(std::string(path + "center" + extension));
		upLeft.loadFromFile(std::string(path + "upLeft" + extension));
		downLeft.loadFromFile(std::string(path + "upRight" + extension));
		upRight.loadFromFile(std::string(path + "downLeft" + extension));
		downRight.loadFromFile(std::string(path + "downRight" + extension));

		debugMsg(std::string("GL Debug: Loaded!"));
	}

	void loadIntoShaders() {

		blockShapeCenterShader.setUniform("upTexture", up);
		blockShapeCenterShader.setUniform("downTexture", down);
		blockShapeCenterShader.setUniform("leftTexture", left);
		blockShapeCenterShader.setUniform("rightTexture", right);
		blockShapeCenterShader.setUniform("centerTexture", center);
		blockShapeCornerShader.setUniform("upLeftTexture", upLeft);
		blockShapeCornerShader.setUniform("downLeftTexture", downLeft);
		blockShapeCornerShader.setUniform("upRightTexture", upRight);
		blockShapeCornerShader.setUniform("downRightTexture", downRight);
	}
};

// these allow for fairly low memory usage and only need 2 sprites compared to over 10 times more memory for 1
// performance decrease from two sprites should be fairly small but fps can be boosted by multithreaded rendering

std::vector<blockShapeVariant> blockShapeVariants;

// stores corner textures of given variant
struct shapeCornerContainer {

	// accessed by bools with order: up left -> up right -> down left -> down right
	sf::Texture variantTable[2][2][2][2];

	// prepares corner textures for given variant stored in an array
	void prepareCornerVariants(unsigned short shapeVariantNumber, sf::Texture* texture, bool doShadeTexture, sf::Texture* shadeTexture) {
		
		blockShapeVariants[shapeVariantNumber].loadIntoShaders();
		blockShapeCornerShader.setUniform("resolution", sf::Vector2i(blockBaseSize, blockBaseSize));

		for (char b1 = 0; b1 < 2; b1++) {
			for (char b2 = 0; b2 < 2; b2++) {
				for (char b3 = 0; b3 < 2; b3++) {
					for (char b4 = 0; b4 < 2; b4++) {
						blockShapeCornerShader.setUniform("addUpLeft", b1);
						blockShapeCornerShader.setUniform("addUpRight", b2);
						blockShapeCornerShader.setUniform("addDownLeft", b3);
						blockShapeCornerShader.setUniform("addDownRight", b4);
						blockShapeCornerShader.setUniform("targetTexture", *texture);

						blockShapeTexture.clear(sf::Color(0, 0, 0, 0));
						blockShapeTexture.draw(blockShapeSprite, &blockShapeCornerShader);
						blockShapeTexture.display();
						variantTable[b1][b2][b3][b4] = blockShapeTexture.getTexture();
						

						if (doShadeTexture) {
							blockShapeCornerShader.setUniform("targetTexture", *shadeTexture);
							blockShapeTexture.clear(sf::Color(0, 0, 0, 0));
							blockShapeTexture.draw(blockShapeSprite, &blockShapeCornerShader);
							blockShapeTexture.display();
							variantTable[b1][b2][b3][b4] = blockShapeTexture.getTexture();
						}
					}
				}
			}
		}
	}
};

// stores center (main) textures of given variant
struct shapeCenterContainer {
	// accessed by bools with order: center -> top -> right -> down -> left
	sf::Texture variantTable[2][2][2][2][2];

	void prepareCenterVariants(unsigned short shapeVariantNumber, sf::Texture* texture, bool doShadeTexture, sf::Texture* shadeTexture) {

		blockShapeVariants[shapeVariantNumber].loadIntoShaders();
		blockShapeCornerShader.setUniform("resolution", sf::Vector2i(blockBaseSize, blockBaseSize));

		for (char b1 = 0; b1 < 2; b1++) {
			for (char b2 = 0; b2 < 2; b2++) {
				for (char b3 = 0; b3 < 2; b3++) {
					for (char b4 = 0; b4 < 2; b4++) {
						for (char b5 = 0; b5 < 2; b5++) {
							blockShapeCornerShader.setUniform("addCenter", b1);
							blockShapeCornerShader.setUniform("addUp", b2);
							blockShapeCornerShader.setUniform("addLeft", b3);
							blockShapeCornerShader.setUniform("addDown", b4);
							blockShapeCornerShader.setUniform("addLeft", b5);
							blockShapeCornerShader.setUniform("targetTexture", *texture);

							blockShapeTexture.clear(sf::Color(0, 0, 0, 0));
							blockShapeTexture.draw(blockShapeSprite, &blockShapeCornerShader);
							blockShapeTexture.display();
							variantTable[b1][b2][b3][b4][b5] = blockShapeTexture.getTexture();


							if (doShadeTexture) {
								blockShapeCornerShader.setUniform("targetTexture", *shadeTexture);
								blockShapeTexture.clear(sf::Color(0, 0, 0, 0));
								blockShapeTexture.draw(blockShapeSprite, &blockShapeCornerShader);
								blockShapeTexture.display();
								variantTable[b1][b2][b3][b4][b5] = blockShapeTexture.getTexture();
							}
						}
					}
				}
			}
		}
	}
};