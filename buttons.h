#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// a button made of only text
class buttonTextOnly {
private:
	bool clickStatus = false;
	sf::Text textSprite;

private:

	// checks if a given point is inside the button
	bool isPointOnButton(sf::Vector2i coords) {
		if (coords.x < textSprite.getPosition().x
			and coords.x > textSprite.getPosition().x - textSprite.getLocalBounds().width
			and coords.y > textSprite.getPosition().y
			and coords.y < textSprite.getPosition().y + textSprite.getCharacterSize()) {
			return 1;
		}
		else {
			return 0;
		}
	}

public:

	// create a button at certain place (right upper corner coords) with certain text, font (pointer!), size and color
	void create(sf::Vector2f coords, std::string text, sf::Font* fontPointer, unsigned short textSize, sf::Color color = sf::Color(255, 255, 255)) {
		textSprite.setFont(*fontPointer);
		textSprite.setCharacterSize(textSize);
		textSprite.setFillColor(color);
		textSprite.setString(text);
		textSprite.setOrigin(sf::Vector2f(textSprite.getLocalBounds().width, 0));
		textSprite.setPosition(coords);
	}

	// check if mouse started click on this button (coords relative to window!)
	bool checkClickStart(sf::Vector2i coords) {

		if (isPointOnButton(coords)) {
			clickStatus = true;
			return 1;
		}

		return 0;
	}

	// check if mouse click ended on this button too (use with start function!)
	bool checkClickEnd(sf::Vector2i coords) {

		clickStatus = false;

		if (isPointOnButton(coords)) {
			return 1;
		}

		return 0;
	}

	// display text in given window
	void draw(sf::RenderWindow* window) {
		window->draw(textSprite);
	}
};

// a button made of text with a checkbox
class buttonTextCheckbox {
private:
	bool clickStatus = false;
	sf::Text textSprite;
	sf::Texture* checkedTex;
	sf::Texture* uncheckedTex;
	sf::Sprite checkboxSprite;

private:

	// checks if a given point is inside the checkbox
	bool isPointOnCheckbox(sf::Vector2i coords) {
		if (coords.x > checkboxSprite.getPosition().x
			and coords.x < checkboxSprite.getPosition().x + checkboxSprite.getLocalBounds().width
			and coords.y > checkboxSprite.getPosition().y
			and coords.y < checkboxSprite.getPosition().y + checkboxSprite.getLocalBounds().height) {
			return 1;
		}
		else {
			return 0;
		}
	}

public:

	// create a button with given font (pointer!), size, checkbox style (both should be the same size and as pointer) and color, wirtten to be usedto represent multiple buttons cause vectors break single button verison
	void create(sf::Font* fontPointer, unsigned short textSize, sf::Texture* checkboxActiveTexture, sf::Texture* checkboxInactiveTexture, unsigned short characterOffset = 3, sf::Color color = sf::Color(255, 255, 255)) {

		textSprite.setFont(*fontPointer);
		textSprite.setCharacterSize(textSize);
		textSprite.setFillColor(color);

		checkedTex = checkboxActiveTexture;
		uncheckedTex = checkboxInactiveTexture;

	}

	// checks if a given point is inside the textbox, not used right now?
	bool isPointOnText(sf::Vector2i coords, sf::Vector2f buttonCoords) {
		if (coords.x > buttonCoords.x + checkedTex->getSize().x
			and coords.x < buttonCoords.x + checkedTex->getSize().x + textSprite.getLocalBounds().width
			and coords.y > buttonCoords.y
			and coords.y < buttonCoords.y + textSprite.getCharacterSize()) {
			return 1;
		}
		else {
			return 0;
		}
	}

	// check if mouse started click on this checkBox (coords relative to window!)
	bool checkCheckboxClickStart(sf::Vector2i coords, sf::Vector2f buttonCoords) {

		checkboxSprite.setPosition(buttonCoords);

		if (isPointOnCheckbox(coords)) {
			clickStatus = true;
			return 1;
		}

		return 0;
	}

	// check if mouse click ended on this checkBox too (use with start function!)
	bool checkCheckboxClickEnd(sf::Vector2i coords, sf::Vector2f buttonCoords) {

		checkboxSprite.setPosition(buttonCoords);

		clickStatus = false;

		if (isPointOnCheckbox(coords)) {
			return 1;
		}

		return 0;
	}


	// display button in given window, at given coordinates (left upper corner coords), text and state
	void draw(sf::RenderWindow* window, sf::Vector2f coords, std::string text, bool isChecked = false, unsigned short characterOffset = 2) {

		textSprite.setPosition(sf::Vector2f(coords.x + checkedTex->getSize().x, coords.y - characterOffset));
		textSprite.setString(text);
		checkboxSprite.setPosition(coords);

		if (isChecked) {
			checkboxSprite.setTexture(*checkedTex);
		}
		else {
			checkboxSprite.setTexture(*uncheckedTex);
		}

		window->draw(textSprite);
		window->draw(checkboxSprite);
	}
};

// a two sided button with window to switch between options
class buttonMoveSwitch {
private:
	sf::Sprite buttonLeft, buttonRight;
	sf::Text text, label;
	bool loop;
	bool clickStatusLeft = false;
	bool clickStatusRight = false;
	std::vector<std::string> optionNames;

	// checks if given coordinates are inside given button
	bool isButtonClicked(sf::Sprite* button, sf::Vector2i mouseCoords) {

		if (mouseCoords.x > button->getPosition().x and
			mouseCoords.x < button->getPosition().x + button->getLocalBounds().width and
			mouseCoords.y > button->getPosition().y and
			mouseCoords.y < button->getPosition().y + button->getLocalBounds().height) {
			return true;
		}

		return false;
	}

	void setOptionName() {
		text.setString(optionNames[currentOption]);
		text.setOrigin(sf::Vector2f(text.getLocalBounds().width / 2, 0));
		text.setPosition(sf::Vector2f((buttonRight.getPosition().x + buttonRight.getLocalBounds().width - buttonLeft.getPosition().x) / 2 + buttonLeft.getPosition().x, text.getPosition().y));
	}

public:
	unsigned short currentOption;

	// function used to initialize button at given coordinates, with given font, options, starting at given option (from 0!), with given button textures (best same size!) and dynamic (to ensure all options fit) or static size
	void create(sf::Vector2f coords, sf::Font* fontPointer, std::string labelText, std::vector<std::string> optionNameStrings, sf::Texture* leftTexturePointer, sf::Texture* rightTexturePointer, unsigned short startingOptionNumber = 0,
		unsigned short distanceFromLabel = 3, unsigned short characterOffset = 2, bool dynamicSize = true, unsigned short distanceFromSides = 3, unsigned short staticSize = 100, sf::Color textColor = sf::Color(255, 255, 255)) {

		optionNames = optionNameStrings;
		currentOption = startingOptionNumber;

		text.setFont(*fontPointer);
		text.setCharacterSize(rightTexturePointer->getSize().y);
		text.setPosition(sf::Vector2f(coords.x + rightTexturePointer->getSize().x + distanceFromSides, coords.y - characterOffset + rightTexturePointer->getSize().y + distanceFromLabel));
		text.setFillColor(textColor);

		label.setFont(*fontPointer);
		label.setCharacterSize(rightTexturePointer->getSize().y);
		label.setPosition(sf::Vector2f(coords.x, coords.y - characterOffset));
		label.setString(labelText);
		label.setFillColor(textColor);

		// check max text size
		unsigned short maxLength = 0;
		if (dynamicSize) {
			for (unsigned short n = 0; n < optionNameStrings.size(); n++) {
				text.setString(optionNameStrings[n]);
				if (text.getLocalBounds().width > maxLength) {
					maxLength = text.getLocalBounds().width;
				}
			}
		}
		else {
			maxLength = staticSize;
		}

		buttonLeft.setTexture(*leftTexturePointer);
		buttonLeft.setPosition(sf::Vector2f(coords.x, coords.y + rightTexturePointer->getSize().y + distanceFromLabel));

		buttonRight.setTexture(*rightTexturePointer);
		buttonRight.setPosition(sf::Vector2f(coords.x + maxLength + (2 * distanceFromSides) + rightTexturePointer->getSize().x, coords.y + rightTexturePointer->getSize().y + distanceFromLabel));
		
		setOptionName();
	}

	bool checkClickStart(sf::Vector2i mouseCoords) {
		if (isButtonClicked(&buttonLeft, mouseCoords)) {
			clickStatusLeft = true;
			return true;
		}
		else if (isButtonClicked(&buttonRight, mouseCoords)) {
			clickStatusRight = true;
			return true;
		}
		
		return false;
	}

	bool checkClickEnd(sf::Vector2i mouseCoords, unsigned short* optionVariable) {

		

		if (clickStatusLeft and isButtonClicked(&buttonLeft, mouseCoords)) {

			if (!currentOption and loop) {
				currentOption = optionNames.size() - 1;
			}
			else if (currentOption) {
				currentOption--;
			}

			*optionVariable = currentOption;

			clickStatusLeft = false;
			clickStatusRight = false;

			setOptionName();

			return true;
		}
		else if (clickStatusRight and isButtonClicked(&buttonRight, mouseCoords)) {

			currentOption++;

			if (currentOption == optionNames.size()) {
				if (loop) {
					currentOption = 0;
				}
				else {
					currentOption--;
				}
			}

			*optionVariable = currentOption;

			clickStatusLeft = false;
			clickStatusRight = false;

			setOptionName();

			return true;
		}

		clickStatusLeft = false;
		clickStatusRight = false;

		return false;
	}

	// function to display the button
	void draw(sf::RenderWindow* window) {
		window->draw(label);
		window->draw(text);
		window->draw(buttonLeft);
		window->draw(buttonRight);
	}

};