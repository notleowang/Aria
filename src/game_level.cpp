#include "game_level.hpp" 
#include <random>
#include <tiny_ecs_registry.hpp>

/*
	CREATING WALLS:
		Push back a pair to terrains:
			make_pair(vec4(x_coord, y_coord, length, width), Terrain)))
			Honestly, implementation is a bit messy right now (could think of refactoring)

	CREATING FLOORS:
		Push back a vec2 to floors:
			vec2(x_coord, y_coord)

	CREATING ENEMIES:
		Push back a pair of pos and enemy attributes to enemies:
			make_pair(vec2(x_coord, y_coord), ENEMY_OBJECT);

	CREATING HEALTH PACKS:
		Push back a vec2 to health_packs_pos
			vec2(x_coord, y_coord)
*/

// Helper function to get a random normal damage enemy
const Enemy& getRandomNormalEnemy() {
	static const std::vector<Enemy> normalEnemies = { WATER_NORMAL, FIRE_NORMAL, EARTH_NORMAL, LIGHTNING_NORMAL };

	// Use C++11 random number generation
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distribution(0, normalEnemies.size() - 1);

	return normalEnemies[distribution(gen)];
}

bool GameLevel::init(uint level) {
	printf("Initializing game for level: %i\n", level);
	if (level < 0) {
		return false;
	}
	this->curr_level = level;
	this->is_cutscene = false;
	this->is_boss_level = false;

	std::vector<vec2>& health_packs_pos = this->health_packs_pos;
	std::vector<std::string>& texts = this->texts;
	std::vector<std::array<float, TEXT_ATTRIBUTES>>& text_attrs = this->text_attrs;
	std::vector<std::array< vec2, OBSTACLE_ATTRIBUTES>>& obstacles = this->obstacle_attrs;
	std::vector<vec4>& floors = this->floor_attrs;
	std::vector<std::pair<vec4, Terrain>>& terrains = this->terrains_attr;
	std::vector<std::pair<vec2, Enemy>>& enemies = this->enemies_attr;
	std::vector<std::pair<vec2, Enemy>>& bosses = this->bosses_attr;
	std::vector<std::pair<vec2, LostSoul>>& lost_souls = this->lost_souls_attr;

	health_packs_pos.clear();
	texts.clear();
	text_attrs.clear();
	floors.clear();
	terrains.clear();
	enemies.clear();
	obstacles.clear();
	bosses.clear();
	lost_souls.clear();

	switch (level) {
	case TUTORIAL:
		floors.push_back(vec4(25, 30, 1300, 650));

		this->player_starting_pos = vec2(200, 200);
		this->exit_door_pos = vec2(1225, 575);

		health_packs_pos.push_back(vec2(400, 300));
		health_packs_pos.push_back(vec2(600, 400));
		health_packs_pos.push_back(vec2(800, 500));
		health_packs_pos.push_back(vec2(1000, 600));

		texts.push_back("Use WASD to move around");
		text_attrs.push_back({0.f,225.f,1.0f,1.0f,1.0f,0.f});
		texts.push_back("Use M1 button to shoot");
		text_attrs.push_back({0.f,175.f,1.0f,1.0f,1.0f,0.f});
		texts.push_back("Use 1,2,3,4 to cycle through elements");
		text_attrs.push_back({0.f,125.f,1.0f,1.0f,1.0f,0.f});		
		texts.push_back("Collect health packs to heal!");
		text_attrs.push_back({0.f,75.f,1.0f,1.0f,1.0f,0.f});		
		texts.push_back("Move to the exit door when ready");
		text_attrs.push_back({0.f,25.f,1.0f,1.0f,1.0f,0.f});


		terrains.push_back(std::make_pair(vec4(25, 0, 1300, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 675, 1300, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 700), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(1325, 0, default_side_width, 700), SIDE_STATIONARY));
		break;

	case CUTSCENE_1:
		this->is_cutscene = true;
		floors.push_back(vec4(25, 25, 10000, 400));

		this->player_starting_pos = vec2(50, 300);
		this->exit_door_pos = vec2(9300,200);
		this->cutscene_player_velocity = { 300.f,0.f };

		lost_souls.push_back({ vec2(5800, 300), LostSoul()});

		terrains.push_back(std::make_pair(vec4(25, 0, 10000, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 400, 10000, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 425), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(10025, 0, default_side_width, 425), SIDE_STATIONARY));
		
		break;
	case LEVEL_1:
		this->player_starting_pos = vec2(200, 700);
		this->exit_door_pos = vec2(1450, 775);

		floors.push_back(vec4(25, 25, 1800, 875));

		// I think we should make a helper for pushing back into terrains vector, thoughts? - Leo
		terrains.push_back(std::make_pair(vec4(25, 0, 1800, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 875, 1800, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 900), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(1825, 0, default_side_width, 900), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(350, 400, default_side_width, 475), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(750, 0, default_side_width, 475), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(1150, 400, default_side_width, 475), SIDE_STATIONARY));

		enemies.push_back(std::make_pair(vec2(350, 250), getRandomNormalEnemy()));
		enemies.push_back(std::make_pair(vec2(700, 800), getRandomNormalEnemy()));
		enemies.push_back(std::make_pair(vec2(950, 800), getRandomNormalEnemy()));
		enemies.push_back(std::make_pair(vec2(1100, 250), FIRE_NORMAL)); // last enemy must be fire
		break;
	
	case FIRE_BOSS:
		this->is_boss_level = true;
		this->life_orb_piece = NULL;

		floors.push_back(vec4(25, 25, 2700, 1375));

		this->player_starting_pos = vec2(800, 650);
		this->exit_door_pos = NULL_POS;

		terrains.push_back(std::make_pair(vec4(25, 0, 2700, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 1375, 2700, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 1400), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(2725, 0, default_side_width, 1400), SIDE_STATIONARY));

		bosses.push_back(std::make_pair(vec2(1400, 700), FIRE_HIGH_DAMAGE));
		break;

	case CUTSCENE_2:
		this->is_cutscene = true;


		terrains.push_back(std::make_pair(vec4(25, 0, 3000, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 400, 2900, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 0, default_side_width, 400), SIDE_STATIONARY));
		floors.push_back(vec4(25, 25, 3000, 400));

		terrains.push_back(std::make_pair(vec4(2900, 400, default_side_width, 2200), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(3000, 0, default_side_width, 1000), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(3000, 1200, default_side_width, 1600), SIDE_STATIONARY));

		floors.push_back(vec4(2900, 400, 100, 2400));

		//extra room
		terrains.push_back(std::make_pair(vec4(3000, 1000, 1000, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(3000, 1200, 1000, default_south_height), SOUTH_STATIONARY));
		floors.push_back(vec4(3000, 1000, 1000, 200));


		terrains.push_back(std::make_pair(vec4(25, 2500, 2900, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 2800, 3000, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 2500, default_side_width, 300), SIDE_STATIONARY));
		floors.push_back(vec4(25, 2500, 3000, 300));

		this->player_starting_pos = vec2(25, 300);
		//POSSIBLE MEMORY LEAK IN LOSTSOUL()
		lost_souls.push_back({ vec2(200, 200), LostSoul()});

		this->exit_door_pos = vec2(125, 2650);
		this->cutscene_player_velocity = { 220,0.f };

		// add a bunch of enemies outside the map walls
		// above top
		for (int i = 25; i <= 2800; i += 100) {
			Enemy enemy = getRandomNormalEnemy();
			enemy.isAggravated = false;
			enemies.push_back(std::make_pair(vec2(i, -50), enemy));
		}
		// under top
		for (int i = 25; i <= 2800; i += 100) {
			Enemy enemy = getRandomNormalEnemy();
			enemy.isAggravated = false;
			enemies.push_back(std::make_pair(vec2(i, 500), enemy));
		}
		// far right
		for (int i = 0; i <= 900; i += 100) {
			Enemy enemy = getRandomNormalEnemy();
			enemy.isAggravated = false;
			enemies.push_back(std::make_pair(vec2(3050, i), enemy));
		}
		for (int i = 1300; i <= 2800; i += 100) {
			Enemy enemy = getRandomNormalEnemy();
			enemy.isAggravated = false;
			enemies.push_back(std::make_pair(vec2(3050, i), enemy));
		}
		// inside right
		for (int i = 700; i <= 2600; i += 100) {
			Enemy enemy = getRandomNormalEnemy();
			enemy.isAggravated = false;
			enemies.push_back(std::make_pair(vec2(2850, i), enemy));
		}
		//// above bottom
		for (int i = 100; i <= 2000; i += 100) {
			Enemy enemy = getRandomNormalEnemy();
			enemy.isAggravated = false;
			enemies.push_back(std::make_pair(vec2(i, 2500), enemy));
		}
		// under bottom
		for (int i = 100; i <= 2000; i += 100) {
			Enemy enemy = getRandomNormalEnemy();
			enemy.isAggravated = false;
			enemies.push_back(std::make_pair(vec2(i, 2900), enemy));
		}

		break;
	
	case LEVEL_2: // Same as level 1 but with moving walls
		this->player_starting_pos = vec2(200, 700);

		texts.push_back("Don't get haunted by the ghosts!");
		text_attrs.push_back({ 0.f,80.f,1.0f,1.0f,1.0f,0.f });
		
		obstacles.push_back({ vec2(150, 450), vec2(80,80), vec2(100.f,0.f) });

		floors.push_back(vec4(25, 25, 1800, 875));

		terrains.push_back(std::make_pair(vec4(25, 0, 1800, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 875, 1800, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 900), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(1825, 0, default_side_width, 900), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(350, 400, default_side_width, 475), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(750, 0, default_side_width, 475), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(1150, 400, default_side_width, 475), SIDE_STATIONARY));

		terrains.push_back(std::make_pair(vec4(525, 450, 50, 50), GENERIC_MOVABLE));
		terrains.push_back(std::make_pair(vec4(775, 450, 50, 50), GENERIC_MOVABLE));
		terrains.push_back(std::make_pair(vec4(1125, 450, 50, 50), GENERIC_MOVABLE));


		enemies.push_back(std::make_pair(vec2(350, 250), getRandomNormalEnemy()));
		enemies.push_back(std::make_pair(vec2(700, 800), getRandomNormalEnemy()));
		enemies.push_back(std::make_pair(vec2(950, 800), getRandomNormalEnemy()));
		enemies.push_back(std::make_pair(vec2(1100, 250), EARTH_NORMAL)); // last enemy must be earth

		this->exit_door_pos = vec2(1450, 775);
		break;

	case EARTH_BOSS:
		this->is_boss_level = true;
		this->life_orb_piece = 1;
		floors.push_back(vec4(25, 25, 2700, 1375));

		this->player_starting_pos = vec2(800, 650);
		this->exit_door_pos = NULL_POS;

		terrains.push_back(std::make_pair(vec4(25, 0, 2700, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 1375, 2700, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 1400), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(2725, 0, default_side_width, 1400), SIDE_STATIONARY));

		bosses.push_back(std::make_pair(vec2(1400, 700), EARTH_HIGH_DAMAGE));
		break;

	case CUTSCENE_3:
		this->is_cutscene = true;
		floors.push_back(vec4(25, 25, 5000, 400));

		this->player_starting_pos = vec2(50, 300);
		this->exit_door_pos = vec2(4800, 200);
		this->cutscene_player_velocity = { 300.f,0.f };

		lost_souls.push_back({ vec2(200, 300), LostSoul() });

		terrains.push_back(std::make_pair(vec4(25, 0, 5000, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 400, 5000, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 425), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(5025, 0, default_side_width, 425), SIDE_STATIONARY));

		break;

	case LEVEL_3: // Same as level 1 but with moving walls
		this->player_starting_pos = vec2(200, 700);

		texts.push_back("Don't get haunted by the ghosts!");
		text_attrs.push_back({ 0.f,80.f,1.0f,1.0f,1.0f,0.f });
		
		floors.push_back(vec4(25, 25, 1800, 875));

		obstacles.push_back({ vec2(150, 450), vec2(80,80), vec2(100.f,0.f) });

		terrains.push_back(std::make_pair(vec4(25, 0, 1800, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 875, 1800, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 900), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(1825, 0, default_side_width, 900), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(350, 400, default_side_width, 475), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(750, 0, default_side_width, 475), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(1150, 400, default_side_width, 475), SIDE_STATIONARY));

		terrains.push_back(std::make_pair(vec4(525, 450, 50, 50), GENERIC_MOVABLE));
		terrains.push_back(std::make_pair(vec4(775, 450, 50, 50), GENERIC_MOVABLE));
		terrains.push_back(std::make_pair(vec4(1125, 450, 50, 50), GENERIC_MOVABLE));

		enemies.push_back(std::make_pair(vec2(350, 250), getRandomNormalEnemy()));
		enemies.push_back(std::make_pair(vec2(700, 800), getRandomNormalEnemy()));
		enemies.push_back(std::make_pair(vec2(950, 800), getRandomNormalEnemy()));
		enemies.push_back(std::make_pair(vec2(1100, 250), LIGHTNING_NORMAL)); // last enemy must be lightning


		this->exit_door_pos = vec2(1450, 775);
		break;

	case LIGHTNING_BOSS:
		this->is_boss_level = true;
		this->life_orb_piece = 2;
		floors.push_back(vec4(25, 25, 2700, 1375));

		this->player_starting_pos = vec2(800, 650);
		this->exit_door_pos = NULL_POS;

		terrains.push_back(std::make_pair(vec4(25, 0, 2700, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 1375, 2700, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 1400), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(2725, 0, default_side_width, 1400), SIDE_STATIONARY));

		bosses.push_back(std::make_pair(vec2(1400, 700), LIGHTNING_HIGH_DAMAGE));
		break;

	case CUTSCENE_4:
		this->is_cutscene = true;
		floors.push_back(vec4(25, 25, 5000, 400));

		this->player_starting_pos = vec2(50, 300);
		this->exit_door_pos = vec2(4800, 200);
		this->cutscene_player_velocity = { 300.f,0.f };

		lost_souls.push_back({ vec2(200, 300), LostSoul() });

		terrains.push_back(std::make_pair(vec4(25, 0, 5000, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 400, 5000, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 425), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(5025, 0, default_side_width, 425), SIDE_STATIONARY));

		break;

	case LEVEL_4:
		this->player_starting_pos = vec2(200, 200);
		this->exit_door_pos = vec2(3900, 1875);

		floors.push_back(vec4(0, 0, 4000, 2000));

		//Surrounding box walls
		terrains.push_back(std::make_pair(vec4(0, -100, 4000, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 1975, 4000, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(-25, -100, default_side_width, 2100), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(4000, -100, default_side_width, 2100), SIDE_STATIONARY));

		//inner walls
		terrains.push_back(std::make_pair(vec4(0, 700, 1000, 1300), GENERIC_STATIONARY));
		terrains.push_back(std::make_pair(vec4(1000, 900, 700, 1100), GENERIC_STATIONARY));
		terrains.push_back(std::make_pair(vec4(1700, 0, 500, 400), GENERIC_STATIONARY));
		terrains.push_back(std::make_pair(vec4(1700, 700, 1600, 400), GENERIC_STATIONARY));
		terrains.push_back(std::make_pair(vec4(2200, 1400, 1800, 100), NORTH_STATIONARY));

		terrains.push_back(std::make_pair(vec4(3800, 700, 75, 75), GENERIC_MOVABLE));
		terrains.push_back(std::make_pair(vec4(3600, 800, 75, 75), GENERIC_MOVABLE));
		terrains.push_back(std::make_pair(vec4(3400, 900, 75, 75), GENERIC_MOVABLE));

		// Area 1
		enemies.push_back(std::make_pair(vec2(1200, 700), getRandomNormalEnemy()));
		enemies.push_back(std::make_pair(vec2(1500, 500), getRandomNormalEnemy()));

		// Area 2
		enemies.push_back(std::make_pair(vec2(2400, 200), WATER_NORMAL));
		enemies.push_back(std::make_pair(vec2(3200, 200), FIRE_NORMAL));
		enemies.push_back(std::make_pair(vec2(2800, 500), EARTH_NORMAL));
		enemies.push_back(std::make_pair(vec2(3600, 500), LIGHTNING_NORMAL));

		// Area 3
		enemies.push_back(std::make_pair(vec2(2400, 1250), EARTH_NORMAL));
		enemies.push_back(std::make_pair(vec2(3000, 1250), EARTH_NORMAL));

		// Area 4
		enemies.push_back(std::make_pair(vec2(3200, 1600), WATER_NORMAL));	// final group of water enemies before "final boss"
		enemies.push_back(std::make_pair(vec2(3200, 1700), WATER_NORMAL));
		enemies.push_back(std::make_pair(vec2(3200, 1800), WATER_NORMAL));
		enemies.push_back(std::make_pair(vec2(3200, 1900), WATER_NORMAL));

		break;


	case WATER_BOSS:
		this->is_boss_level = true;
		this->life_orb_piece = 3;
		floors.push_back(vec4(25, 25, 2700, 1375));

		this->player_starting_pos = vec2(800, 650);
		this->exit_door_pos = NULL_POS;

		terrains.push_back(std::make_pair(vec4(25, 0, 2700, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 1375, 2700, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 1400), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(2725, 0, default_side_width, 1400), SIDE_STATIONARY));

		bosses.push_back(std::make_pair(vec2(1400, 700), WATER_HIGH_DAMAGE));
		break;

	case CUTSCENE_5:
		this->is_cutscene = true;
		floors.push_back(vec4(25, 25, 5000, 400));

		this->player_starting_pos = vec2(50, 300);
		this->exit_door_pos = vec2(4800, 200);
		this->cutscene_player_velocity = { 300.f,0.f };

		lost_souls.push_back({ vec2(200, 300), LostSoul() });

		terrains.push_back(std::make_pair(vec4(25, 0, 5000, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 400, 5000, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 425), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(5025, 0, default_side_width, 425), SIDE_STATIONARY));

		break;

	case FINAL_BOSS: // actual final boss (the reaper dude)
		floors.push_back(vec4(25, 25, 2700, 1375));

		this->player_starting_pos = vec2(800, 650);
		this->exit_door_pos = NULL_POS;

		terrains.push_back(std::make_pair(vec4(25, 0, 2700, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 1375, 2700, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 1400), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(2725, 0, default_side_width, 1400), SIDE_STATIONARY));

		bosses.push_back(std::make_pair(vec2(1400, 700), COMBO_HIGH_DAMAGE));
		break;

	case POWER_UP:
		floors.push_back(vec4(25, 30, 1300, 650));
		
		this->player_starting_pos = vec2(500, 500);
		this->exit_door_pos = vec2(1200, 575);

		texts.push_back("Shoot the mystery boxes to power up!");
		text_attrs.push_back({ 0.f,150.f,1.0f,1.0f,1.0f,0.f });
		texts.push_back("Choose wisely... you can only keep one!");
		text_attrs.push_back({ 0.f,100.f,1.0f,1.0f,1.0f,0.f });

		terrains.push_back(std::make_pair(vec4(25, 0, 1300, default_north_height), NORTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(25, 675, 1300, default_south_height), SOUTH_STATIONARY));
		terrains.push_back(std::make_pair(vec4(0, 0, default_side_width, 700), SIDE_STATIONARY));
		terrains.push_back(std::make_pair(vec4(1325, 0, default_side_width, 700), SIDE_STATIONARY));
		break;

	default:
		printf("no level provided\n");
		break;
	}

	return true;
}