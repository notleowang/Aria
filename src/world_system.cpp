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
const float PROJECTILE_SPEED = 700.f;

// Create the world
WorldSystem::WorldSystem() {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (projectile_sound != nullptr)
		Mix_FreeChunk(projectile_sound);
	if (aria_death_sound != nullptr)
		Mix_FreeChunk(aria_death_sound);
	if (enemy_death_sound != nullptr)
		Mix_FreeChunk(enemy_death_sound);
	if (damage_tick_sound != nullptr)
		Mix_FreeChunk(damage_tick_sound);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char* desc) {
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
	auto mouse_button_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button(_0, _1, _2); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_redirect);
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

	background_music = Mix_LoadMUS(audio_path("eerie_ambience.wav").c_str());
	projectile_sound = Mix_LoadWAV(audio_path("projectile.wav").c_str());
	aria_death_sound = Mix_LoadWAV(audio_path("aria_death.wav").c_str());
	enemy_death_sound = Mix_LoadWAV(audio_path("enemy_death.wav").c_str());
	damage_tick_sound = Mix_LoadWAV(audio_path("damage_tick.wav").c_str());

	if (background_music == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("eerie_ambience.wav").c_str());
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
	std::stringstream title_ss;
	title_ss << "Aria: Whispers of Darkness";
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());


	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];

	for (Entity entity : registry.invulnerableTimers.entities) {
		InvulnerableTimer& timer = registry.invulnerableTimers.get(entity);
		timer.timer_ms -= elapsed_ms_since_last_update;
		if (timer.timer_ms <= 0) {
			registry.invulnerableTimers.remove(entity);
		}
	}

    float min_timer_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
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
	// This might be overkill. Everything that has velocity should already have a position, etc.
	// Just being safe
	while (registry.positions.entities.size() > 0)
	    registry.remove_all_components_of(registry.positions.entities.back());
	while (registry.velocities.entities.size() > 0)
		registry.remove_all_components_of(registry.velocities.entities.back());
	while (registry.resources.entities.size() > 0)
		registry.remove_all_components_of(registry.resources.entities.back());
	while(registry.collidables.entities.size() > 0)
		registry.remove_all_components_of(registry.collidables.entities.back());


	// Debugging for memory/component leaks
	registry.list_all_components();

	// Screen is currently 1200 x 800 (refer to common.hpp to change screen size)
	//player = createTestSalmon(renderer, this->player_starting_pos);
	player = createAria(renderer, this->player_starting_pos);

	for (uint i = 0; i < this->terrains_attrs.size(); i++) {
		vec4 terrain_i = this->terrains_attrs[i];
		createTerrain(renderer, vec2(terrain_i[0], terrain_i[1]), vec2(terrain_i[2], terrain_i[3]));
	}

	for (uint i = 0; i < this->enemies_attrs.size(); i++) {
		std::array<float, ENEMY_ATTRIBUTES> enemy_i = this->enemies_attrs[i];
		createEnemy(renderer, vec2(enemy_i[0], enemy_i[1]));
	}

	createExitDoor(renderer, this->exit_door_pos);
}

bool collidedLeft(Position& pos_i, Position& pos_j) 
{
	return (((pos_i.prev_position.x + abs(pos_i.scale.x / 2)) < (pos_j.position.x - abs(pos_j.scale.x / 2))) &&
		((pos_i.position.x + abs(pos_i.scale.x / 2)) >= (pos_j.position.x - abs(pos_j.scale.x/2))));
}

bool collidedRight(Position& pos_i, Position& pos_j) 
{
	return (((pos_i.prev_position.x - abs(pos_i.scale.x / 2)) >= (pos_j.position.x + abs(pos_j.scale.x / 2))) &&
		((pos_i.position.x - abs(pos_i.scale.x / 2)) < (pos_j.position.x + abs(pos_j.scale.x/2))));
}

bool collidedTop(Position& pos_i, Position& pos_j) 
{
	return (((pos_i.prev_position.y + abs(pos_i.scale.y / 2)) < (pos_j.position.y - abs(pos_j.scale.y / 2))) &&
		((pos_i.position.y + abs(pos_i.scale.y / 2)) >= (pos_j.position.y - abs(pos_j.scale.y/2))));
}

bool collidedBottom(Position& pos_i, Position& pos_j) 
{
	return (((pos_i.prev_position.y - abs(pos_i.scale.y / 2)) >= (pos_j.position.y + abs(pos_j.scale.y / 2))) &&
		((pos_i.position.y - abs(pos_i.scale.x / 2)) < (pos_j.position.x + abs(pos_j.scale.x/2))));
}

void WorldSystem::win_level() {
	printf("hooray you won the level\n");
	this->levelDone = true;
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
		if (registry.enemies.has(entity_other) && registry.players.has(entity)) {
			if (!registry.invulnerableTimers.has(entity)) {
				Mix_PlayChannel(-1, damage_tick_sound, 0);
				Resources& player_resource = registry.resources.get(entity);
				player_resource.health -= registry.enemies.get(entity_other).damage;
				printf("player hp: %f\n", player_resource.health);
				registry.invulnerableTimers.emplace(entity);
				if (player_resource.health <= 0) {
					registry.deathTimers.emplace(entity);
					registry.velocities.get(player).velocity = vec2(0.f, 0.f);
					Mix_PlayChannel(-1, aria_death_sound, 0);
				}
			}
		}

		// Checking Player - Terrain Collisions
		if (registry.players.has(entity) && registry.terrain.has(entity_other)) {
			Position& player_position = registry.positions.get(entity);
			Position& terrain_position = registry.positions.get(entity_other);
			if (collidedLeft(player_position, terrain_position) || collidedRight(player_position, terrain_position)) {
				player_position.position.x = player_position.prev_position.x;
			} else if (collidedTop(player_position, terrain_position) || collidedBottom(player_position, terrain_position)) {
				player_position.position.y = player_position.prev_position.y;
			}
			else { // Collided on diagonal, displace based on vector
				player_position.position += collisionsRegistry.components[i].displacement;
			}
		}
		
		// Checking Enemy - Terrain Collisions
		if (registry.enemies.has(entity) && registry.terrain.has(entity_other)) {
			Position& enemy_position = registry.positions.get(entity);
			Position& terrain_position = registry.positions.get(entity_other);
			if (collidedLeft(enemy_position, terrain_position) || collidedRight(enemy_position, terrain_position)) {
				enemy_position.position.x = enemy_position.prev_position.x;
			}
			else if (collidedTop(enemy_position, terrain_position) || collidedBottom(enemy_position, terrain_position)) {
				enemy_position.position.y = enemy_position.prev_position.y;
			}
			else { // Collided on diagonal, displace based on vector
				enemy_position.position += collisionsRegistry.components[i].displacement;
			}
		}

		// Checking Projectile - Enemy collisions
		if (registry.enemies.has(entity_other) && registry.projectiles.has(entity)) {
			Mix_PlayChannel(-1, damage_tick_sound, 0);
			Resources& enemy_resource = registry.resources.get(entity_other);
			enemy_resource.health -= registry.projectiles.get(entity).damage;
			printf("enemy hp: %f\n", enemy_resource.health);
			if (enemy_resource.health <= 0) {
				registry.remove_all_components_of(entity_other);
				Mix_PlayChannel(-1, enemy_death_sound, 0);
			}
			registry.remove_all_components_of(entity);
		}

		// Checking Projectile - Wall collisions
		if (registry.terrain.has(entity_other) && registry.projectiles.has(entity)) {
			registry.remove_all_components_of(entity);
		}

		// Checking Player - Exit Door collision
		if (registry.players.has(entity) && registry.exitDoors.has(entity_other)) {
			win_level();
		}
	}
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	if (registry.deathTimers.has(player)) { return; }
	Velocity& player_velocity = registry.velocities.get(player);
	Position& player_position = registry.positions.get(player);
	Direction& player_direction = registry.directions.get(player);

	// get states of each arrow key
	int state_up = glfwGetKey(window, GLFW_KEY_W);
	int state_down = glfwGetKey(window, GLFW_KEY_S);
	int state_left = glfwGetKey(window, GLFW_KEY_A);
	int state_right = glfwGetKey(window, GLFW_KEY_D);

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

	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
		Velocity vel = computeVelocity(PROJECTILE_SPEED, player_direction);
		vec2 proj_position = player_position.position;
		// TODO: need to figure out the most consistent way to find the middle of the sprites.
		switch (player_direction.direction) {
		case DIRECTION::N:
			proj_position = vec2(player_position.position.x, player_position.position.y - abs(player_position.scale.y / 2));
			break;
		case DIRECTION::NE:
		case DIRECTION::E:
		case DIRECTION::SE:
			proj_position = vec2(player_position.position.x + abs(player_position.scale.x / 2), player_position.position.y);
			break;
		case DIRECTION::S:
			proj_position = vec2(player_position.position.x , player_position.position.y + abs(player_position.scale.y/2));
			break;
		case DIRECTION::NW:
		case DIRECTION::W:
		case DIRECTION::SW:
			proj_position = vec2(player_position.position.x - abs(player_position.scale.x / 2), player_position.position.y);
			break;
		default:
			break;
		}
		Entity projectile = createProjectile(renderer, proj_position, vel.velocity);
		Mix_PlayChannel(-1, projectile_sound, 0);
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		restart_game();
	}

	// Debugging
	//if (key == GLFW_KEY_D) {
	//	if (action == GLFW_RELEASE)
	//		debugging.in_debug_mode = false;
	//	else
	//		debugging.in_debug_mode = true;
	//}
}

void WorldSystem::on_mouse_button(int button, int action, int mod) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		//getting cursor position
		glfwGetCursorPos(window, &xpos, &ypos);
		printf("Cursor Position at (%f, %f)\n", xpos, ypos);

		Position& position = registry.positions.get(player);

		// calculate angle
		float deltaX = xpos - (window_width_px / 2);
		float deltaY = ypos - (window_height_px / 2);
		float angle = atan2(deltaY, deltaX);


		printf("angle: %f\n", angle);
		Velocity vel = computeVelocity(PROJECTILE_SPEED, angle);
		vec2 proj_position = position.position;
		Entity projectile = createProjectile(renderer, proj_position, vel.velocity);
		Mix_PlayChannel(-1, projectile_sound, 0);
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	// We are probably not going to need this on_mouse_move
	
	(vec2)mouse_position; // dummy to avoid compiler warning
}
