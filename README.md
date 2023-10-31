# Aria: Whispers of Darkness
In the eerie village of Grimhaven, shrouded in perpetual mist and shadows, Aria's journey begins. 

# Milestone 2
Below is the documentation for our `README.md` submission for Milestone 2.

## Entry Points
- [Mesh Based Collision](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/main/src/physics_system.cpp#L36)
  - Physics System first goes through a broad phase collision check (AABB Collision Detection) followed by a narrow phase collision check which uses line segment 
    intersection. (Draw a line from entity's mid point to each vertex and check if it collides with the edge of another entity and vice versa for other entity)
- Basic User Tutorial/Help
  - Text is provided in the very first level (tutorial level) to inform the user what can be done in the game.
- External Integration of FreeType Library
  - [Modified CMake](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/main/CMakeLists.txt#L70)
  - [FreeType Load](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/main/src/render_system_init.cpp#L66)
  - [Character Render](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/main/src/render_system.cpp#L87)

## Development Plan
Most of the work in this milestone aligned with our development plan for this milestone.

### Aligned with the proposal:

### Discreprancies:

## ECS Design Pattern
The ECS design pattern has not been changed since Milestone 1.

## Game Entities
- Player
- Enemy
- Terrain
- Exit Door

### New Game Entities:
- Projectile
- Text
- Health Bar

## Actionable Components
- Velocity
- Position
- Collision
- Direction
- Invulnerable Timer
- Death Timer
- Resources
- Mesh
- Render Request
- Screen State
- Color

### New Actionable Components
- Projectile
- ElementType
- CharacterProjectileType
- Collidable

## Entity Component Diagram
Highlight any changes versus the previous milestone.
<!-- ![ECS diagram](docu/images/M1_ECS_diagram.png) -->
