#pragma once

#include "shared.h"

// prepares the keyboard with distinction as is to register letters as a string or as an individual input (for example keys to open inventory etc)
void prepareKeyboard(bool typeMode = true) {
	// empties the queue just in case if it gets a new element after the disabling emptying
	while (!keysPressedQueue.empty()) {
		keysPressedQueue.pop();
	}

	isKeyboardActive = true;

	debugMsg("KB Debug: Keyboard active");
}

void endKeyboard() {

	isKeyboardActive = false;

	debugMsg("KB Debug: Keyboard inactive");

	// empties the queue
	while (!keysPressedQueue.empty()) {
		keysPressedQueue.pop();
	}

	debugMsg("KB Debug: Keyboard queue flushed");
}

// put pressed key into queue if valid else 0
void getPressedKey(sf::Event* event) {
	
	if (event->text.unicode < 128 and event->text.unicode > 31) {
		// normal letters any human being can comprehend

		keysPressedQueue.push(event->text.unicode);
	}
	else if (event->text.unicode == 8) {
		// backspace

		keysPressedQueue.push(event->text.unicode);
	}
	else if (event->text.unicode == 27) {
		// escape

		keysPressedQueue.push(event->text.unicode);
	}
	else if (event->text.unicode == 13) {

		//enter

		keysPressedQueue.push(event->text.unicode);
	}
}

// updates current input text with letters from the queue, returns 0 when only letters clicked, 1 when enter and 2 when escape
char updateText() {

	while (keysPressedQueue.size() > 0) {
		if (keysPressedQueue.front() == 13) {
			// enter clicked
			debugMsg("KB Debug: Enter input");

			while (!keysPressedQueue.empty()) {
				keysPressedQueue.pop();
			}

			return 1;
		}
		else if (keysPressedQueue.front() == 27) {
			// escape clicked
			debugMsg("KB Debug: Escape input");

			while (!keysPressedQueue.empty()) {
				keysPressedQueue.pop();
			}

			return 2;
		}
		else if (keysPressedQueue.front() == 8) {
			// backspace clicked
			debugMsg("KB Debug: Backspace input");

			if (keyboardInputString.size() > 0) {
				keyboardInputString.resize(keyboardInputString.size() - 1);
			}

			// removes the backspace from the queue
			keysPressedQueue.pop();

			debugMsg(std::string("KB Debug: Current text: " + keyboardInputString));
		}
		else {
			debugMsg("KB Debug: Keyboard input");

			keyboardInputString += keysPressedQueue.front();

			// removes the letter from the queue
			keysPressedQueue.pop();

			debugMsg(std::string("KB Debug: Current text: " + keyboardInputString));
		}
	}
	return 0;
}