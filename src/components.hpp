#pragma once
#include "common.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"
using namespace std;

// Aria component
struct Player
{

};

// All data relevant to elements and weaknesses
enum ElementType {
	WATER = 0,
	FIRE = 1,
	EARTH = 2,
	LIGHTNING = 3,
	COUNT = 4,
	COMBO = 5 // this is reserved for the final boss only
};

// Enemy component
struct Enemy
{
	float damage = 10.f;
	float movementTimer = 3000.f;
	float stamina = 0.5f;
	float mana = 1.f;
	ElementType type = ElementType::FIRE; // By default, an enemy is of fire type
	float isAggravated = true;
};

// hooded guy
struct LostSoul {

};

struct Cutscene {
	bool is_cutscene_6 = false;
};

// life orb
struct LifeOrb {
	bool centered_on_screen = false;
};

// Boss
struct Boss {
	int phase = 0;
	int subphase = 0;
	float phaseTimer = 250.f;
};

// Obstacles
struct Obstacle
{

};

// all data relevant to the player's power ups
struct PowerUp {
	bool fasterMovement;
	bool increasedDamage[4]; // array size == number of element types
	bool tripleShot[4];
	bool bounceOffWalls[4];
};

// all data related to power up block in the power up level
struct PowerUpBlock {
	string powerUpText;
	bool* powerUpToggle;
	Entity textEntity;
};

// Health pack
struct HealthPack
{
	float value = 20;
};

// Shadow of the owner entity
struct Shadow
{
	Entity owner;
	bool active;
	vec2 original_size;
};

// Exit door
struct ExitDoor
{

};

// Text component
struct Text
{
	std::string text;
	vec3 color;
};

// All data relevant to the resources of entities
struct Resources
{
	float maxHealth = 100.f;
	float currentHealth = 100.f;
	float maxMana = 10.f;
	float currentMana = 10.f;
	float logoRatio = 0.f;
	float barRatio = 1.f;
	Entity healthBar;
	Entity manaBar;
};

struct HealthBar
{
	Entity owner;
};

struct ManaBar
{
	Entity owner;
};

struct ProjectileSelectDisplay
{
	Entity fasterMovement;
	Entity increasedDamage[4];
	Entity tripleShot[4];
	Entity bounceOffWalls[4];
};

struct PowerUpIndicator
{

};

struct Follower
{
	Entity owner;
	float y_offset = 0.f;
	float x_offset = 0.f;
};

struct SecondaryFollower
{
	Entity owner;
	float y_offset = 0.f;
	float x_offset = 0.f;
};

// Structure to store projectile entities
struct Projectile
{
	float damage = 10.f;
	ElementType type;
	bool hostile = false;
	int bounces;
};

struct CharacterProjectileType
{
	ElementType projectileType = ElementType::WATER; //By default, the characters projectile type is water
	char foo; // just a random extra data type added in the struct to make it a "complete" definition (otherwise compiler throws an error)
};

// All data relevant to the position of entities
struct Position {
	vec2 position = { 0.f, 0.f };
	float angle = 0.f;
	vec2 scale = { 10.f, 10.f };
	vec2 prev_position = { 0.f, 0.f };
};

// Data relevant to velocity of entities
struct Velocity {
	vec2 velocity = { 0.f, 0.f };
};

struct Floor {

};


// Data relevant to direction of entities
typedef enum {
	N,
	NE,
	E,
	SE,
	S,
	SW,
	W,
	NW,
	NONE
} DIRECTION;

struct Direction {
	DIRECTION direction;
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other_entity; // the second object involved in the collision
	vec2 displacement;
	Collision(Entity& other_entity, vec2 displacement) { 
		this->other_entity = other_entity;
		this->displacement = displacement;
	};
};

// Terrain
struct Terrain
{
	DIRECTION direction = DIRECTION::N;
	float speed = 0.f;
	bool moveable = false;
};

// Component container that marks an entity as being collidable
// Will be: players, enemies, terrain, projectiles, etc.
struct Collidable
{

};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float screen_darken_factor = -1;
	bool apply_spotlight = false;
	float spotlight_radius = 0.0f;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// A timer that will be associated to an entity having an invulnerability period to damage
struct InvulnerableTimer
{
	float timer_ms = 1000.f;
};

// A timer that will be associated to an entity dying
struct DeathTimer
{
	float timer_ms = 2700.f;
};

// Timer that signifies level change
struct WinTimer
{
	float timer_ms = 3600.f;
	bool changedLevel = false;
};

struct WeaknessTimer
{
	float timer_ms = 3000.f;
	ElementType weakTo = ElementType::FIRE;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

struct AnimState
{
	int first;
	int last;
	int getNumFrames();
	int getNextFrame(int curr_frame);
	AnimState() = default;
	AnimState(int first, int last) {
		this->first = first;
		this->last = last;
	}
};

struct SpriteSheet
{
	std::vector<AnimState> states;
	int num_rows;
	int num_cols;
	float frame_height;
	float frame_width;
	vec2 getFrameSizeInTexcoords();
	int getNumFrames();
	static int getPlayerStateFromDirection(DIRECTION dir);
	static bool getPlayerMirrored(DIRECTION dir);
};

struct Animation
{
	SpriteSheet* sprite_sheet_ptr;
	int curr_state_index = 0;
	int curr_frame = 0;
	bool is_animating = true;
	bool rainbow_enabled = false;
	int getColumn();
	int getRow();
	void advanceFrame();
	void advanceState();
	void setState(int new_state_index);
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	NORTH_TERRAIN,
	SOUTH_TERRAIN,
	SIDE_TERRAIN,
	GENERIC_TERRAIN,
	WATER_ENEMY,
	FIRE_ENEMY,
	EARTH_ENEMY,
	LIGHTNING_ENEMY,
	WATER_BOSS,
	FIRE_BOSS,
	EARTH_BOSS,
	LIGHTNING_BOSS,
	EARTH_BOSS_SHEET,
	FINAL_BOSS,
	FINAL_BOSS_SHADOW,
	GHOST,
	LIFE_ORB,
	LIFE_ORB_PIECE_1,
	LIFE_ORB_PIECE_2,
	LIFE_ORB_PIECE_3,
	WATER_PROJECTILE_SHEET,
	FIRE_PROJECTILE_SHEET,
	EARTH_PROJECTILE_SHEET,
	LIGHTNING_PROJECTILE_SHEET,
	FLOOR,
	BOSS_HEALTH_BAR,
	ENEMY_HEALTH_BAR,
	ENEMY_MANA_BAR,
	PLAYER_HEALTH_BAR,
	PLAYER_MANA_BAR,
	POWER_UP_BLOCK,
	PLAYER,
	PORTAL,
	PROJECTILE_SELECT_DISPLAY,
	FASTER_MOVEMENT,
	DAMAGE_ARROW,
	TRIPLE_SHOT,
	BOUNCE,
	HEALTH_PACK,
	LOST_SOUL,
	TEXTURE_COUNT
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	PLAYER = 0,
	COLOURED = PLAYER + 1,
	SALMON = COLOURED + 1,
	TEXTURED = SALMON + 1,
	DARKEN = TEXTURED + 1,
	REPEAT = DARKEN + 1,
	EXIT_DOOR = REPEAT + 1,
	RESOURCE_BAR = EXIT_DOOR + 1,
	TEXT_2D = RESOURCE_BAR + 1,
	ANIMATED = TEXT_2D + 1,
	SHADOW = ANIMATED + 1,
	EFFECT_COUNT = SHADOW + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SALMON,
	SPRITE,
	SMALL_WATER_ENEMY,
	SMALL_FIRE_ENEMY,
	SMALL_EARTH_ENEMY,
	SMALL_LIGHTNING_ENEMY,
	DEBUG_LINE,
	SCREEN_TRIANGLE,
	TERRAIN,
	EXIT_DOOR,
	TEXT_2D,
	RESOURCE_BAR,
	WATER_PROJECTILE,
	FIRE_PROJECTILE,
	EARTH_PROJECTILE_SHEET,
	LIGHTNING_PROJECTILE_SHEET,
	POWER_UP_BLOCK,
	PLAYER,
	BOSS,
	FINAL_BOSS,
	PROJECTILE_SELECT_DISPLAY,
	GEOMETRY_COUNT
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

enum class SPRITE_SHEET_DATA_ID {
	NONE,
	WATER_PROJECTILE,
	FIRE_PROJECTILE,
	EARTH_PROJECTILE_SHEET,
	LIGHTNING_PROJECTILE_SHEET,
	POWER_UP_BLOCK,
	PLAYER,
	BOSS,
	FINAL_BOSS,
	PROJECTILE_SELECT_DISPLAY,
	SPRITE_SHEET_COUNT
};
const int sprite_sheet_count = (int)SPRITE_SHEET_DATA_ID::SPRITE_SHEET_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

// One for each sprite sheet to indicate the states
enum class POWER_UP_BLOCK_STATES {
	ACTIVE = 0,
	INACTIVE = ACTIVE + 1,
	STATE_COUNT = INACTIVE + 1
};

enum class PROJECTILE_STATES {
	MOVING = 0,
	STATE_COUNT = MOVING + 1
};

enum class PLAYER_SPRITE_STATES {
	WEST,
	EAST,
	NORTH_WEST,
	NORTH_EAST,
	NORTH,
	SOUTH,
	STATE_COUNT
};

enum class BOSS_STATES {
	STANDING,
	STATE_COUNT
};

enum class FINAL_BOSS_SPRITE_STATES {
	EAST,
	WEST,
	WATER,
	EARTH,
	FIRE,
	LIGHTNING,
	STATE_COUNT
};