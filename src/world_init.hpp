#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"
using namespace std;

const float RESOURCE_BAR_HEIGHT = 0.3f * 33.f;
const float RESOURCE_BAR_WIDTH = 0.3f * 369.f;
const int PROJECTILE_SPRITESHEET_NUM_ROWS = 1;
const int PROJECTILE_SPRITESHEET_NUM_COLS = 4;

// the player
Entity createAria(RenderSystem* renderer, vec2 pos);
Entity createProjectile(RenderSystem* renderer, vec2 pos, vec2 vel, ElementType elementType, bool hostile, Entity& player);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);

// creates a terrain with fixed size
Entity createTerrain(RenderSystem* renderer, vec2 pos, vec2 size, bool moveable);

// creates an exit door
Entity createExitDoor(RenderSystem* renderer, vec2 pos);

Entity createText(std::string in_text, vec2 pos, float scale, vec3 color);

// creates a power up block
Entity createPowerUpBlock(RenderSystem* renderer, pair<string, bool*>* powerUp);

// mock enemy TODO: change enemy implementation
Entity createEnemy(RenderSystem* renderer, vec2 pos, ElementType enemyType);

// creates a health bar associated with an owner entity
Entity createHealthBar(RenderSystem* renderer, Entity &owner_entity);

// creates a mana bar associated with an owner entity
Entity createManaBar(RenderSystem* renderer, Entity& owner_entity);

Entity createFloor(RenderSystem* renderer, vec2 pos);

// test
Entity createTestSalmon(RenderSystem* renderer, vec2 pos);

