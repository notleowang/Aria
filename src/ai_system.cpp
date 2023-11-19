// internal
#include "ai_system.hpp"
#include <iostream>

#include "world_init.hpp"
#include "world_system.hpp"
#include "render_system.hpp"
#include <utils.hpp>

#define ENEMY_PROJECTILE_SPEED 500

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
		
		bool canSprint = enemy.stamina > 0;
		bool isDodging = false;
		bool isSprinting = false;

		for (uint i = 0; i < registry.projectiles.size(); i++) {
			Entity entity_p = registry.projectiles.entities[i];
			Projectile& projectile = registry.projectiles.get(entity_p);
			if (projectile.hostile) continue;
			vec2 projectilePos = registry.positions.get(entity_p).position;
			if (distance(projectilePos, thisPos) < 300) {
				isDodging = true;
				if (canSprint) {
					isSprinting = true;
					enemy.stamina -= elapsed_ms / 1000;
				}

				float c = cosf(90);
				float s = sinf(90);
				mat2 R = {{c, s}, {-s, c}};

				vec2 direction = projectilePos - thisPos;
				direction /= length(direction);
				direction *= isSprinting ? 350 : 50; // allow enemies to sprint even faster to dodge
				vel_i.velocity = direction * R;
			}
		}

		if (enemy.mana < 1.f) {
			enemy.mana += elapsed_ms / 1000;
		}
		if (!isDodging) {
			if (dist <= 350) {
				if (canSprint) {
					isSprinting = true;
					enemy.stamina -= elapsed_ms / 1000;
				}
				vec2 direction = playerPos - thisPos;
				direction /= length(direction);
				enemyFireProjectile(entity_i, direction);
				direction *= isSprinting ? 250 : 50;
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
		}

		if (!isSprinting) {
			// replenish 1 stamina per second if not sprinting
			enemy.stamina += elapsed_ms / 1000;
		}



		// Decision tree:
		// Is there a player-made projectile within 50 pixels?
		//   Yes -> Do I have stamina?
		//     Yes -> Try to dodge at sprint speed
		//     No -> Try to dodge at normal speed
		//   No -> Is player within 350 pixels?
		//     Yes -> Do I have mana?
		//       Yes -> Fire a projectile at the player
		//       No -> Do I have stamina?
		//             Yes -> Sprint towards player
		//             No -> Move towards player
		//     No -> Have I moved in current direction for long enough?
		//           Yes -> Flip direction
		//           No -> Continue moving
	}
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
}

bool AISystem::enemyFireProjectile(Entity& enemy, vec2 direction) {
	// check mana
	if (registry.enemies.get(enemy).mana < 1) {
		return false;
	} else {
		registry.enemies.get(enemy).mana -= 1;
	}

	vec2 enemyPos = registry.positions.get(enemy).position;
	vec2 vel;
	vel.x = direction.x * ENEMY_PROJECTILE_SPEED;
	vel.y = direction.y * ENEMY_PROJECTILE_SPEED;

	// Get current player projectile type
	ElementType elementType = registry.enemies.get(enemy).type;
	if (elementType == ElementType::COMBO) elementType = getRandomElementType();

	createProjectile(renderer, enemyPos, vel, elementType, true, enemy);
	//															 ^^^^^ doesnt matter as ignored by the hostile = true
	// Mix_PlayChannel(-1, projectile_sound, 0);
	return true;
}

void AISystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
}