#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<WinTimer> winTimers;
	ComponentContainer<WeaknessTimer> weaknessTimers;
	ComponentContainer<Resources> resources;
	ComponentContainer<HealthBar> healthBars;
	ComponentContainer<ManaBar> manaBars;
	ComponentContainer<Projectile> projectiles;
	ComponentContainer<CharacterProjectileType> characterProjectileTypes;
	ComponentContainer<ProjectileSelectDisplay> projectileSelectDisplays;
	ComponentContainer<Follower> followers;
	ComponentContainer<Text> texts;
	ComponentContainer<InvulnerableTimer> invulnerableTimers;
	ComponentContainer<Position> positions;
	ComponentContainer<Velocity> velocities;
	ComponentContainer<Floor> floors;
	ComponentContainer<Direction> directions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Collidable> collidables;
	ComponentContainer<Player> players;
	ComponentContainer<Enemy> enemies;
	ComponentContainer<Boss> bosses;
	ComponentContainer<PowerUp> powerUps;
	ComponentContainer<PowerUpBlock> powerUpBlocks;
	ComponentContainer<Terrain> terrain;
	ComponentContainer<HealthPack> healthPacks;
	ComponentContainer<Shadow> shadows;
	ComponentContainer<ExitDoor> exitDoors;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<SpriteSheet*> spriteSheetPtrs;
	ComponentContainer<Animation> animations;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;
	ComponentContainer<Obstacle> obstacles;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&weaknessTimers);
		registry_list.push_back(&projectiles);
		registry_list.push_back(&texts);
		registry_list.push_back(&resources);
		registry_list.push_back(&healthBars);
		registry_list.push_back(&manaBars);
		registry_list.push_back(&characterProjectileTypes);
		registry_list.push_back(&projectileSelectDisplays);
		registry_list.push_back(&followers);
		registry_list.push_back(&invulnerableTimers);
		registry_list.push_back(&positions);
		registry_list.push_back(&velocities);
		registry_list.push_back(&floors);
		registry_list.push_back(&directions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&collidables);
		registry_list.push_back(&players);
		registry_list.push_back(&enemies);
		registry_list.push_back(&bosses);
		registry_list.push_back(&powerUps);
		registry_list.push_back(&powerUpBlocks);
		registry_list.push_back(&terrain);
		registry_list.push_back(&healthPacks);
		registry_list.push_back(&shadows);
		registry_list.push_back(&exitDoors);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&spriteSheetPtrs);
		registry_list.push_back(&animations);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
		registry_list.push_back(&obstacles);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}

	void remove_all_components_of_no_collision(Entity e) {
		for (ContainerInterface* reg : registry_list) {
			if (reg == &collisions) continue;
			reg->remove(e);
		}
	}
};

extern ECSRegistry registry;