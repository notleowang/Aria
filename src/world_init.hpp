#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// the player
Entity createAria(RenderSystem* renderer, vec2 pos);
Entity createProjectile(RenderSystem* renderer, vec2 pos, vec2 vel);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);

// creates a terrain with fixed size
Entity createTerrain(vec2 pos, vec2 size);

// creates an exit door
Entity createExitDoor(vec2 pos);

// mock enemy TODO: change enemy implementation
Entity createEnemy(RenderSystem* renderer, vec2 pos);

// test
Entity createTestSalmon(RenderSystem* renderer, vec2 pos);
Entity createTestStationaryTexture(RenderSystem* renderer, vec2 pos);

