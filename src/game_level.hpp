// TODO: might not need all of these libs (figure out what to remove)
#include "common.hpp"
#include "components.hpp"

// Class that describes the current configurations of the current game level.
// This includes things like the player's starting position, the enemies, the terrain, etc.
class GameLevel
{
public:
	vec2 player_starting_pos;
	std::vector<Enemy> enemies;
	//std::array<Terrain> terrain; needs collision pr
	std::vector<vec2> light_source_pos;
	vec2 exit_door_pos;

	bool init(uint level);

	vec2& getPlayerStartingPos() {
		return player_starting_pos;
	}
	std::vector<Enemy>& getEnemies() {
		return enemies;
	}
	/*std::vector<Terrain>& getTerrain() {
		return terrain;
	}*/
	std::vector<vec2>& getLightSourcePos() {
		return light_source_pos;
	}

	vec2& getExitDoorPos() {
		return exit_door_pos;
	}
};