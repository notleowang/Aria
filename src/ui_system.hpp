#include "common.hpp"

class UISystem {

public:
	enum State {
		MAIN_MENU = 0,
		GAME_START = 1,
		LOAD = 2,
		SAVE = 3,
		GAME_OVER = 4
	};

	void init();
	void showWindows();

	// Getters for States
	State getState() { return state; }

private:
	// Helpers
	void showMainMenu(bool* p_open);
	void CenterText(const char* text);

	// State
	State state = MAIN_MENU;
};