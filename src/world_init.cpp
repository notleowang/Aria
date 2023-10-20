#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createAria(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();
	return entity;
}

Entity createTerrain(vec2 pos, vec2 size)
{
	auto entity = Entity();

	Position& position = registry.positions.emplace(entity);
	position.position = pos;
	position.scale = size;

	registry.terrain.emplace(entity);
	registry.renderRequests.insert(
		entity, 
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::TERRAIN,
			GEOMETRY_BUFFER_ID::TERRAIN });
	
	return entity;
}

Entity createEnemy(RenderSystem* renderer, vec2 pos)
{
	// TODO: change enemy implementation to include different enemy types
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	auto& position = registry.positions.emplace(entity);
	position.position = pos;

	position.scale = vec2({ 75, 50 });


	// Create and (empty) Turtle component to be able to refer to all turtles
	registry.enemies.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TURTLE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.resources.emplace(entity);
	auto& velocity = registry.velocities.emplace(entity);
	velocity.velocity.x = 50;

	createHealthBar(renderer, entity);

	return entity;
}

Entity createHealthBar(RenderSystem* renderer, Entity &owner_entity)
{
	auto entity = Entity();

	HealthBar& healthBar = registry.healthBars.emplace(entity);
	healthBar.owner = owner_entity;
	healthBar.y_offset = -50.f;

	Position& position = registry.positions.emplace(entity);
	position.scale = vec2(HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HEALTH_BAR_EMPTY, // !!! This technically does nothing
			EFFECT_ASSET_ID::HEALTH_BAR,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createExitDoor(vec2 pos) {
	auto entity = Entity();

	Position& position = registry.positions.emplace(entity);
	position.position = pos;
	position.scale = vec2(100.f, 100.f);

	registry.exitDoors.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::EXIT_DOOR,
			GEOMETRY_BUFFER_ID::EXIT_DOOR});

	return entity;
}

Entity createTestSalmon(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SALMON);
	registry.meshPtrs.emplace(entity, &mesh);

	// set initial component values
	Position& position = registry.positions.emplace(entity);
	position.position = pos;
	position.scale = mesh.original_size * 150.f;
	position.scale.x *= -1; // point front to the right; with sprites this wont be a thing?
	// TODO: how to we integrate direction into our entities?

	Velocity& velocity = registry.velocities.emplace(entity);
	velocity.velocity = { 0.f, 0.f };

	Resources& resources = registry.resources.emplace(entity);

	Direction& direction = registry.directions.emplace(entity);
	direction.direction = DIRECTION::E;

	// Create and (empty) Salmon component to be able to refer to all turtles
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::SALMON,
			GEOMETRY_BUFFER_ID::SALMON });

	createHealthBar(renderer, entity);

	return entity;
}

Entity createProjectile(RenderSystem* renderer, vec2 pos, vec2 vel) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Set initial position and velocity for the projectile
	Position& position = registry.positions.emplace(entity);
	position.position = pos;
	//position.scale = ??

	registry.projectiles.emplace(entity);

	Velocity& velocity = registry.velocities.emplace(entity);
	velocity.velocity = vel;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TURTLE, //TODO: Change texture asset- the projectiles are currently turtles
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::TERRAIN,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE });

	// Create motion
	/*
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;
	motion.scale = scale;
	*/

	registry.debugComponents.emplace(entity);
	return entity;
}