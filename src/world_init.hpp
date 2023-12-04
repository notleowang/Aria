#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"
using namespace std;

const float SCALE_FACTOR = 2.f;
const float PLAYER_HEALTH_BAR_Y_OFFSET = -60.f;
const float ENEMY_HEALTH_BAR_Y_OFFSET = -50.f;
const float BOSS_HEALTH_BAR_Y_OFFSET = -110.f;
const float PLAYER_MANA_BAR_Y_OFFSET = -75.f;
const float PROJECTILE_SELECT_DISPLAY_Y_OFFSET = 274.f;
const float PROJECTILE_SELECT_DISPLAY_X_OFFSET = 565.f;

// the player
Entity createAria(RenderSystem* renderer, vec2 pos);
Entity createProjectile(RenderSystem* renderer, vec2 pos, vec2 vel, ElementType elementType, bool hostile, Entity& player);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);

// creates a terrain with fixed size
Entity createTerrain(RenderSystem* renderer, vec2 pos, vec2 size, DIRECTION dir, bool moveable);

// creates an exit door
Entity createExitDoor(RenderSystem* renderer, vec2 pos);

Entity createText(std::string in_text, vec2 pos, float scale, vec3 color);

// creates a power up block
Entity createPowerUpBlock(RenderSystem* renderer, pair<string, bool*>* powerUp, vec2 pos);

// mock enemy TODO: change enemy implementation
Entity createEnemy(RenderSystem* renderer, vec2 pos, Enemy enemyAttributes);

// creates the hooded guy
Entity createLostSoul(RenderSystem* renderer, vec2 pos);

// creates a boss
Entity createBoss(RenderSystem* renderer, vec2 pos, Enemy enemyAttributes);

//Creates a ghost obstacle
Entity createObstacle(RenderSystem* renderer, vec2 pos, vec2 size, vec2 vel);

// creates a health bar associated with an owner entity
Entity createHealthBar(RenderSystem* renderer, Entity &owner_entity, float y_offset);

// creates a mana bar associated with an owner entity
Entity createManaBar(RenderSystem* renderer, Entity& owner_entity, float y_offset);

// creates UI that displays the currently selected projectile element
Entity createProjectileSelectDisplay(RenderSystem* renderer, Entity& owner_entity, float y_offset, float x_offset);

Entity createFloor(RenderSystem* renderer, vec2 pos, vec2 size);

Entity createHealthPack(RenderSystem* renderer, vec2 pos);

Entity createShadow(RenderSystem* renderer, Entity& owner_entity, TEXTURE_ASSET_ID texture, GEOMETRY_BUFFER_ID geom);

// test entity
Entity createTestSalmon(RenderSystem* renderer, vec2 pos);

