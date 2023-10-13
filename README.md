# Aria: Whispers of Darkness
In the eerie village of Grimhaven, shrouded in perpetual mist and shadows, Aria's journey begins. 

## Milestone 1
### Entry Points
- [Movement Controls](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/0d59974eadc6cf1e6482af4443dfe1ed5fba9fb9/src/world_system.cpp#L270)
- Camera Controls: [Camera struct](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/d37c69f8169b5c6068eda0682ba48fa821cc33ef/src/common.hpp#L51),  [function call and projection matrix](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/d37c69f8169b5c6068eda0682ba48fa821cc33ef/src/render_system.cpp#L205), [function definition](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/d37c69f8169b5c6068eda0682ba48fa821cc33ef/src/common.cpp#L24)
- [Collisions](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/blob/0d59974eadc6cf1e6482af4443dfe1ed5fba9fb9/src/world_system.cpp#L217)

### Development Plan
- Your submission should align with your proposed development plan: Provide a write-up explaining how your milestone aligns with the plan. Explain all discrepancies and submit an updated proposal when such discrepancies occur.

### ECS Design Pattern
The ECS design pattern we used was the one provided by A1 which is..

### Game Entities
- Player
- Enemy
- Terrain
- Exit Door

### Actionable Components
- Velocity
- Position
- Collision
- Direction
- Invulnerable Death Timer
- Death Timer
- Resource
- Mesh
- Render Request
- Screen State
- Color

### Entity Component Diagram
![ECS diagram](docu/images/M1_ECS_diagram.png)
