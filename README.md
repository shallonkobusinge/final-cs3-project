# The Caltech Hideout
## SUMMARY
  

### Concept Note

Navigate a maze of Caltech buildings, avoiding seekers at every turn. Outsmart them, hide cleverly, and reach your target unscathed.

  
  

## GAMEPLAY
This is a visual hide and seek game with a constructed maze with some of caltech buildings as target locations. The hider is on a mission to reach the target building.

The game begins with an informative screen displaying  instructions on how to play. This initial screen includes a "Play Game" button that, when clicked, transitions to the mission briefing screen. Here, the player is presented with the name and image of the target Caltech building, which they must reach to win the game. Clicking "Start Game" brings the player to the main game screen. On this screen, a randomly generated maze is presented, with a hider represented as a Caltech beaver and seekers designed as robots. The hider starts at the topmost cell of the maze, while the target buildings and seekers are placed randomly. Two timers are displayed at the bottom of the screen: one showing the remaining time of 2 minutes to reach the target building and another indicating the time until the next seeker is added.

In the main game, the hider navigates the maze using the arrow keys. The seeker's movements are controlled by the computer, progressively becoming more challenging as new seekers are added every 30 seconds. If the hider is tagged by a seeker or fails to reach the target building within the remaining time, the game ends, displaying a screen that shows the total time used, the number of seekers, and options to retry the game or return to the main page. Also, if the hider successfully reaches the target building, a victory screen appears with similar details, celebrating the achievement. 



- **Maze Generation**: The random maze generation is implemented using the DFS algorithm and it deals with the placement of walls, paths, and open spaces to create a challenging environment.  <br>
- **Maze Traversal**: The traversal prevents the player (hider and seeker) from passing through walls and ensure that movements within the maze are responsive. The collisions are managed in real-time to provide a good experience. <br>
- **Hiding**: The hider's movements are controlled via keyboard inputs, with the arrow keys for navigation. <br>
- **Seekers**: Seekers are computer driven characters that navigate the maze in search of the hider. Their movement patterns become more complex and aggressive over time, increasing the difficulty. The game adds a new seeker every 30 seconds.<br>
- **Hidder tagging**: This feature detects when a seeker tags the hider, triggering the end of the game. The collision detection is precise, to make sure the game is fair. <br>
- **Maze missions**: Each mission involves reaching a some Caltech building, displayed at the start of the game. The selection of target buildings is randomized to keep the missions fresh and challenging. <br>
- **Scenery**: This includes the game instruction screen, which provides players with essential information before starting, the main game screen, which displays the maze, and the timer.<br>
- **Timing**: A 2-minute countdown timer challenges the hider to reach the target quickly, while a secondary timer adds a new seeker every 30 seconds. <br>
- **Sound effects**: Background music sets the mood, and we also provide audio cues for actions such as tagging and mission accomplishment. <br>

  
  
