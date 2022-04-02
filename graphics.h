#pragma once

// header file to store non intractable graphic elements used by the game

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <filesystem>
#include "convert.h"
#include "quickWrite.h"

// for debug
#include <iostream>

namespace fs = std::filesystem;



//		NON INTERACTABLE DYNAMIC ELEMENTS

// a textbox used to show somethings properties
class infobox {
private:
	sf::Text text;
	sf::RectangleShape box;

public:

	// function used to initialize a text box with given font, character size, box colors, width
	void create(sf::Font* fontPointer, unsigned short characterSize, sf::Color textColor = sf::Color(0, 0, 0), sf::Color boxFillColor = sf::Color(255, 255, 255), sf::Color boxBorderColor = sf::Color(0, 0, 0), unsigned short boxBorderWidth = 1) {

		text.setFont(*fontPointer);
		text.setCharacterSize(characterSize);
		text.setFillColor(textColor);

		box.setFillColor(boxFillColor);
		box.setOutlineColor(boxBorderColor);
		box.setOutlineThickness(boxBorderWidth);

	}

	// function used to draw infobox in given window, with given lines of text, coordinates and other parameters
	void draw(sf::RenderWindow* window, std::vector<std::string> linesToDraw, sf::Vector2f coords, unsigned short characterOffset = 2, unsigned short distanceFromBorder = 3, unsigned short distanceBetweenLines = 3) {

		float maxLength = 0;

		// initial size check
		for (unsigned short n = 0; n < linesToDraw.size(); n++) {
			text.setString(linesToDraw[n]);
			if (text.getLocalBounds().width > maxLength) {
				maxLength = text.getLocalBounds().width;
			}
		}

		// sets background box size depending on text size
		box.setSize(sf::Vector2f(maxLength + (distanceFromBorder * 2), text.getCharacterSize() * linesToDraw.size() + ((linesToDraw.size() - 1) * distanceBetweenLines) + (2 * distanceFromBorder)));
		box.setPosition(coords);
		window->draw(box);

		for (unsigned short n = 0; n < linesToDraw.size(); n++) {
			text.setString(linesToDraw[n]);
			text.setPosition(sf::Vector2f(coords.x + distanceFromBorder, coords.y + distanceFromBorder + (n * (text.getCharacterSize() + distanceBetweenLines)) - characterOffset));
			window->draw(text);
		}


	}
};

// used to easily store and use changeable background sprites
class spriteBackgroundChangeable {
private:
	std::vector<sf::Texture> textures;
	sf::Sprite sprite;
	sf::Vector2i res;

public:

	unsigned short current, maximum;

private:

	void setTexture() {

		sprite.setPosition(sf::Vector2f(0, 0));
		sprite.setTexture(textures[current]);
		sprite.setScale(sf::Vector2f(asFloat(res.x) / asFloat(textures[current].getSize().x), asFloat(res.y) / asFloat(textures[current].getSize().y)));
		//std::cout << asFloat(res.x) << " " << asFloat(textures[current].getSize().x) << " " << asFloat(res.y) << " " << asFloat(textures[current].getSize().y) << "\n";
	}

public:

	void create(std::vector<std::string> pathsToTextures, sf::Vector2i resolution, unsigned short startingImage = 0) {

		current = startingImage;
		maximum = pathsToTextures.size();
		res = resolution;

		textures.resize(pathsToTextures.size());
		for (unsigned short n = 0; n < textures.size(); n++) {
			textures[n].loadFromFile(pathsToTextures[n]);
		}

		setTexture();
	}

	void createFromFolder(std::string path, sf::Vector2i resolution, unsigned short startingImage = 0) {

		current = startingImage;
		res = resolution;

		std::vector<std::string> pathsToTextures;

		for (const auto& entry : fs::directory_iterator(path)) {
			pathsToTextures.push_back(entry.path().string());
		}

		maximum = pathsToTextures.size();

		textures.resize(pathsToTextures.size());
		for (unsigned short n = 0; n < textures.size(); n++) {
			textures[n].loadFromFile(pathsToTextures[n]);
		}

		setTexture();
	}

	void draw(sf::RenderWindow* window) {
		window->draw(sprite);
	}
};

// used to create a centered horizontally and vertically textbox at given position
class centeredTextbox {
private:
	sf::Text text;

	void center() {
		text.setOrigin(sf::Vector2f(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2));
	}

public:

	// creates a textbox with given font, position, string, character size and color
	void create(sf::Font* fontPointer, sf::Vector2f coords, std::string startingString, ushort characterSize = 30, sf::Color characterColor = sf::Color(255, 255, 255)) {
		
		text.setFont(*fontPointer);
		text.setString(startingString);
		text.setPosition(coords);
		text.setCharacterSize(characterSize);
		text.setFillColor(characterColor);

		center();
	}

	// sets given string to the textbox, always use it in the same thread as the textbox!
	void setString(std::string string){

		text.setString(string);

		center();
	}

	// draws textbox to given RenderWindow
	void draw(sf::RenderWindow* window) {
		window->draw(text);
	}
};