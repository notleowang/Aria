#include "game_level.hpp"

bool GameLevel::init(uint level) {
	printf("Initializing game for level: %i\n", level);

	switch (level) {
		case 1:
			// Temporary level 1 design:
			this->player_starting_pos = vec2(300, 600);
			//this->terrain = {};
			this->exit_door_pos = vec2(1100, 400);
			this->enemies = {};
			
			printf("reached inside switch statement\n");
			break;
		case 2:
			break;
		default:
			printf("no level provided\n");
	}
			

	return true;
}