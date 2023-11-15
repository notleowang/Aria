#include "common.hpp"

class UISystem {

public:
	enum State {
		GAME_START = 0,
		LOAD = 3,
		GAME_OVER = 4
	};

	void init();
	void showWindows();

	// Getters for States
	State getState() { return state; }

private:
	// Helpers
	void showMainMenu(bool* p_open);

	// State
	State state = GAME_START;
};