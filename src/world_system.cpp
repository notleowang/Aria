// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "utils.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"

// Game configuration
const float PLAYER_SPEED = 300.f;

// Create the world
WorldSystem::WorldSystem() {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, "Aria", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	//salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav").c_str()); // keeping one so we know how to load future wavs

	if (background_music == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg, GameLevel level) {
	this->renderer = renderer_arg;
	this->player_starting_pos = level.getPlayerStartingPos();
	this->exit_door_pos = level.getExitDoorPos();
	this->terrains_attrs = level.getTerrains();
	this->enemies_attrs = level.getEnemies();
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	// Set all states to default
    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Aria: Whispers of Darkness";
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());


	// Processing the salmon state
	assert(registry.screenStates.components.size() <= 1);
    ScreenState &screen = registry.screenStates.components[0];

	for (Entity entity : registry.invulnerableTimers.entities) {
		// progress timer
		InvulnerableTimer& timer = registry.invulnerableTimers.get(entity);
		timer.timer_ms -= elapsed_ms_since_last_update;
		if (timer.timer_ms <= 0) {
			registry.invulnerableTimers.remove(entity);
		}
	}

    float min_timer_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
		// progress timer
		DeathTimer& timer = registry.deathTimers.get(entity);
		timer.timer_ms -= elapsed_ms_since_last_update;
		if (timer.timer_ms < min_timer_ms) {
			min_timer_ms = timer.timer_ms;
		}
		// restart the game once the death timer expired
		if (timer.timer_ms < 0) {
			registry.deathTimers.remove(entity);
			screen.screen_darken_factor = 0;
			restart_game();
			return true;
		}
	}
	screen.screen_darken_factor = 1 - min_timer_ms / 3000;
	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// !!!
	// Remove all entities that we created
	while (registry.positions.entities.size() > 0)
	    registry.remove_all_components_of(registry.positions.entities.back());
	while (registry.velocities.entities.size() > 0)
		registry.remove_all_components_of(registry.velocities.entities.back());
	while (registry.resources.entities.size() > 0)
		registry.remove_all_components_of(registry.resources.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	// Screen is currently 1200 x 800 (refer to common.hpp to change screen size)
	player = createTestSalmon(renderer, this->player_starting_pos);

	createExitDoor(this->exit_door_pos);

	for (uint i = 0; i < this->terrains_attrs.size(); i++) {
		vec4 terrain_i = this->terrains_attrs[i];
		createTerrain(vec2(terrain_i[0], terrain_i[1]), vec2(terrain_i[2], terrain_i[3]));
	}

	for (uint i = 0; i < this->enemies_attrs.size(); i++) {
		std::array<float, 6> enemy_i = this->enemies_attrs[i];
		createEnemy(renderer, vec2(enemy_i[0], enemy_i[1]));
	}


	//registry.colors.insert(player, { 1, 0.8f, 0.8f });

	/*
	// Create a new player component
	player = createPlayer(renderer, { 100, 200 });
	registry.colors.insert(player_salmon, {1, 0.8f, 0.8f});

	for (uint i = 0; i < 20; i++) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		float radius = 30 * (uniform_dist(rng) + 0.3f); // range 0.3 .. 1.3
		Entity pebble = createPebble({ uniform_dist(rng) * w, h - uniform_dist(rng) * 20 }, 
			         { radius, radius });
		float brightness = uniform_dist(rng) * 0.5 + 0.5;
		registry.colors.insert(pebble, { brightness, brightness, brightness});
	}
	*/
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	if (registry.deathTimers.has(player)) { return; }
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other_entity;

		// Checking Player - Enemy collisions
		if (registry.enemies.has(entity_other)) {
			if (!registry.invulnerableTimers.has(entity)) {
				Resources& player_resource = registry.resources.get(entity);
				player_resource.health -= registry.enemies.get(entity_other).damage;
				printf("player hp: %f\n", player_resource.health);
				registry.invulnerableTimers.emplace(entity);
				if (player_resource.health <= 0) {
					registry.deathTimers.emplace(entity);
					registry.velocities.get(player).velocity = vec2(0.f, 0.f);
					// TODO: play death sound here
				}
			}
		}
	}
	registry.collisions.clear();
			//if (registry.hardShells.has(entity_other)) {
	//			// initiate death unless already dying
	//			if (!registry.deathTimers.has(entity)) {
	//				// Scream, reset timer, and make the salmon sink
	//				registry.deathTimers.emplace(entity);

	//				// !!! TODO A1: change the salmon orientation and color on death
	//			}
	//		}
	//		// Checking Player - SoftShell collisions
	//		else if (registry.softShells.has(entity_other)) {
	//			if (!registry.deathTimers.has(entity)) {
	//				// chew, count points, and set the LightUp timer
	//				registry.remove_all_components_of(entity_other);
	//				Mix_PlayChannel(-1, salmon_eat_sound, 0);
	//				++points;
	//			}
	//		}
	//	}

}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	if (registry.deathTimers.has(player)) { return; }
	// TODO: solve issue where player is faster on the diagonals
	Velocity& player_velocity = registry.velocities.get(player);
	Direction& player_direction = registry.directions.get(player);

	// get states of each arrow key
	int state_up = glfwGetKey(window, GLFW_KEY_UP);
	int state_down = glfwGetKey(window, GLFW_KEY_DOWN);
	int state_left = glfwGetKey(window, GLFW_KEY_LEFT);
	int state_right = glfwGetKey(window, GLFW_KEY_RIGHT);

	DIRECTION new_direction = DIRECTION::NONE;

	// up
	if ((state_up == GLFW_PRESS && state_down == GLFW_RELEASE && state_left == GLFW_RELEASE && state_right == GLFW_RELEASE) ||
		(state_up == GLFW_PRESS && state_down == GLFW_RELEASE && state_left == GLFW_PRESS && state_right == GLFW_PRESS)) {
		new_direction = DIRECTION::N;
	}
	// down
	else if ((state_down == GLFW_PRESS && state_up == GLFW_RELEASE && state_left == GLFW_RELEASE && state_right == GLFW_RELEASE) ||
		(state_down == GLFW_PRESS && state_up == GLFW_RELEASE && state_left == GLFW_PRESS && state_right == GLFW_PRESS)) {
		new_direction = DIRECTION::S;
	}
	// left
	else if ((state_left == GLFW_PRESS && state_down == GLFW_RELEASE && state_up == GLFW_RELEASE && state_right == GLFW_RELEASE) ||
		(state_left == GLFW_PRESS && state_down == GLFW_PRESS && state_up == GLFW_PRESS && state_right == GLFW_RELEASE)) {
		new_direction = DIRECTION::W;
	}
	// right
	else if ((state_right == GLFW_PRESS && state_down == GLFW_RELEASE && state_left == GLFW_RELEASE && state_up == GLFW_RELEASE) ||
		(state_right == GLFW_PRESS && state_down == GLFW_PRESS && state_left == GLFW_RELEASE && state_up == GLFW_PRESS)) {
		new_direction = DIRECTION::E;
	}
	// up and left
	else if ((state_up == GLFW_PRESS && state_down == GLFW_RELEASE && state_left == GLFW_PRESS && state_right == GLFW_RELEASE)) {
		new_direction = DIRECTION::NW;
	}
	// up and right
	else if ((state_up == GLFW_PRESS && state_down == GLFW_RELEASE && state_left == GLFW_RELEASE && state_right == GLFW_PRESS)) {
		new_direction = DIRECTION::NE;
	}
	// down and right
	else if ((state_up == GLFW_RELEASE && state_down == GLFW_PRESS && state_left == GLFW_PRESS && state_right == GLFW_RELEASE)) {
		new_direction = DIRECTION::SW;
	}
	// down and left
	else if ((state_up == GLFW_RELEASE && state_down == GLFW_PRESS && state_left == GLFW_RELEASE && state_right == GLFW_PRESS)) {
		new_direction = DIRECTION::SE;
	}

	// set the player velocity based on the new_direction
	if (new_direction != DIRECTION::NONE) {
		player_direction.direction = new_direction;
		player_velocity = computeVelocity(PLAYER_SPEED, player_direction);
	}
	else {
		player_velocity = computeVelocity(0.0, player_direction);
	}

	// Resetting game (currently disabled, think about adding this back in later)
	//if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
	//	int w, h;
	//	glfwGetWindowSize(window, &w, &h);

	//  restart_game();
	//}

	// Debugging
	//if (key == GLFW_KEY_D) {
	//	if (action == GLFW_RELEASE)
	//		debugging.in_debug_mode = false;
	//	else
	//		debugging.in_debug_mode = true;
	//}
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	// We are probably not going to need this on_mouse_move

	(vec2)mouse_position; // dummy to avoid compiler warning
}
