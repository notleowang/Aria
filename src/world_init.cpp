#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createAria(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::ARIA);
	registry.meshPtrs.emplace(entity, &mesh);

	// set initial component values
	Position& position = registry.positions.emplace(entity);
	position.position = pos;
	position.scale = vec2(100.f, 100.f);

	Velocity& velocity = registry.velocities.emplace(entity);
	velocity.velocity = { 0.f, 0.f };

	Resources& resources = registry.resources.emplace(entity);
	resources.healthBar = createHealthBar(renderer, entity);
	
	HealthBar& healthBar = registry.healthBars.get(resources.healthBar);
	healthBar.y_offset = -60.f;

	Direction& direction = registry.directions.emplace(entity);
	direction.direction = DIRECTION::E;

	PowerUp& powerUp = registry.powerUps.emplace(entity);
	//powerUp.fasterMovement = false;
	//powerUp.bounceOffWalls[ElementType::EARTH] = true;

	registry.characterProjectileTypes.emplace(entity);
	registry.players.emplace(entity);
	registry.collidables.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TODO: CHANGE TEXTURE OF ARIA
			EFFECT_ASSET_ID::ARIA,
			GEOMETRY_BUFFER_ID::ARIA});

	return entity;
}

Entity createFloor(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// set initial component values
	Position& position = registry.positions.emplace(entity);
	position.position = pos;
	position.scale = vec2(250.f, 250.f);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FLOOR,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createTerrain(RenderSystem* renderer, vec2 pos, vec2 size)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::TERRAIN);
	registry.meshPtrs.emplace(entity, &mesh);

	Position& position = registry.positions.emplace(entity);
	position.position = pos;
	position.scale = size;

	registry.terrain.emplace(entity);
	registry.collidables.emplace(entity); // Marking terrain as collidable
	registry.renderRequests.insert(
		entity, 
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::TERRAIN,
			GEOMETRY_BUFFER_ID::TERRAIN });
	
	return entity;
}

Entity createEnemy(RenderSystem* renderer, vec2 pos, ElementType enemyType)
{
	// TODO: change enemy implementation to include different enemy types
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Position& position = registry.positions.emplace(entity);
	position.position = pos;

	position.scale = vec2({ 125, 100 });

	Velocity& velocity = registry.velocities.emplace(entity);
	velocity.velocity.x = 50;

	Resources& resources = registry.resources.emplace(entity);
	resources.healthBar = createHealthBar(renderer, entity);

	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.type = enemyType;
	
	TEXTURE_ASSET_ID textureAsset;
	switch (enemy.type) {
	case ElementType::WATER:
		textureAsset = TEXTURE_ASSET_ID::TURTLE;
		break;

	case ElementType::FIRE:
		textureAsset = TEXTURE_ASSET_ID::FIRE_ENEMY;
		break;
	//case ElementType::Earth:
	//	textureAsset = TEXTURE_ASSET_ID::EARTH_ENEMY;
	//	break;
	//case ElementType::Lightning:
	//	textureAsset = TEXTURE_ASSET_ID::LIGHTNING_ENEMY;
	//	break;
	default:
		textureAsset = TEXTURE_ASSET_ID::TURTLE;
		break;
	}

	registry.collidables.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{textureAsset,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createHealthBar(RenderSystem* renderer, Entity &owner_entity)
{
	auto entity = Entity();

	HealthBar& healthBar = registry.healthBars.emplace(entity);
	healthBar.owner = owner_entity;

	Position& position = registry.positions.emplace(entity);
	position.scale = vec2(HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HEALTH_BAR_EMPTY, // !!! This technically does nothing
			EFFECT_ASSET_ID::HEALTH_BAR,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createExitDoor(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::EXIT_DOOR);
	registry.meshPtrs.emplace(entity, &mesh);

	Position& position = registry.positions.emplace(entity);
	position.position = pos;
	position.scale = vec2(100.f, 100.f);

	registry.exitDoors.emplace(entity);
	registry.collidables.emplace(entity);
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
	resources.healthBar = createHealthBar(renderer, entity);

	Direction& direction = registry.directions.emplace(entity);
	direction.direction = DIRECTION::E;

	registry.players.emplace(entity);
	registry.collidables.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::SALMON,
			GEOMETRY_BUFFER_ID::SALMON });

	return entity;
}

Entity createProjectile(RenderSystem* renderer, vec2 pos, vec2 vel, ElementType elementType, Entity& player) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Set initial position and velocity for the projectile
	Position& position = registry.positions.emplace(entity);
	position.position = pos;
	position.scale = vec2(30.f, 30.f);

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.type = elementType;
	TEXTURE_ASSET_ID textureAsset;
	switch (projectile.type) {
		case ElementType::WATER:
			textureAsset = TEXTURE_ASSET_ID::WATER_PROJECTILE;
			break;
		
		case ElementType::FIRE:
			textureAsset = TEXTURE_ASSET_ID::FIRE_PROJECTILE;
			break;
		case ElementType::EARTH:
			textureAsset = TEXTURE_ASSET_ID::EARTH_PROJECTILE;
			break;
		case ElementType::LIGHTNING:
			textureAsset = TEXTURE_ASSET_ID::LIGHTNING_PROJECTILE;
			break;
		default:
			textureAsset = TEXTURE_ASSET_ID::WATER_PROJECTILE;
			break;
	}

	registry.collidables.emplace(entity);

	Velocity& velocity = registry.velocities.emplace(entity);
	velocity.velocity = vel;

	PowerUp& powerUp = registry.powerUps.get(player);
	if (powerUp.increasedDamage[elementType]) projectile.damage *= 1.5; // increase damage by factor of 1.5
	if (powerUp.bounceOffWalls[elementType]) projectile.bounces = 2; // allow 2 bounces off walls

	registry.renderRequests.insert(
		entity,
		{	textureAsset, //TODO: Change texture asset- the projectiles are currently turtles
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