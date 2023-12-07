# Game Balance

## Enemy AI:

- One balance fix was that the enemies' speed was reduced when they were dodging the player's projectiles. This was done in [this commit](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/commit/3b575567193e4f74545f4c330c49f724153f0bf8#diff-3000781566c743b7082cc7fb39eb123341cd39ad26b73ead94f7c4e0c5623335R57), which reduced the speed from 350 to 300. Initially the speed was set to 200, then I discovered that the enemies were too easy, so I adjusted it to 350. Then the enemies were too frustrating to beat as they dodged too quickly, so I reduced it to 300. 

## Boss AI:

- The speed of the projectiles shot by the bosses had to be decreased as they were too difficult to dodge at full speed. It was set to 0.4x the normal speed initially, then I updated it to 0.5x the normal speed in [this commit](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/commit/a198d13eea120e3f4007b7cf36be950903ad1cc50), as I experienced a loss of the game's momentum at 0.4x speed (i.e. it felt boring).

- A lot of adjustments were made for the amount that the bosses heal. I originally envisioned players being able to kill a boss with no power ups after one cycle of the bosses' phases. I did not want players to be able to easily kill the bosses before the first cycle is over, so that they can experience all the attacking patterns the bosses have. However, the player must also be able to do net damage each cycle, otherwise the bosses are simply not beatable. I eventually arrived at a sweet spot of healing 625 HP over 1250ms in [this commit](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/commit/d89c36e2c21041abf3c8eba956afde08d7069f6d).

- Some phases require precise movement from the players to dodge the projectiles. I had to also adjust the width of the opening during some phases so that the players must navigate carefully (but should not take damage if they move correctly). In the phase where the bosses generate projectiles around the player, I had to try different values for the size of the opening and the speed at which the projectiles move, and eventually I arrived at a maximum velocity of 200 initially then slowing down to 150, in [this commit](https://github.students.cs.ubc.ca/CPSC427-2023W-T1/Team06Aria/commit/7bf22ea6eec6ac26eeedcdbadf018feaff0d3164).

- I thought it was unfair for the bosses to be able to dodge projectiles, as players sometimes need to be in specific positions (due to dodging the bosses' projectiles), so the boss movement was made simpler - and the bosses will never chase the players either.