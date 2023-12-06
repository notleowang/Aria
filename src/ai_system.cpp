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

		if (registry.bosses.has(entity_i) && enemy.isAggravated) {
			Boss& boss = registry.bosses.get(entity_i);
			if (boss.phaseTimer > 0.f) {
				boss.phaseTimer -= elapsed_ms;
			} else {
				// printf("Resolving phase %d:%d\n", boss.phase, boss.subphase);
				switch (boss.phase) {
					case 0:
						if (boss.subphase == 48) {
							boss.phase += 1;
							boss.phaseTimer = 5000.f;
							boss.subphase = 0;
						} else {
							for (int deg = boss.subphase * 2; deg < 360 + boss.subphase * 2; deg += 120) {
								float rad = deg * 180 / 3.14;
								vec2 direction = {cosf(rad), sinf(rad)};
								enemyFireProjectile(entity_i, direction, 0.5f);
							}
							boss.subphase += 1;
							boss.phaseTimer = 50.f;
						}
						break;
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
						if (boss.subphase == 10) {
							boss.phaseTimer = 50.f;
							if (boss.phase == 7) {
								boss.phaseTimer = 1500.f;
							}
							boss.phase += 1;
							boss.subphase = 0;
						} else {
							vec2 direction = {1.f, 0.f};
							if (boss.subphase >= 5) {
								direction = {-1.f, 0.f};
							}
							// vec2 position = registry.positions.get(entity_i).position;
							vec2 adjust = {0, (boss.phase - 4) * 50};
							vec2 subadjust = {0, ((boss.subphase % 5) + 1) * 75 + 40};
							enemyFireProjectile(entity_i, direction, 0.5f, thisPos - adjust + subadjust);
							enemyFireProjectile(entity_i, direction, 0.5f, thisPos - adjust - subadjust);
							boss.subphase += 1;
							boss.phaseTimer = 25.f;
						}
						break;
					case 8:
						if (boss.subphase == 25) {
							boss.phase += 1;
							boss.phaseTimer = 1500.f;
							boss.subphase = 0;
						} else {
							registry.resources.get(entity_i).currentHealth += 25;
							if (registry.resources.get(entity_i).currentHealth > registry.resources.get(entity_i).maxHealth) {
								registry.resources.get(entity_i).currentHealth = registry.resources.get(entity_i).maxHealth;
							}
							boss.subphase += 1;
							boss.phaseTimer = 50.f;
						}
						break;
					case 9:
						if (boss.subphase == 4) {
							boss.phase += 1;
							boss.phaseTimer = 1000.f;
							boss.subphase = 0;
						} else {
							for (int deg = 0; deg < 360; deg += 10) {
								float rad = deg * 180 / 3.14;
								vec2 direction = {cosf(rad), sinf(rad)};
								if (boss.subphase == 0) {
									direction *= 200;
								} else {
									direction *= 150 * (boss.subphase + 1);
								}
								enemyFireProjectile(entity_i, - direction, 0.0f, playerPos + direction);
							}
							boss.subphase += 1;
							boss.phaseTimer = 100.f;
						}
						break;
					case 10:
					case 11:
					case 12:
					case 13:
					case 14:
						for (uint i = 0; i < registry.projectiles.size(); i++) {
							Entity thisProj = registry.projectiles.entities[i];
							if (!registry.projectiles.get(thisProj).hostile) continue;
							Velocity& thisProjVel = registry.velocities.get(thisProj);
							switch (boss.phase) {
								case 10:
									// make sure the circle does not lead back into the boss
									thisProjVel.velocity = normalize(playerPos - thisPos);
									thisProjVel.velocity *= 200;
									break;
								case 11:
									thisProjVel.velocity = {-150, 0};
									break;
								case 12:
									thisProjVel.velocity = {0, 150};
									break;
								case 13:
									thisProjVel.velocity = {150, 0};
									break;
								case 14:
									thisProjVel.velocity = {0, -150};
									break;
							}
						}
						boss.phaseTimer = 750.f;
						if (boss.phase == 10) {
							boss.phaseTimer = 1000.f;
						}
						boss.phase += 1;
						boss.subphase = 0;
						break;
					case 15:
					case 16:
						for (uint i = 0; i < registry.projectiles.size(); i++) {
							Entity thisProj = registry.projectiles.entities[i];
							if (!registry.projectiles.get(thisProj).hostile) continue;
							Velocity& thisProjVel = registry.velocities.get(thisProj);
							Position& thisProjPos = registry.positions.get(thisProj);
							thisProjVel.velocity = normalize(thisProjPos.position - playerPos);
							thisProjVel.velocity *= 100;
							if (boss.phase == 15) {
								thisProjVel.velocity *= -0.75;
							}
						}
						boss.phase += 1;
						boss.phaseTimer = 1000.f;
						boss.subphase = 0;
						break;
					case 17:
						while (0 != registry.projectiles.size()) {
							registry.remove_all_components_of_no_collision(registry.projectiles.entities[0]);
						}
						boss.phase += 1;
						boss.phaseTimer = 1500.f;
						boss.subphase = 0;
						break;
					default:
						boss.phaseTimer = 2500.f;
						boss.phase = 0; // reset to first phase
						break;
				}
			}
		}

		if (!registry.bosses.has(entity_i)) { // bosses never dodge
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

		if (!isDodging && !isFlanking && !registry.bosses.has(entity_i)) {
			// bosses never give chase
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

		if (registry.bosses.has(entity_i)) {
			// idk why bosses still move but set their velo to 0 here
			registry.velocities.get(entity_i).velocity.x = 0;
			registry.velocities.get(entity_i).velocity.y = 0;
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
}




bool AISystem::enemyFireProjectile(Entity& enemy, vec2 direction, float speedMultiplier, vec2 position) {

	// vec2 enemyPos = registry.positions.get(enemy).position;
	vec2 vel;
	vel.x = direction.x * ENEMY_PROJECTILE_SPEED * speedMultiplier;
	vel.y = direction.y * ENEMY_PROJECTILE_SPEED * speedMultiplier;

	// Get current player projectile type
	ElementType elementType = registry.enemies.get(enemy).type;
	if (elementType == ElementType::COMBO) elementType = getRandomElementType();

	createProjectile(renderer, position, vel, elementType, true, enemy);
	//															 ^^^^^ doesnt matter as ignored by the hostile = true
	// Mix_PlayChannel(-1, projectile_sound, 0);
	return true;
}

bool AISystem::enemyFireProjectile(Entity& enemy, vec2 direction, float speedMultiplier) {
	return enemyFireProjectile(enemy, direction, speedMultiplier, registry.positions.get(enemy).position);
}

bool AISystem::enemyFireProjectile(Entity& enemy, vec2 direction) {
	return enemyFireProjectile(enemy, direction, 1.f);
}

void AISystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
}