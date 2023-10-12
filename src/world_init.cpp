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

	position.scale = vec2({ 150, 100 });


	// Create and (empty) Turtle component to be able to refer to all turtles
	registry.enemies.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TURTLE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.resources.emplace(entity);

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
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::TERRAIN,
			GEOMETRY_BUFFER_ID::TERRAIN });

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


/*
Entity createFish(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { -50.f, 0.f };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -FISH_BB_WIDTH, FISH_BB_HEIGHT });

	// Create an (empty) Fish component to be able to refer to all fish
	registry.softShells.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FISH,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createTurtle(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { -100.f, 0.f };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -TURTLE_BB_WIDTH, TURTLE_BB_HEIGHT });

	// Create and (empty) Turtle component to be able to refer to all turtles
	registry.hardShells.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TURTLE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}
*/

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::PEBBLE,
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