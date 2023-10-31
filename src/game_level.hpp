#include <components.hpp>
#include <common.hpp> 

// Class that describes the current configurations of the current game level.
// This includes things like the player's starting position, the enemies, the terrain, etc.
// We don't have access to the renderer until the renderer system runs so we should only be defining
// the attributes of the enemies and terrain
const int TERRAIN_ATTRIBUTES = 4;
const int ENEMY_ATTRIBUTES = 6;
const int TEXT_ATTRIBUTES = 6;

// enum for each level type
enum Level {
	TUTORIAL = 0,
	LEVEL_1 = TUTORIAL + 1,
	LEVEL_2 = LEVEL_1 + 1,
	LEVEL_3 = LEVEL_2 + 1,
	LEVEL_4 = LEVEL_3 + 1,
	POWER_UP = LEVEL_2 + 1,
};

class GameLevel
{
public:
	uint curr_level;
	vec2 player_starting_pos;
	vec2 exit_door_pos;

	std::vector<std::string> texts;

	// pos_x, pos_y, font_size, r, g, b
	std::vector<std::array<float, TEXT_ATTRIBUTES>> text_attrs;

	// pos_x, pos_y
	std::vector<vec2> floor_pos;

	// pos_x, pos_y, scale_x, scale_y, moveable?
	std::vector<std::pair<vec4, bool>> terrains_attr;

	// pos_x, pos_y, vel_x, vel_y, scale_x, scale_y
	std::vector<std::array<float, ENEMY_ATTRIBUTES>> enemies_attr;

	bool init(uint level);

	uint getCurrLevel() {
		return curr_level;
	}

	vec2& getPlayerStartingPos() {
		return player_starting_pos;
	}

	vec2& getExitDoorPos() {
		return exit_door_pos;
	}

	std::vector<std::string>& getTexts() {
		return texts;
	}

	std::vector<std::array<float, TEXT_ATTRIBUTES>>& getTextAttrs() {
		return text_attrs;
	}

	std::vector<vec2>& getFloorPos() {
		return floor_pos;
	}

	std::vector<std::pair<vec4, bool>>& getTerrains() {
		return terrains_attr;
	}

	std::vector<std::array<float,ENEMY_ATTRIBUTES>>& getEnemies() {
		return enemies_attr;
	}
};