#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "render_system.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

class AISystem
{
public:
	void step(float elapsed_ms);
	void init(RenderSystem* renderer);
private:
	bool enemyFireProjectile(Entity& enemy, vec2 direction);
	RenderSystem* renderer;
};