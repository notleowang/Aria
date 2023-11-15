#include "game_level.hpp" 

bool GameLevel::init(uint level) {
	printf("Initializing game for level: %i\n", level);
	if (level < 0) {
		return false;
	}
	this->curr_level = level;

	std::vector<std::string>& texts = this->texts;
	std::vector<std::array<float, TEXT_ATTRIBUTES>>& text_attrs = this->text_attrs;
	std::vector<vec2>& floors = this->floor_pos;
	std::vector<std::pair<vec4, bool>>& terrains = this->terrains_attr;
	std::vector<std::array<float, ENEMY_ATTRIBUTES>>& enemies = this->enemies_attr;

	texts.clear();
	text_attrs.clear();
	floors.clear();
	terrains.clear();
	enemies.clear();

	switch (level) {
	case TUTORIAL:
		for (uint i = 0; i < 5; i++) {
			for (uint j = 0; j < 3; j++) {
				floors.push_back(vec2(100 + i * 250, 100 + j * 250));
			}
		}

		this->player_starting_pos = vec2(200, 200);
		this->exit_door_pos = vec2(1200, 700);

		texts.push_back("Use WASD to move around");
		text_attrs.push_back({0.f,175.f,1.0f,1.0f,1.0f,0.f});
		texts.push_back("Use M1 button to shoot");
		text_attrs.push_back({0.f,125.f,1.0f,1.0f,1.0f,0.f});
		texts.push_back("Use 1,2,3,4 to cycle through elements");
		text_attrs.push_back({0.f,75.f,1.0f,1.0f,1.0f,0.f});		
		texts.push_back("Move to the exit door when ready");
		text_attrs.push_back({0.f,25.f,1.0f,1.0f,1.0f,0.f});


		terrains.push_back(std::make_pair(vec4(0, 0, 1400, 100), false));
		terrains.push_back(std::make_pair(vec4(0, 100, 100, 700), false));
		terrains.push_back(std::make_pair(vec4(0, 800, 1400, 100), false));
		terrains.push_back(std::make_pair(vec4(1300, 100, 100, 700), false));
		break;
	case POWER_UP:
		for (uint i = 0; i < 5; i++) {
			for (uint j = 0; j < 3; j++) {
				floors.push_back(vec2(100 + i * 250, 100 + j * 250));
			}
		}

		this->player_starting_pos = vec2(500, 500);
		this->exit_door_pos = vec2(1200, 700);

		texts.push_back("Shoot the mystery block to power up!");
		text_attrs.push_back({ 0.f,100.f,1.0f,1.0f,1.0f,0.f });


		terrains.push_back(std::make_pair(vec4(0, 0, 1400, 100), false));
		terrains.push_back(std::make_pair(vec4(0, 100, 100, 700), false));
		terrains.push_back(std::make_pair(vec4(0, 800, 1400, 100), false));
		terrains.push_back(std::make_pair(vec4(1300, 100, 100, 700), false));
		break;
	case LEVEL_1:
		for (uint i = 0; i < 6; i++) {
			for (uint j = 0; j < 4; j++) {
				floors.push_back(vec2(100 + i * 250, 50 + j * 250));
			}
		}
		for (uint i = 0; i < 4; i++) {
			floors.push_back(vec2(1475, 50 + i * 250));
		}

		this->player_starting_pos = vec2(200, 700);
		this->exit_door_pos = vec2(1450, 900);

		// I think we should make a helper for pushing back into terrains vector, thoughts? - Leo
		terrains.push_back(std::make_pair(vec4(0, -50, 1800, 100), false));
		terrains.push_back(std::make_pair(vec4(0, 50, 100, 900), false));
		terrains.push_back(std::make_pair(vec4(0, 950, 1800, 100), false));
		terrains.push_back(std::make_pair(vec4(1700, 50, 100, 900), false));
		terrains.push_back(std::make_pair(vec4(350, 400, 100, 600), false));
		terrains.push_back(std::make_pair(vec4(750, 0, 100, 600), false));
		terrains.push_back(std::make_pair(vec4(1150, 400, 100, 600), false));

		enemies.push_back({ 800, 700, 100, 100, 0, 0});
		enemies.push_back({ 1200, 300, 100, 100, 0, 0 });
		break;
	case LEVEL_2:
		// Temporary level 2
		this->player_starting_pos = vec2(200, 200);
		terrains.push_back(std::make_pair(vec4(550, 350, 100, 100), true));
		terrains.push_back(std::make_pair(vec4(100, 0, 1000, 100), false));
		terrains.push_back(std::make_pair(vec4(0, 0, 100, 800), false));
		terrains.push_back(std::make_pair(vec4(100, 700, 1000, 100), false));
		terrains.push_back(std::make_pair(vec4(1100, 0, 100, 800), false));

		enemies.push_back({ 900, 600, 100, 100, 0, 0 });

		for (uint i = 0; i < 5; i++) {
			for (uint j = 0; j < 3; j++) {
				floors.push_back(vec2(i * 250, 50 + j * 250));
			}
		}

		this->exit_door_pos = vec2(850, 650);
		break;
	case LEVEL_3:
		this->player_starting_pos = vec2(50, 200);
		terrains.push_back(std::make_pair(vec4(-400, -50, 800, 100), false));
		terrains.push_back(std::make_pair(vec4(-400, 50, 100, 1200), false));
		terrains.push_back(std::make_pair(vec4(-300, 1150, 1200, 100), false));
		terrains.push_back(std::make_pair(vec4(400, -50, 100, 500), false));
		terrains.push_back(std::make_pair(vec4(-100, 450, 600, 100), false));
		terrains.push_back(std::make_pair(vec4(150, 450, 1000, 100), false));
		terrains.push_back(std::make_pair(vec4(1150, 450, 100, 1600), false));
		terrains.push_back(std::make_pair(vec4(850, 1150, 100, 900), false));
		terrains.push_back(std::make_pair(vec4(925, 1950, 250, 100), false));

		enemies.push_back({ 50, 900, 100, 100, 0, 0 });
		enemies.push_back({ 450, 900, 100, 100, 0, 0 });
		enemies.push_back({ 850, 900, 100, 100, 0, 0 });


		this->exit_door_pos = vec2(1000, 1950);
		break;
	case LEVEL_4:
		this->player_starting_pos = vec2(200, 700);

		terrains.push_back(std::make_pair(vec4(0, -50, 1800, 100), false));
		terrains.push_back(std::make_pair(vec4(0, 50, 100, 900), false));
		terrains.push_back(std::make_pair(vec4(0, 950, 1800, 100), false));
		terrains.push_back(std::make_pair(vec4(1700, 50, 100, 900), false));
		terrains.push_back(std::make_pair(vec4(350, 400, 100, 600), false));
		terrains.push_back(std::make_pair(vec4(759, 0, 100, 600), false));
		terrains.push_back(std::make_pair(vec4(1150, 400, 100, 600), false));

		terrains.push_back(std::make_pair(vec4(150, 450, 50, 100), true));
		terrains.push_back(std::make_pair(vec4(525, 450, 50, 100), true));
		terrains.push_back(std::make_pair(vec4(775, 450, 50, 100), true));
		terrains.push_back(std::make_pair(vec4(1125, 450, 50, 100), true));

		enemies.push_back({ 350, 250, 100, 100, 0, 0 });
		enemies.push_back({ 700, 800, 100, 100, 0, 0 });
		enemies.push_back({ 950, 800, 100, 100, 0, 0 });
		enemies.push_back({ 1100, 250, 100, 100, 0, 0 });

		for (uint i = 0; i < 6; i++) {
			for (uint j = 0; j < 4; j++) {
				floors.push_back(vec2(100 + i * 250, 50 + j * 250));
			}
		}
		for (uint i = 0; i < 4; i++) {
			floors.push_back(vec2(1475, 50+ i * 250));
		}

		this->exit_door_pos = vec2(1450, 900);
		break;
	case LEVEL_3:
		this->player_starting_pos = vec2(50, 200);
		terrains.push_back(std::make_pair(vec4(0, 0, 800, 100), false));
		terrains.push_back(std::make_pair(vec4(-350, 650, 100, 1200), false));
		terrains.push_back(std::make_pair(vec4(300, 1200, 1200, 100), false));
		terrains.push_back(std::make_pair(vec4(450, 200, 100, 500), false));
		terrains.push_back(std::make_pair(vec4(200, 500, 600, 100), false));
		terrains.push_back(std::make_pair(vec4(650, 500, 1000, 100), false));
		terrains.push_back(std::make_pair(vec4(1200, 1250, 100, 1600), false));
		terrains.push_back(std::make_pair(vec4(900, 1600, 100, 900), false));
		terrains.push_back(std::make_pair(vec4(1050, 2000, 250, 100), false));

		enemies.push_back({ 50, 900, 100, 100, 0, 0 });
		enemies.push_back({ 450, 900, 100, 100, 0, 0 });
		enemies.push_back({ 850, 900, 100, 100, 0, 0 });
		enemies.push_back({ 1000, 1400, 100, 100, 0, 0 });
		enemies.push_back({ 1000, 1550, 100, 100, 0, 0 });
		enemies.push_back({ 1000, 1700, 100, 100, 0, 0 });
		enemies.push_back({ 1000, 1850, 100, 100, 0, 0 });

		this->exit_door_pos = vec2(1050, 2000);
	case LEVEL_4:
		this->player_starting_pos = vec2(200, 700);

		terrains.push_back(std::make_pair(vec4(900, 0, 1800, 100), false));
		terrains.push_back(std::make_pair(vec4(50, 500, 100, 900), false));
		terrains.push_back(std::make_pair(vec4(900, 1000, 1800, 100), false));
		terrains.push_back(std::make_pair(vec4(1750, 500, 100, 900), false));
		terrains.push_back(std::make_pair(vec4(400, 700, 100, 600), false));
		terrains.push_back(std::make_pair(vec4(800, 300, 100, 600), false));
		terrains.push_back(std::make_pair(vec4(1200, 700, 100, 600), false));

		terrains.push_back(std::make_pair(vec4(200, 500, 50, 100), true));
		terrains.push_back(std::make_pair(vec4(550, 500, 50, 100), true));
		terrains.push_back(std::make_pair(vec4(800, 500, 50, 100), true));
		terrains.push_back(std::make_pair(vec4(1150, 500, 50, 100), true));

		enemies.push_back({ 350, 250, 100, 100, 0, 0 });
		enemies.push_back({ 700, 800, 100, 100, 0, 0 });
		enemies.push_back({ 950, 800, 100, 100, 0, 0 });
		enemies.push_back({ 1100, 250, 100, 100, 0, 0 });

		for (uint i = 0; i < 6; i++) {
			for (uint j = 0; j < 4; j++) {
				floors.push_back(vec2(225 + i * 250, 175 + j * 250));
			}
		}
		for (uint i = 0; i < 4; i++) {
			floors.push_back(vec2(1600, 175 + i * 250));
		}

		this->exit_door_pos = vec2(1500, 950);
	default:
		printf("no level provided\n");
		break;
	}

	return true;
}