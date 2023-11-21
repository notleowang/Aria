#include <vector>

#include "common.hpp"
#include "tiny_ecs_registry.hpp"

enum State {
	MAIN_MENU = 0,
	GAME_START = 1,
	LOAD = 2,
	SAVE = 3,
	QUIT = 4
};

class UISystem {
public:
	void init();
	void showWindows();

	// Getters for States
	State getState() { return state; }

	// Setters
	void setTutorialFlag(bool isTutorial) { this->isTutorial = isTutorial; }

private:
	// Helpers
	void showMainMenu(bool* p_open);
	void showTutorial(bool* p_open);
	void CenterText(const char* text);
	void WorldCoordinateText(const char* text, float x, float y);

	// State
	State state = MAIN_MENU;

	// bools
	bool isTutorial = false;
};