#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "render_system.hpp"

Entity createAria(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::PLAYER);
	registry.meshPtrs.emplace(entity, &mesh);

	SpriteSheet& sprite_sheet = renderer->getSpriteSheet(SPRITE_SHEET_DATA_ID::PLAYER);
	registry.spriteSheetPtrs.emplace(entity, &sprite_sheet);

	Animation& animation = registry.animations.emplace(entity);
	animation.sprite_sheet_ptr = &sprite_sheet;
	animation.setState((int)PLAYER_SPRITE_STATES::EAST);
	animation.is_animating = false; // initially stationary

	// set initial component values
	Position& position = registry.positions.emplace(entity);
	position.position = pos;
	position.scale = vec2(60.f, 100.f);

	Velocity& velocity = registry.velocities.emplace(entity);
	velocity.velocity = { 0.f, 0.f };

	Resources& resources = registry.resources.emplace(entity);
	resources.healthBar = createHealthBar(renderer, entity);
	resources.manaBar = createManaBar(renderer, entity);

	Direction& direction = registry.directions.emplace(entity);
	direction.direction = DIRECTION::E;

	PowerUp& powerUp = registry.powerUps.emplace(entity);
	// TOGGLE THESE TO TEST OR GO GOD MODE - enjoy! :)
	//powerUp.fasterMovement = true;
	//powerUp.increasedDamage[ElementType::WATER] = true;
	//powerUp.increasedDamage[ElementType::FIRE] = true;
	//powerUp.increasedDamage[ElementType::EARTH] = true;
	//powerUp.increasedDamage[ElementType::LIGHTNING] = true;
	//powerUp.tripleShot[ElementType::WATER] = true;
	//powerUp.tripleShot[ElementType::FIRE] = true;
	//powerUp.tripleShot[ElementType::EARTH] = true;
	//powerUp.tripleShot[ElementType::LIGHTNING] = true;
	//powerUp.bounceOffWalls[ElementType::WATER] = true;
	//powerUp.bounceOffWalls[ElementType::FIRE] = true;
	//powerUp.bounceOffWalls[ElementType::EARTH] = true;
	//powerUp.bounceOffWalls[ElementType::LIGHTNING] = true;

	registry.characterProjectileTypes.emplace(entity);
	registry.players.emplace(entity);
	registry.collidables.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLAYER,
			EFFECT_ASSET_ID::ANIMATED,
			GEOMETRY_BUFFER_ID::PLAYER});

	return entity;
}

Entity createFloor(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// set initial component values
	Position& position = registry.positions.emplace(entity);
	position.scale = vec2(250.f, 250.f);
	// pos passed in to createFloor assumes top left corner is (x,y)
	position.position = vec2(pos.x + position.scale.x/2, pos.y + position.scale.y/2);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FLOOR,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createTerrain(RenderSystem* renderer, vec2 pos, vec2 size, bool moveable)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::TERRAIN);
	registry.meshPtrs.emplace(entity, &mesh);

	// The position passed into createTerrain (x,y) assumes the top left corner
	// and size corresponds to width and height
	Position& position = registry.positions.emplace(entity);
	position.position = vec2(pos.x + size.x/2, pos.y + size.y/2);
	position.scale = size;

	Terrain& terrain = registry.terrain.emplace(entity);
	if (moveable) {
		terrain.moveable = true;
		Velocity& velocity = registry.velocities.emplace(entity);
		velocity.velocity = { 200.f, 0.f };
	}
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
	
	HealthBar& healthBar = registry.healthBars.get(resources.healthBar);
	healthBar.y_offset = -50.f;

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

Entity createHealthBar(RenderSystem* renderer, Entity& owner_entity)
{
	auto entity = Entity();

	HealthBar& healthBar = registry.healthBars.emplace(entity);
	healthBar.owner = owner_entity;

	Position& position = registry.positions.emplace(entity);
	position.scale = vec2(RESOURCE_BAR_WIDTH, RESOURCE_BAR_HEIGHT);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HEALTH_BAR,
			EFFECT_ASSET_ID::RESOURCE_BAR,
			GEOMETRY_BUFFER_ID::RESOURCE_BAR });

	return entity;
}

Entity createManaBar(RenderSystem* renderer, Entity& owner_entity)
{
	auto entity = Entity();

	ManaBar& manaBar = registry.manaBars.emplace(entity);
	manaBar.owner = owner_entity;

	Position& position = registry.positions.emplace(entity);
	position.scale = vec2(RESOURCE_BAR_WIDTH, RESOURCE_BAR_HEIGHT);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MANA_BAR,
			EFFECT_ASSET_ID::RESOURCE_BAR,
			GEOMETRY_BUFFER_ID::RESOURCE_BAR });

	return entity;
}

Entity createExitDoor(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::EXIT_DOOR);
	registry.meshPtrs.emplace(entity, &mesh);

	Position& position = registry.positions.emplace(entity);
	position.scale = vec2(100.f, 100.f);
	position.position = vec2(pos.x + position.scale.x/2, pos.y + position.scale.y/2);

	registry.exitDoors.emplace(entity);
	registry.collidables.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::EXIT_DOOR,
			GEOMETRY_BUFFER_ID::EXIT_DOOR});

	return entity;
}

Entity createPowerUpBlock(RenderSystem* renderer, pair<string, bool*>* powerUp) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::EXIT_DOOR);
	registry.meshPtrs.emplace(entity, &mesh);

	SpriteSheet& sprite_sheet = renderer->getSpriteSheet(SPRITE_SHEET_DATA_ID::POWER_UP_BLOCK);
	registry.spriteSheetPtrs.emplace(entity, &sprite_sheet);

	Animation& animation = registry.animations.emplace(entity);
	animation.sprite_sheet_ptr = &sprite_sheet;
	animation.setState((int)POWER_UP_BLOCK_STATES::ACTIVE);
	animation.rainbow_enabled = true;

	Position& position = registry.positions.emplace(entity);
	position.position = vec2(700, 300);
	position.scale = vec2(100.f, 100.f);

	PowerUpBlock& powerUpBlock = registry.powerUpBlock.emplace(entity);
	powerUpBlock.powerUpText = powerUp->first;
	powerUpBlock.powerUpToggle = powerUp->second;

	registry.collidables.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::POWER_UP_BLOCK,
			EFFECT_ASSET_ID::ANIMATED,
			GEOMETRY_BUFFER_ID::POWER_UP_BLOCK });

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

Entity createProjectile(RenderSystem* renderer, vec2 pos, vec2 vel, ElementType elementType, bool hostile, Entity& player) {
	auto entity = Entity();

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.type = elementType;
	projectile.hostile = hostile;

	TEXTURE_ASSET_ID textureAsset;
	GEOMETRY_BUFFER_ID geometryBuffer;
	SPRITE_SHEET_DATA_ID spriteSheet;
	switch (projectile.type) {
		case ElementType::WATER:
			textureAsset = TEXTURE_ASSET_ID::WATER_PROJECTILE_SHEET;
			geometryBuffer = GEOMETRY_BUFFER_ID::WATER_PROJECTILE;
			spriteSheet = SPRITE_SHEET_DATA_ID::WATER_PROJECTILE;
			break;
		case ElementType::FIRE:
			textureAsset = TEXTURE_ASSET_ID::FIRE_PROJECTILE_SHEET;
			geometryBuffer = GEOMETRY_BUFFER_ID::FIRE_PROJECTILE;
			spriteSheet = SPRITE_SHEET_DATA_ID::FIRE_PROJECTILE;
			break;
		case ElementType::EARTH:
			textureAsset = TEXTURE_ASSET_ID::EARTH_PROJECTILE_SHEET;
			geometryBuffer = GEOMETRY_BUFFER_ID::EARTH_PROJECTILE_SHEET;
			spriteSheet = SPRITE_SHEET_DATA_ID::EARTH_PROJECTILE_SHEET;
			break;
		case ElementType::LIGHTNING:
			textureAsset = TEXTURE_ASSET_ID::LIGHTNING_PROJECTILE_SHEET;
			geometryBuffer = GEOMETRY_BUFFER_ID::LIGHTNING_PROJECTILE_SHEET;
			spriteSheet = SPRITE_SHEET_DATA_ID::LIGHTNING_PROJECTILE_SHEET;
			break;
		default:
			textureAsset = TEXTURE_ASSET_ID::WATER_PROJECTILE_SHEET;
			geometryBuffer = GEOMETRY_BUFFER_ID::WATER_PROJECTILE;
			spriteSheet = SPRITE_SHEET_DATA_ID::WATER_PROJECTILE;
			break;
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(geometryBuffer);
	registry.meshPtrs.emplace(entity, &mesh);

	SpriteSheet& sprite_sheet = renderer->getSpriteSheet(spriteSheet);
	registry.spriteSheetPtrs.emplace(entity, &sprite_sheet);

	Animation& animation = registry.animations.emplace(entity);
	animation.sprite_sheet_ptr = &sprite_sheet;
	animation.setState((int)PROJECTILE_STATES::MOVING);

	Velocity& velocity = registry.velocities.emplace(entity);
	velocity.velocity = vel;

	// Set initial position and velocity for the projectile
	Position& position = registry.positions.emplace(entity);
	position.position = pos;
	position.angle = atan2(vel.y, vel.x);
	position.scale = vec2(30.f, 30.f);

	registry.collidables.emplace(entity);
  if (!hostile) {
	  PowerUp& powerUp = registry.powerUps.get(player);
	  if (powerUp.increasedDamage[elementType]) projectile.damage *= 1.5; // increase damage by factor of 1.5
	  if (powerUp.bounceOffWalls[elementType]) projectile.bounces = 2; // allow 2 bounces off walls
  }

	registry.renderRequests.insert(
		entity,
		{	textureAsset,
			EFFECT_ASSET_ID::ANIMATED,
			geometryBuffer });
	return entity;
}

Entity createText(std::string in_text, vec2 pos, float scale, vec3 color)
{
	Entity entity = Entity();

	Position& position = registry.positions.emplace(entity);
	position.position = pos;
	position.scale = vec2(scale, scale);

	Text& text = registry.texts.emplace(entity);
	text.text = in_text;
	text.color = color;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::TEXT_2D,
			GEOMETRY_BUFFER_ID::TEXT_2D });

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