// internal
#include "ai_system.hpp"
#include <iostream>

#include "world_init.hpp"
#include "world_system.hpp"
#include "render_system.hpp"
#include <chrono>
#include <utils.hpp>

#define ENEMY_PROJECTILE_SPEED 500

void AISystem::step(float elapsed_ms)
{
	auto& enemy_container = registry.enemies;
	Entity player = registry.players.entities[0];
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
		bool isFlanking = false;

		if (enemy.isAggravated) {
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

					int deg = 90;
					// https://stackoverflow.com/questions/16177295/get-time-since-epoch-in-milliseconds-preferably-using-c11-chrono
					unsigned long milliseconds_since_epoch = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
					if (milliseconds_since_epoch % 10000 > 5000) {
						deg = -90;
					}
					float c = cosf(deg);
					float s = sinf(deg);
					mat2 R = {{c, s}, {-s, c}};

					vec2 direction = projectilePos - thisPos;
					direction /= length(direction);
					direction *= isSprinting ? 300 : 50; // allow enemies to sprint even faster to dodge
					vel_i.velocity = direction * R;
				}
			}
		}

		if (enemy.mana < 1.f) {
			enemy.mana += elapsed_ms / 1000;
		}


		for (uint j = 0; j < enemy_container.size(); j++) {
			if (i == j) continue;
			Entity entity_j = enemy_container.entities[j];
			Enemy& enemy_j = enemy_container.get(entity_j);
			if (distance(registry.positions.get(entity_j).position, thisPos) < 250 && registry.resources.get(entity_j).currentHealth < 80 && enemy_j.type != enemy.type) {
				vec2 direction = registry.positions.get(entity_j).position - thisPos;
				direction /= length(direction);
				if (enemy.mana >= 0.75f) {
					// printf("Attempting to heal injured ally!\n");
					enemyFireProjectile(entity_i, direction);
					enemy.mana -= 0.75f;
				}
			}
			// flank the player
			if (distance(thisPos, registry.positions.get(entity_j).position) < 100 && i > j) {
				vec2 direction = playerPos - thisPos;
				direction /= length(direction);
				direction *= -50;
				if (distance(thisPos, playerPos) > 100) {
					vel_i.velocity = direction;
				}
				isFlanking = true;
			}
		}

		if (!isDodging && !isFlanking) {
			if (dist <= 350 && dist > 15 && enemy.isAggravated) {
				if (canSprint) {
					isSprinting = true;
					enemy.stamina -= elapsed_ms / 1000;
				}
				vec2 direction = playerPos - thisPos;
				direction /= length(direction);
				if (enemy.mana >= 1.f) {
					enemyFireProjectile(entity_i, direction);
					enemy.mana -= 1.f;
				}
				direction *= isSprinting ? 200 : 50;
				vel_i.velocity = direction;
			} else if (dist > 350 || !enemy.isAggravated) {
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