#include "game_level.hpp"

bool GameLevel::init(uint level) {
	printf("Initializing game for level: %i\n", level);
	if (level <= 0) {	
		return false;
	}

	switch (level) {
		case 1:
		{
			std::vector<vec4>& terrains = this->terrains_attr;
			std::vector<std::array<float, ENEMY_ATTRIBUTES>>& enemies = this->enemies_attr;
			this->player_starting_pos = vec2(400, 400);
			terrains.push_back(vec4(600, 400, 100, 100));
			terrains.push_back(vec4(600, 50, 1000, 100));
			terrains.push_back(vec4(50, 400, 100, 800));
			terrains.push_back(vec4(600, 750, 1000, 100));
			terrains.push_back(vec4(1150, 400, 100, 800));

			enemies.push_back({300, 600, 100, 100, 0, 0});
			
			// Need to add the exit door
			//this->exit_door_pos = vec2(1100, 400);
		}
			break;
		case 2:
			break;
		default:
			printf("no level provided\n");
			break;
	}
			

	return true;
}