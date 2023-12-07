#include <components.hpp>
#include <common.hpp> 

// Class that describes the current configurations of the current game level.
// This includes things like the player's starting position, the enemies, the terrain, etc.
// We don't have access to the renderer until the renderer system runs so we should only be defining
// the attributes of the enemies and terrain
const int TERRAIN_ATTRIBUTES = 5;
const int ENEMY_ATTRIBUTES = 6;
const int TEXT_ATTRIBUTES = 6;
const int OBSTACLE_ATTRIBUTES = 3;

const float default_north_height = 100.f;
const float default_south_height = 25.f;
const float default_side_width = 25.f;

const vec2 NULL_POS = vec2(-1, -1);

// enum for each level type
enum Level {
	CUTSCENE_1 = 0,
	TUTORIAL = CUTSCENE_1 + 1,
	TUTORIAL_2 = TUTORIAL + 1,
	LEVEL_1 = TUTORIAL_2 + 1,
	FIRE_BOSS = LEVEL_1 + 1,
	CUTSCENE_2 = FIRE_BOSS + 1,
	LEVEL_2 = CUTSCENE_2 + 1,
	EARTH_BOSS = LEVEL_2 + 1,
	CUTSCENE_3 = EARTH_BOSS + 1,
	LEVEL_3 = CUTSCENE_3 + 1,
	LIGHTNING_BOSS = LEVEL_3 + 1,
	CUTSCENE_4 = LIGHTNING_BOSS + 1,
	LEVEL_4 = CUTSCENE_4 + 1,
	WATER_BOSS = LEVEL_4 + 1,
	CUTSCENE_5 = WATER_BOSS + 1,
	FINAL_BOSS = CUTSCENE_5 + 1,
	CUTSCENE_6 = FINAL_BOSS + 1,
	THE_END = CUTSCENE_6 + 1,
	POWER_UP = THE_END + 1
};

//Enemy types to re-use later
const Enemy WATER_NORMAL = { 10.f, 3000.f, 0.5f, 1.f, ElementType::WATER, true };
const Enemy WATER_HIGH_DAMAGE = { 20.f, 3000.f, 0.5f, 1.f, ElementType::WATER, false };

const Enemy FIRE_NORMAL = { 10.f, 3000.f, 0.5f, 1.f, ElementType::FIRE, true };
const Enemy FIRE_HIGH_DAMAGE = { 20.f, 3000.f, 0.5f, 1.f, ElementType::FIRE, false };

const Enemy EARTH_NORMAL = { 10.f, 3000.f, 0.5f, 1.f, ElementType::EARTH, true };
const Enemy EARTH_HIGH_DAMAGE = { 20.f, 3000.f, 0.5f, 1.f, ElementType::EARTH, false };

const Enemy LIGHTNING_NORMAL = { 10.f, 3000.f, 0.5f, 1.f, ElementType::LIGHTNING, true };
const Enemy LIGHTNING_HIGH_DAMAGE = { 20.f, 3000.f, 0.5f, 1.f, ElementType::LIGHTNING, false };

const Enemy FINAL_BOSS_ATTRS = { 20.f, 3000.f, 0.5f, 1.f, ElementType::COMBO, false };

// Terrain types
const Terrain NORTH_STATIONARY = {DIRECTION::N, 0.f, false};
const Terrain SIDE_STATIONARY = {DIRECTION::E, 0.f, false};
const Terrain SOUTH_STATIONARY = {DIRECTION::S, 0.f, false};
const Terrain GENERIC_STATIONARY = {DIRECTION::W, 0.f, false};
const Terrain GENERIC_MOVABLE_SLOW = {DIRECTION::W, 150.f, true};
const Terrain GENERIC_MOVABLE_FAST = {DIRECTION::W, 200.f, true};

class GameLevel
{
public:
	uint curr_level;
	vec2 player_starting_pos;
	vec2 exit_door_pos;
	bool is_cutscene;
	bool is_boss_level;
	vec2 cutscene_player_velocity;
	int life_orb_piece;
	bool hasEnemies;
	bool power_up_next_level = false;

	std::vector<vec2> health_packs_pos;

	std::vector<std::string> texts;

	// pos_x, pos_y, font_size, r, g, b
	std::vector<std::array<float, TEXT_ATTRIBUTES>> text_attrs;

	// pos_x, pos_y
	std::vector<vec4> floor_attrs;

	// [{pos_x,pos_y} {scale_x,scale_y} {vel_x,vel_y}]
	std::vector<std::array<vec2,OBSTACLE_ATTRIBUTES>> obstacle_attrs;

	// pos_x, pos_y, scale_x, scale_y, moveable?
	std::vector<std::pair<vec4, Terrain>> terrains_attr;

	// pos_x, pos_y, vel_x, vel_y, scale_x, scale_y
	std::vector<std::pair<vec2, Enemy>> enemies_attr;

	// pos_x, pos_y, vel_x, vel_y, scale_x, scale_y
	std::vector<std::pair<vec2, Enemy>> bosses_attr;
	
	// pos_x, pos_y, vel_x, vel_y, scale_x, scale_y
	std::vector<std::pair<vec2, LostSoul>> lost_souls_attr;

	bool init(uint level);

	uint getCurrLevel() {
		return curr_level;
	}
	
	bool getIsCutscene() {
		return is_cutscene;
	}

	bool getIsBossLevel() {
		return is_boss_level;
	}

	bool getPowerUpNextLevel() {
		return power_up_next_level;
	}

	vec2& getPlayerStartingPos() {
		return player_starting_pos;
	}

	vec2& getExitDoorPos() {
		return exit_door_pos;
	}

	std::vector<vec2>& getHealthPackPos() {
		return health_packs_pos;
	}

	std::vector<std::string>& getTexts() {
		return texts;
	}

	std::vector<std::array<float, TEXT_ATTRIBUTES>>& getTextAttrs() {
		return text_attrs;
	}

	std::vector<vec4>& getFloorAttrs() {
		return floor_attrs;
	}

	std::vector<std::array<vec2,OBSTACLE_ATTRIBUTES>>& getObstacleAttrs() {
		return obstacle_attrs;
	}

	std::vector<std::pair<vec4, Terrain>>& getTerrains() {
		return terrains_attr;
	}

	std::vector<std::pair<vec2,Enemy>>& getEnemies() {
		return enemies_attr;
	}

	std::vector<std::pair<vec2, Enemy>>& getBosses() {
		return bosses_attr;
	}

	std::vector<std::pair<vec2, LostSoul>>& getLostSouls() {
		return lost_souls_attr;
	}

	int getLifeOrbPiece() { return life_orb_piece; }
};