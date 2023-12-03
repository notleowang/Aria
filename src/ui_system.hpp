#include <vector>

#include "common.hpp"
#include "tiny_ecs_registry.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

enum State {
	MAIN_MENU = 0,
	PAUSE_MENU = 1,
	NEW_GAME = 2,
	PLAY_GAME = 3,
	LOAD = 4,
	SAVE = 5,
	QUIT = 6
};

class UISystem {
public:
	// get singleton instance
	static UISystem* getInstance() {
		if (instancePtr == NULL) {
			instancePtr = new UISystem();
		}
		return instancePtr;
	}

	// deleting copy constructor
	UISystem(const UISystem& obj) = delete;

	void init();
	void showWindows();

	// Getters
	State getState() { return state; }

	// Setters
	void setTutorialFlag(bool isTutorial) { this->isTutorial = isTutorial; }
	void setState(State new_state) { state = new_state; }

private:
	static UISystem* instancePtr;
	UISystem() {}; // default constructor

	// Helpers
	void showMainMenu(bool* p_open);
	void showPauseMenu(bool* p_open);
	void showTutorial(bool* p_open);
	void CenterText(const char* text);
	void WorldCoordinateText(const char* text, float x, float y);

	// State
	State state = MAIN_MENU;

	// Volume
	int volume = 1;

	// bools
	bool isTutorial = false;
};
