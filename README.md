
# The Caltech Hideout
## SUMMARY


### Members

[Divin Irakiza](https://gitlab.caltech.edu/dirakiza)  <br>[Shallon Kobusinge](https://gitlab.caltech.edu/skobusin)

  

### Concept Note

Navigate a maze of Caltech buildings, avoiding seekers at every turn. Outsmart them, hide cleverly, and reach your target unscathed.

  
  

## GAMEPLAY
This is a visual hide and seek game with a constructed maze with some of caltech buildings as target locations. The hider is on a mission to reach the target building.

The game begins with an informative screen displaying  instructions on how to play. This initial screen includes a "Play Game" button that, when clicked, transitions to the mission briefing screen. Here, the player is presented with the name and image of the target Caltech building, which they must reach to win the game. Clicking "Start Game" brings the player to the main game screen. On this screen, a randomly generated maze is presented, with a hider represented as a Caltech beaver and seekers designed as robots. The hider starts at the topmost cell of the maze, while the target buildings and seekers are placed randomly. Two timers are displayed at the bottom of the screen: one showing the remaining time of 2 minutes to reach the target building and another indicating the time until the next seeker is added.

In the main game, the hider navigates the maze using the arrow keys. The seeker's movements are controlled by the computer, progressively becoming more challenging as new seekers are added every 30 seconds. If the hider is tagged by a seeker or fails to reach the target building within the remaining time, the game ends, displaying a screen that shows the total time used, the number of seekers, and options to retry the game or return to the main page. Also, if the hider successfully reaches the target building, a victory screen appears with similar details, celebrating the achievement. 

### FEATURES

| #      | Feature | Description | Priority | Member |
| ----------- | ----------- |   ----------- |   ----------- |   ----------- | 
| 1      | Maze Generation        | - Randomly generate maze. <br> | 1 | Divin |
| 2| Maze Traversal  | - Maze collisions.  <br> - Object traversal through maze.| 1 | Shallon | 
|3 | Hiding| - Manage the hider. <br> - Handle hider's keyboard movements.  | 2 | Divin  |
|4 | Seeking| - Manage the seeker. <br> - Seeker movement through maze. <br> - Manage multiple seekers | 3 | Shallon  |
|5 | Hider Tagging | - Hider & Seeker collision. <br> | 3 | Shallon  
|6 | Maze Missions | - Create caltech buildings. <br>- Choose target building.| 3 | Divin |
|7 | Scenery | - Game instruction screen <br> - Game screen. | 4 | Divin  |
|4 | Timing | - Display 2 minute countdown <br> - Displaying 30 seconds timer until next seeker. (Divin). | 4 | Divin  |
|4 | Sound effects | - Background sounds. <br> - Make sound on tag. | 4 | Shallon  |

- **Maze Generation**: The random maze generation is implemented using the DFS algorithm and it deals with the placement of walls, paths, and open spaces to create a challenging environment.  <br>
- **Maze Traversal**: The traversal prevents the player (hider and seeker) from passing through walls and ensure that movements within the maze are responsive. The collisions are managed in real-time to provide a good experience. <br>
- **Hiding**: The hider's movements are controlled via keyboard inputs, with the arrow keys for navigation. <br>
- **Seekers**: Seekers are computer driven characters that navigate the maze in search of the hider. Their movement patterns become more complex and aggressive over time, increasing the difficulty. The game adds a new seeker every 30 seconds.<br>
- **Hidder tagging**: This feature detects when a seeker tags the hider, triggering the end of the game. The collision detection is precise, to make sure the game is fair. <br>
- **Maze missions**: Each mission involves reaching a some Caltech building, displayed at the start of the game. The selection of target buildings is randomized to keep the missions fresh and challenging. <br>
- **Scenery**: This includes the game instruction screen, which provides players with essential information before starting, the main game screen, which displays the maze, and the timer.<br>
- **Timing**: A 2-minute countdown timer challenges the hider to reach the target quickly, while a secondary timer adds a new seeker every 30 seconds. <br>
- **Sound effects**: Background music sets the mood, and we also provide audio cues for actions such as tagging and mission accomplishment. <br>
  
## TIMELINE
| Week 1 | Member |  Features | 
| ------ | ----- | ------ | 
| Week 1      |  Divin |  - Hiding. <br> - Scenery |
| Week 1      |  Shallon |  - Seeking. <br> - Hider Tagging |
| Week 2      |  Divin |  - Maze Generation |
| Week 2      |  Shallon |  - Maze traversal. |
| Week 3      |  Divin |  - Maze missions <br> - Timing |
| Week 3      |  Shallon | - Sound Effects. |

  

## DISASTER RECOVERY

<u>Shallon Kobusinge</u>: If I fall behind, before we start implementing I plan to explain our game design to my section TA “Max” such that incase I get stuck he can be the second hand I go to for clarification or ideas on how to push back along with consulting my teammate for any issue that might arise.

  

<u>Divin Irakiza</u>: Falling behind is not on my bucket list. However, misfortunes do happen. When this happens, I plan to consult my teammate on what to do inorder not to get her stuck.

  

<u>Joint Statement</u>: We have put most of our important features in Week 1 and Week 2 so that when unforeseen circumstances happen, there is some time in Week 3 to catch up. We are also eager to start the next task once one is finished in order to be ahead of time as a way to be in the safe zone when disaster occurs. We are planning to use a task management tool (Jira or Trello or Pivotal Tracker) for a more collaborative experience.
