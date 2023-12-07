# Aria: Whispers of Darkness
In the eerie village of Grimhaven, shrouded in perpetual mist and shadows, Aria's journey begins. 

# Milestone 4
Below is the documentation for our `README.md` submission for Milestone 4.

## User-game interactions
### Changes implemented + rationale
**Observation**: Users found it difficult to remember which power-ups were currently active and there was no visual indicator at all for �increase damage� power-ups.\
**Change**: Added visual indicator icons for the active power-ups to the element selection display

**Observation**: The information about which enemies are weak to which element was not something our users were able to intuitively guess. 
The fact that enemies are healed by their own element was often overlooked.\
**Change**: Added new tutorial level with description of the weaknesses and an infographic showing which element to use on which. It also includes 4 enemies on which to test the elemental projectiles on such that the user can see the differences in damage dealt and enemies healing.

**Observation**: Some users felt that using 1,2,3,4 as hotkeys to change the player�s attack element was sometimes awkward as they use the same hand to move the player using WASD.\
**Change**: In addition to the 1,2,3,4 hotkeys, we provide two additional means of changing the attack element: scroll wheel will cycle through the elements and right-click will increment the player�s current attack element.

**Observation**: Player health and mana bars cluttered the screen.\
**Change**: Moved the player health and mana bars to the top-left hand corner so as not to obscure the action in the middle of the screen.

**Observation**:  Boss health bar often off-screen because of the size of the boss room.\
**Change**: Moved boss health bar to bottom center screen so as to always be visible.

**Observation**: If a user needed to step away from the game mid-level, there was no option to pause.\
**Change**: Added pause menu (ESC)

**Observation**:  To restart the game, it was necessary to kill the program and completely restart it in Visual Studio.\
**Change**: Added pause menu (bound to ESC key) which allows user to exit to the main menu and click �Start Game� for a fresh restart

**Observation**:  Exiting the program was previously bound to �backspace� key, which felt inelegant and was not obvious.\
**Change**: Added pause menu which allows user to exit to the main menu and click �Quit Game� to stop the program.

**Observation**: Gloomy lighting effect caused visibility issues during gameplay. It was difficult to see enemies on the edges of the screen and overall too dim to be visually appealing.\
**Change**: Increased the radius of visibility on the lighting effect.

**Observation**: No indicator to the player once they have killed the last enemy that the exit door has now appeared if they player isn�t in view of the door.\
**Change**: Added a sound effect when exit door appears

## Entry Points
### Advanced Story Elements
- Cutscenes (LINKS HERE TODO)

### Advanced Integrated Assets
- Wide variety of custom sprites (all except those listed below)
- Custom voice lines (see cutscenes)
- [Original music](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/commit/80679e84a270223560993bac1a3ad5ee82e85859)

Below we have included sources for all assets used in the game that were not custom made by one of our team members (replacement of unusable assets as well as proper attributions are incoming for cross-play)

Outsourced images in /data/textures
(Source: https://twitter.com/erasorenart/status/1373668994650144773; Twitter user @EraSorenArt, will be replaced before cross-play)
- dungeon_floor.png
- dungeon_tile.png
- generic_dungeon_wall.png
- north_dungeon_wall.png
- side_dungeon_wall.png
- south_dungeon_wall.png

Outsourced audio files in /data/audio
(Source: opengameart.org; proper attributions coming)
- aria_death.wav
- cutscene_1_background.wav
- eerie_ambience.wav
- end_level.wav
- enemy_death.wav
- fast_pace_background.wav
- last_enemy_death.wav
- obstacle_collision.wav
- portal.wav
- scream.wav
(Source: freesound.org; proper attributions coming)
- boss_battle.wav
- boss_battle_intro.wav
- damage_tick.wav
- final_boss_battle.wav
- final_boss_battle_intro.wav
- power_up.wav
- projectile.wav
(CC0 license no attribution required)
- heal.wav

## Game balance
### Changes implemented + rationale
**Observation**: Triple-shot power-up was unanimously considered the most powerful power-up and dramatically decreased 
the time required to kill enemies relative to the other power-ups.\
**Change**: Decreased the damage dealt by one projectile from a triple-shot to 40%. 40% was chosen after play-testing other values.

**Observation**: The current levels we had were a bit too monotonous and since the layout was quite similar, this made the gameplay straightforward and thus quite easy.\
**Change**: We added new and revamped old levels so that it became a bit harder. We also have a logical ordering of levels in terms of difficulty so that as the player progresses through the game, it becomes a bit more difficult.

**Observation**: Boss fights were quite unbalanced in our previous version of the game. 
Their health values were quite low which made fighting the boss not really feel like fighting a boss. In addition to this, the boss AI was too easy (similar to small enemies).\
**Change**: We increased the amount of health the bosses had and we also made bosses have different ai than smaller enemies by implementing attack patterns.

**Observation**: We noticed that players would sometimes die during a level as enemies slowly dwindled down the player�s health and players had no form of gaining back health.\
**Change**: We implemented health packs throughout some difficult levels so the player can be more successful in completing the level.

## Development Plan
Most of the work in this milestone aligned with our development plan for this milestone.

### Aligned with the proposal:
- Balanced game
- Tutorial implementation finished
- Fixed all bugs from previous milestones
- Finished sprites
- Added many new sounds (music and sound effects)

### Discreprancies:
- Did not do parallax background scrolling creative component
- Decided to use advanced integrated assets creative component

## ECS Design Pattern
The ECS design pattern has not been changed since Milestone 1.

## Game Entities
- Player
- Enemy
- Terrain
- Exit Door
- Projectile
- Text
- Health Bar
- Floor
- PowerUpBlock
- Shadow
- Obstacle
- Boss

### New Game Entities:

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
- Projectile
- Text
- CharacterProjectileType
- Collidable
- Win Timer
- Animation
- Power Up
- Follower

### New Actionable Components
- WeaknessTimer

## Entity Component Diagram
Highlight any changes versus the previous milestone.
![ECS diagram](docu/images/M4_ECS_diagram.png)
