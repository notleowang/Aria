# Aria: Whispers of Darkness
In the eerie village of Grimhaven, shrouded in perpetual mist and shadows, Aria's journey begins. 

# Milestone 2
Below is the documentation for our `README.md` submission for Milestone 2.

## Entry Points
- Game Logic Response to User Input: TODO
- Sprite Sheet Animation: 
[Animation Component](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/005cfe61bd0082a3ecf530d97725b540bb8545e0/src/components.hpp#L196),
[Handling Effect](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/005cfe61bd0082a3ecf530d97725b540bb8545e0/src/render_system.cpp#L79),
[Advancing Frames](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/005cfe61bd0082a3ecf530d97725b540bb8545e0/src/render_system.cpp#L309),
[Shader](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/005cfe61bd0082a3ecf530d97725b540bb8545e0/shaders/animated.fs.glsl),
[Animation Sequences](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/14e8f386b75c6a21ea42ea23c1cf00e87eade535/src/world_init.cpp#L274) (the projectiles with the ANIMATED effect asset id are animation sequences using different textures)
- New integrated assets: 
[Health/Mana bar texture](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/005cfe61bd0082a3ecf530d97725b540bb8545e0/data/textures/health_bar.png), 
[New Projectile Textures](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/005cfe61bd0082a3ecf530d97725b540bb8545e0/data/textures/water_projectile_spritesheet.png) (the link takes you to one of the new projectile textures, the rest are in the textures folder)
- [Mesh Based Collision](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/main/src/physics_system.cpp#L36)
  - Physics System first goes through a broad phase collision check (AABB Collision Detection) followed by a narrow phase collision check which uses line segment 
    intersection. (Draw a line from entity's mid point to each vertex and check if it collides with the edge of another entity and vice versa for other entity)
- Basic User Tutorial/Help
  - Text is provided in the very first level (tutorial level) to inform the user what can be done in the game.
  - Text is also provided on each power-up level prompting the player to shoot at the block to receive a power-up.
- [Consistent Game Resolution](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/a02a00206804e162dfe492ea22aa1a741ab3b5fd/src/world_system.cpp#L75): Consistent resolution is set on line 83
- Simple Rendering Effects: TODO
- External Integration of FreeType Library
  - [Modified CMake](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/main/CMakeLists.txt#L70)
  - [FreeType Load](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/main/src/render_system_init.cpp#L66)
  - [Character Render](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/main/src/render_system.cpp#L87)
- 2 Minutes of Non-Repetitive Gameplay:
[New Game Levels](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/a02a00206804e162dfe492ea22aa1a741ab3b5fd/src/game_level.cpp), [Power-Ups](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/a02a00206804e162dfe492ea22aa1a741ab3b5fd/src/components.hpp#L32)

## Development Plan
Most of the work in this milestone aligned with our development plan for this milestone.

### Aligned with the proposal:
- Implemented new levels.
- Enhanced enemy ai.
- New sprites for enemeies.
- UI sprites (health bar).
- Projectile sprite animations.
- Finished basic tutorial concept level.
- Added power-ups.
- Added weaknesses to enemies.
- Incorporated mana.
- Added additional audio feedback.

### Discreprancies:
- No sprite animations for player yet.

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
- Floor
- PowerUpBlock

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
- Text
- CharacterProjectileType
- Collidable
- Win Timer
- Animation
- Power Up

## Entity Component Diagram
Highlight any changes versus the previous milestone.
![ECS diagram](docu/images/M2_ECS_diagram.png)
