#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "game_level.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer, GameLevel level);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;

	void win_level();
	void power_up_menu();
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_button(int button, int action, int mod);
	void on_mouse_move(vec2 pos);

	// restart level
	void restart_game();

	bool levelDone;

	// OpenGL window handle
	GLFWwindow* window;

	// Game state
	RenderSystem* renderer;
	Entity player;
	
	vec2 player_starting_pos;
	vec2 exit_door_pos;
	std::vector<vec2> floor_pos;
	std::vector<vec4> terrains_attrs;
	std::vector<std::array<float, ENEMY_ATTRIBUTES>> enemies_attrs;

	// music references
	Mix_Music* background_music; // TODO: change background music for our game
	Mix_Chunk* projectile_sound;
	Mix_Chunk* aria_death_sound;
	Mix_Chunk* enemy_death_sound;
	Mix_Chunk* damage_tick_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
