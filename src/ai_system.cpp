// internal
#include "ai_system.hpp"
#include <iostream>

#include "world_init.hpp"
#include "world_system.hpp"
#include "render_system.hpp"

#define ENEMY_PROJECTILE_SPEED 400

void AISystem::step(float elapsed_ms)
{
	auto& enemy_container = registry.enemies;
	Entity player = registry.players.entities[0];
	// Check for collisions with entities that have a velocity
	for (uint i = 0; i < enemy_container.size(); i++)
	{
		Entity entity_i = enemy_container.entities[i];
		Velocity& vel_i = registry.velocities.get(entity_i);
		Enemy& enemy = enemy_container.get(entity_i);

		vec2 playerPos = registry.positions.get(player).position;
		vec2 thisPos = registry.positions.get(entity_i).position;
		float dist = distance(playerPos, thisPos);

		if (enemy.stamina < 10.f && dist > 350) {
			// replenish 1 stamina per second if not sprinting
			enemy.stamina += elapsed_ms / 1000;
		}

		if (enemy.mana < 1.f) {
			enemy.mana += elapsed_ms / 1000;
		}

		if (dist <= 350) {
			bool sprint = enemy.stamina > 0;
			if (sprint) {
				enemy.stamina -= elapsed_ms / 1000;
			}
			vec2 direction = playerPos - thisPos;
			direction /= length(direction);
			enemyFireProjectile(entity_i, direction);
			direction *= sprint ? 250 : 50;
			vel_i.velocity = direction;
		} else {
			vel_i.velocity.y = 0;
			if (abs(vel_i.velocity.x) != 50) {
				vel_i.velocity.x = 50;
			}
			if (enemy.movementTimer <= 0.f) {
				enemy.movementTimer = 3000.f;
				vel_i.velocity.x = -vel_i.velocity.x;
			} else {
				enemy.movementTimer -= elapsed_ms;
			}
		}



		// Decision tree:
		// Is player within 750 pixels?
		//   Yes -> Do I have stamina?
		//           Yes -> Sprint towards player
		//           No -> Move towards player
		//   No -> Have I moved in current direction for long enough?
		//           Yes -> Flip direction
		//           No -> Continue moving
	}
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
}

void AISystem::enemyFireProjectile(Entity& enemy, vec2 direction) {
	// check mana
	if (registry.enemies.get(enemy).mana < 1) {
		return;
	} else {
		registry.enemies.get(enemy).mana -= 1;
	}

	vec2 enemyPos = registry.positions.get(enemy).position;
	vec2 vel;
	vel.x = direction.x * ENEMY_PROJECTILE_SPEED;
	vel.y = direction.y * ENEMY_PROJECTILE_SPEED;

	// Get current player projectile type
	ElementType elementType = registry.enemies.get(enemy).type;

	createProjectile(renderer, enemyPos, vel, elementType, true);
	// Mix_PlayChannel(-1, projectile_sound, 0);
}

void AISystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
}