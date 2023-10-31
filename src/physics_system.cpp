// internal
#include "physics_system.hpp"
#include "world_init.hpp"

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Position& position)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(position.scale.x), abs(position.scale.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool circleCollides(const Position& position1, const Position& position2)
{
	vec2 dp = position1.position - position2.position;
	float dist_squared = dot(dp,dp);
	const vec2 other_bonding_box = get_bounding_box(position1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(position2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = std::max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

vec2 worldTransform(vec2 coords, Position& position) {
	// !!! TODO: add rotation
	return vec2(coords.x * position.scale.x + position.position.x, coords.y * position.scale.y + position.position.y);
}

// Reference: 
// https://github.com/OneLoneCoder/Javidx9/blob/master/PixelGameEngine/SmallerProjects/OneLoneCoder_PGE_PolygonCollisions1.cpp?fbclid=IwAR1e0EyRPtFFGmg1EuiiKU9JxBwOAFN42YA3LIvfm0GHspBbE1df43ZeCz8
void diagonalCollides(Entity& ent_i, Entity& ent_j) 
{
	Entity* entity_i = &ent_i;
	Entity* entity_j = &ent_j;
	bool flag = false;

	for (int obj = 0; obj < 2; obj++) {
		if (obj == 1) {
			entity_i = &ent_j;
			entity_j = &ent_i;
		}
		std::vector<ColoredVertex> i_vertices = registry.meshPtrs.get(*entity_i)->vertices;
		std::vector<ColoredVertex> j_vertices = registry.meshPtrs.get(*entity_j)->vertices;
		// figure how to get vertices of things also made for textures

		Position& position_i = registry.positions.get(*entity_i);
		Position& position_j = registry.positions.get(*entity_j);

		for (uint i = 0; i < i_vertices.size(); i++) 
		{
			vec2 i_line_start = position_i.position;
			// Will need to add rotation into transformation as well
			vec2 i_line_end = worldTransform(vec2(i_vertices[i].position.x, i_vertices[i].position.y), position_i);
			vec2 displacement = { 0, 0 };

			for (uint j = 0; j < j_vertices.size(); j++) {
				uint next_point = (j + 1) % j_vertices.size();		
				vec2 j_line_start = worldTransform(vec2(j_vertices[j].position.x, j_vertices[j].position.y), position_j);
				vec2 j_line_end = worldTransform(vec2(j_vertices[next_point].position.x, j_vertices[next_point].position.y), position_j);
				
				// Line segment intersection
				float h = (j_line_end.x - j_line_start.x) * (i_line_start.y - i_line_end.y) - (i_line_start.x - i_line_end.x) * (j_line_end.y - j_line_start.y);
				float t = ((j_line_start.y - j_line_end.y) * (i_line_start.x - j_line_start.x) + (j_line_end.x - j_line_start.x) * (i_line_start.y - j_line_start.y)) / h;
				float r = ((i_line_start.y - i_line_end.y) * (i_line_start.x - j_line_start.x) + (i_line_end.x - i_line_start.x) * (i_line_start.y - j_line_start.y)) / h;

				if (t >= 0.0f && t < 1.0f && r >= 0.0f && r < 1.0f) 
				{
					displacement.x += (1.0f - t) * (i_line_end.x - i_line_start.x);
					displacement.y += (1.0f - t) * (i_line_end.y - i_line_start.y);
					flag = true;
				}
			}
			if (flag) {
				// ent_i is obj = 0 meaning it penetrated ent_j, so displacement is negative so we pull back ent_i's position
				registry.collisions.emplace_with_duplicates(ent_i, ent_j, (obj == 0) ? -displacement : displacement);
				// ent_j is obj = 0 meaning it penetrated ent_i, so displacement is positive to push away ent_i
				registry.collisions.emplace_with_duplicates(ent_j, ent_i, (obj == 0) ? displacement : -displacement);
				return;
			}
		}
	}
	return;
}

// Function that checks if two entities collides using an axis-aligned bounding-box implementation.
// If there is a collision between two entities, add a Collision component to the Component container
// marking both entities involved in the collision and the direction which entity i hit entity j from.
bool AABBCollides(Entity& entity_i, Entity& entity_j)
{
	auto& position_container = registry.positions;
	Position& position_i = position_container.get(entity_i);
	Position& position_j = position_container.get(entity_j);

	float i_left = position_i.position.x - abs(position_i.scale.x) / 2;
	float i_right = position_i.position.x + abs(position_i.scale.x) / 2;
	float i_top = position_i.position.y - abs(position_i.scale.y) / 2;
	float i_bottom = position_i.position.y + abs(position_i.scale.y) / 2;
	float j_left = position_j.position.x - abs(position_j.scale.x) / 2;
	float j_right = position_j.position.x + abs(position_j.scale.x) / 2;
	float j_top = position_j.position.y - abs(position_j.scale.y) / 2;
	float j_bottom = position_j.position.y + abs(position_j.scale.y) / 2;

	if (i_left < j_right && i_right > j_left && i_bottom > j_top && i_top < j_bottom)
	{
		return true;
	}
	return false;
}

// Shouldn't care if terrain-terrain and exitDoor-terrain collisions happen
bool shouldIgnoreCollision(Entity& entity_i, Entity& entity_j) 
{
	if ((registry.terrain.has(entity_i) && registry.terrain.has(entity_j)) || 
		(registry.terrain.has(entity_i) && registry.exitDoors.has(entity_j)) ||
		(registry.exitDoors.has(entity_i) && registry.terrain.has(entity_j))) {
		return true;
	}
	return false;
}

void PhysicsSystem::step(float elapsed_ms)
{
	auto& velocity_container = registry.velocities;
	for (uint i = 0; i < velocity_container.size(); i++)
	{
		Velocity& velocity = velocity_container.components[i];
		Entity entity = velocity_container.entities[i];
		Position& position = registry.positions.get(entity);
		float step_seconds = elapsed_ms / 1000.f;
		position.prev_position = position.position;
		position.position[0] += step_seconds * velocity.velocity[0];
		position.position[1] += step_seconds * velocity.velocity[1];
	}

	// Check for collisions between things that are collidable
	auto& collidables_container = registry.collidables;
	for (uint i = 0; i < collidables_container.size(); i++) {
		Entity& entity_i = collidables_container.entities[i];
		for (uint j = i+1; j < collidables_container.size(); j++) {
			Entity& entity_j = collidables_container.entities[j];
			// Ignore terrain-terrain and terrain-exitDoor collision
			//if (shouldIgnoreCollision(entity_i, entity_j)) continue;
			// Broad phase of collision check
			if (AABBCollides(entity_i, entity_j)) {
				// Narrow phase of collision check
				diagonalCollides(entity_i, entity_j);
			}

		}
	}

	// Position the health bars
	auto& healthBarContainer = registry.healthBars;
	for (int i = 0; i < healthBarContainer.size(); i++) {
		HealthBar& healthBar = healthBarContainer.components[i];
		Entity entity = healthBarContainer.entities[i];

		Position& position = registry.positions.get(entity);
		Position& ownerPosition = registry.positions.get(healthBar.owner);

		position.position.x = ownerPosition.position.x;
		position.position.y = ownerPosition.position.y + healthBar.y_offset;
	}
	
	// Position the mana bars
	auto& manaBarContainer = registry.manaBars;
	for (int i = 0; i < manaBarContainer.size(); i++) {
		ManaBar& manaBar = manaBarContainer.components[i];
		Entity entity = manaBarContainer.entities[i];

		Position& position = registry.positions.get(entity);
		Position& ownerPosition = registry.positions.get(manaBar.owner);

		position.position.x = ownerPosition.position.x;
		position.position.y = ownerPosition.position.y + manaBar.y_offset;
	}
}