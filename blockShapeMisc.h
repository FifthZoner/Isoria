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

		up.loadFromFile(std::string(path + "Up" + extension));
		down.loadFromFile(std::string(path + "Down" + extension));
		left.loadFromFile(std::string(path + "Left" + extension));
		right.loadFromFile(std::string(path + "Right" + extension));
		center.loadFromFile(std::string(path + "Center" + extension));
		upLeft.loadFromFile(std::string(path + "UpLeft" + extension));
		downLeft.loadFromFile(std::string(path + "DownLeft" + extension));
		upRight.loadFromFile(std::string(path + "UpRight" + extension));
		downRight.loadFromFile(std::string(path + "DownRight" + extension));

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
	sf::Texture variantShadeTable[2][2][2][2];

	// prepares corner textures for given variant stored in an array
	void prepareCornerVariants(unsigned short shapeVariantNumber, sf::Texture* texture, bool doShadeTexture, sf::Texture* shadeTexture) {

		blockShapeVariants[shapeVariantNumber].loadIntoShaders();
		blockShapeCornerShader.setUniform("resolution", sf::Vector2f(blockBaseSize, blockBaseSize));

		for (char b1 = 0; b1 < 2; b1++) {
			for (char b2 = 0; b2 < 2; b2++) {
				for (char b3 = 0; b3 < 2; b3++) {
					for (char b4 = 0; b4 < 2; b4++) {
						blockShapeCornerShader.setUniform("isShadow", false);

						blockShapeCornerShader.setUniform("addDownLeft", b1);
						blockShapeCornerShader.setUniform("addDownRight", b2);
						blockShapeCornerShader.setUniform("addUpLeft", b3);
						blockShapeCornerShader.setUniform("addUpRight", b4);

						blockShapeTexture.clear(sf::Color(0, 0, 0, 0));
						blockShapeSprite.setTexture(*texture);
						blockShapeTexture.draw(blockShapeSprite, &blockShapeCornerShader);
						blockShapeTexture.display();
						variantTable[b1][b2][b3][b4] = blockShapeTexture.getTexture();


						if (doShadeTexture) {
							blockShapeCornerShader.setUniform("isShadow", true);

							blockShapeTexture.clear(sf::Color(0, 0, 0, 0));
							blockShapeSprite.setTexture(*shadeTexture);
							blockShapeTexture.draw(blockShapeSprite, &blockShapeCornerShader);
							blockShapeTexture.display();
							variantShadeTable[b1][b2][b3][b4] = blockShapeTexture.getTexture();
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
	sf::Texture variantShadeTable[2][2][2][2][2];


	void prepareCenterVariants(unsigned short shapeVariantNumber, sf::Texture* texture, bool doShadeTexture, sf::Texture* shadeTexture) {

		blockShapeVariants[shapeVariantNumber].loadIntoShaders();
		blockShapeCenterShader.setUniform("resolution", sf::Vector2f(float(blockBaseSize), float(blockBaseSize)));

		for (char b1 = 0; b1 < 2; b1++) {
			for (char b2 = 0; b2 < 2; b2++) {
				for (char b3 = 0; b3 < 2; b3++) {
					for (char b4 = 0; b4 < 2; b4++) {
						for (char b5 = 0; b5 < 2; b5++) {
							blockShapeCenterShader.setUniform("isShadow", false);

							blockShapeCenterShader.setUniform("addCenter", b1);
							blockShapeCenterShader.setUniform("addDown", b2);
							blockShapeCenterShader.setUniform("addRight", b3);
							blockShapeCenterShader.setUniform("addUp", b4);
							blockShapeCenterShader.setUniform("addLeft", b5);
							sf::RectangleShape temp;
							temp.setSize(sf::Vector2f(24, 24));
							temp.setPosition(0, 0);

							//blockShapeTexture.create(blockBaseSize, blockBaseSize);
							blockShapeTexture.clear(sf::Color(0, 0, 0, 0));
							blockShapeSprite.setTexture(*texture);
							blockShapeTexture.draw(blockShapeSprite, &blockShapeCenterShader);
							blockShapeTexture.draw(temp, &blockShapeCenterShader);
							blockShapeTexture.display();

							variantTable[int(b1)][int(b2)][int(b3)][int(b4)][int(b5)] = blockShapeTexture.getTexture();

							if (doShadeTexture) {
								blockShapeCenterShader.setUniform("isShadow", true);

								blockShapeTexture.clear(sf::Color(0, 0, 0, 0));
								blockShapeSprite.setTexture(*shadeTexture);
								blockShapeTexture.draw(blockShapeSprite, &blockShapeCenterShader);
								blockShapeTexture.display();
								variantShadeTable[b1][b2][b3][b4][b5] = blockShapeTexture.getTexture();
							}
						}
					}
				}
			}
		}
	}
};