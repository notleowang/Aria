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
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

bool rectCollides(const Position& position1, const Position& position2, Entity& entity_i, Entity& entity_j)
{
	float rect1_left = position1.position.x - abs(position1.scale.x) / 2;
	float rect1_right = position1.position.x + abs(position1.scale.x) / 2;
	float rect1_top = position1.position.y - abs(position1.scale.y) / 2;
	float rect1_bottom = position1.position.y + abs(position1.scale.y) / 2;
	float rect2_left = position2.position.x - abs(position2.scale.x) / 2;
	float rect2_right = position2.position.x + abs(position2.scale.x) / 2;
	float rect2_top = position2.position.y - abs(position2.scale.y) / 2;
	float rect2_bottom = position2.position.y + abs(position2.scale.y) / 2;
	if (rect1_left < rect2_right && rect1_right > rect2_left &&
		rect1_bottom > rect2_top &&
		rect1_top < rect2_bottom
		)
	{
		float overlap_left = rect1_right - rect2_left;
		float overlap_right = rect2_right - rect1_left;
		float overlap_top = rect1_bottom - rect2_top;
		float overlap_bottom = rect2_bottom - rect1_top;

		float min_overlap = min(overlap_left, min(overlap_right, min(overlap_top, overlap_bottom)));

		int direction;
		if (min_overlap == overlap_left) {
			direction = 0;
		}
		else if (min_overlap == overlap_right) {
			direction = 1;
		}
		else if (min_overlap == overlap_top) {
			direction = 2;
		}
		else if (min_overlap == overlap_bottom) {
			direction = 3;
		}
		registry.collisions.emplace_with_duplicates(entity_i, entity_j, direction);
		return true;
	}
	return false;
}

void PhysicsSystem::step(float elapsed_ms)
{
	// Move fish based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& velocity_container = registry.velocities;
	for (uint i = 0; i < velocity_container.size(); i++)
	{
		Velocity& velocity = velocity_container.components[i];
		auto& position_container = registry.positions;
		Entity entity = velocity_container.entities[i];			// Grab the i-th entity with the Velocity Component
		Position& position = position_container.get(entity);	// Get the position for the i-th entity
		float step_seconds = elapsed_ms / 1000.f;
		if (registry.collisions.has(entity)) {
			Collision& collision = registry.collisions.get(entity);
			if (collision.direction == 0) {
				position.position.x -= 1; // to remove stacked collisions
				position.position.y += velocity.velocity.y * step_seconds;
			}
			if (collision.direction == 1) {
				position.position.x += 1;
				position.position.y += velocity.velocity.y * step_seconds;
			}
			if (collision.direction == 2) {
				position.position.x += velocity.velocity.x * step_seconds;
				position.position.y -= 1;
			}
			if (collision.direction == 3) {
				position.position.x += velocity.velocity.x * step_seconds;
				position.position.y += 1;
			}
			registry.collisions.remove(entity);
		}
		else {
			position.position += velocity.velocity * step_seconds;
		}
	}

	// Check for collisions with entities that have a velocity
	for (uint i = 0; i < velocity_container.size(); i++) 
	{
		auto& position_container = registry.positions;
		Entity entity_i = velocity_container.entities[i];
		Position& position_i = position_container.get(entity_i);

		// Currently pretty inefficient as there are some duplicate checks but having separate velocity and position make it a bit difficult
		// We could also just do over all entities with a position component... but we'll consider it if this becomes too inefficient
		for (uint j = 0; j < position_container.components.size(); j++) {
			Position& position_j = position_container.components[j];
			if (&position_j != &position_i) {
				Entity& entity_j = position_container.entities[j];
				rectCollides(position_i, position_j, entity_i, entity_j);
			}

		}
	}
}