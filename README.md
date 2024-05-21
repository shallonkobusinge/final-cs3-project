# Trash Trivia

## SUMMARY

### Members 
[Divin Irakiza](https://gitlab.caltech.edu/dirakiza) <br>[Shallon Kobusinge](https://gitlab.caltech.edu/skobusin)

### Concept Note
Trivia gets trashy! Sort recyclables and make the environment greener. Answer questions on sustainability & win! 

## GAMEPLAY
This is a visual objects trivia game. Initially, the game's title appears on the screen for about 5 seconds, followed by the game settings screen. This screen features buttons like "Choose Object Speed," where players can select their preferred speed using a mouse click, or “Choose Environment” where players can choose what environment favors them. If no values are selected for these options, the game loads default backgrounds and sounds relevant to sustainability themes.

In the game, two bins labeled "Recyclable" and "Trash" are fixed at the bottom of the game window. Various objects, such as water bottles, batteries, ice cream, headphones, and bags, fall from the top of the screen. Players must sort these objects into the correct bins using the left and right arrow keys. There will be a pause action that can be triggered by either clicking the pause icon of the game window or a space bar.

On the top-right corner of the screen displays the player's score. Each correctly placed object earns 1 point, displays "CORRECT," and triggers an affirmation sound. Incorrectly placed objects deduct 1 point, display "WRONG," and trigger an annoying sound.  The game ends when all objects have fallen, and the final score is displayed.  When the game ends, a summary of the user's performance, including the number of correct answers and any educational messages about the wrong answers. The game will then allow the user to restart the game or exit.

Our physics engine will use vectors to represent velocity, acceleration, position, and other related quantities. Additionally, the engine will take care of the vector graphics velocity, center of mass, rotation and more. The engine will also make collisions such that when a collision occurs that means a user drops an object in a certain bin. A Color struct will be used to manage any color changes throughout the game. The game will implement the SDL library for rendering text and images.

## FEATURE SET
| #      | Feature | Description | Priority | Member |
| ----------- | ----------- |   ----------- |   ----------- |   ----------- | 
| 1      | Physics engine        | - Implementing vectors to represent velocity, acceleration and position of moving objects. <br> - Collision detection. | 1 | Divin |
| 2|  Game window initialization | - Initialization of game window and loading background images related to sustainability themes. | 1 | Shallon | 
|3 | Object Rendering | - Attaching images of trash items and bins (Shallon). <br> - Displaying the name of the trash items along with trash items (Divin). | 1 | Divin, Shallon  |
|4 | Keyboard handling | - Handling user’s keyboard movements of objects to the correct bin. | 2 | Divin |
| 5 | Score management | - Keeping track of the user’s points based on their long and correct answers | 2 | Shallon |
| 6 | Scene management | - Handling different scenes in the game including the start screen, game play, end screen. | 3 | Shallon |
| 7 | Feedback | - Displaying education messages, feedback to the user based on their correct and incorrect answers from the game. | 3 | Divin | 
| 8 | Game screen customization. | - Handling “Choose object speed” and “Choose background environment” options. <br> - Mouse handling for setting preference values.  <br> -  Pause the game on the space bar and click the button. | 4| Divin |
| 9 | Sound effects. | Playing background music | 4 | Shallon |


### IMPLEMENTATION STEPS
1. Setup the SDL environment
2. Developing the scene manager
3. Developing the keyboard handler and scene movement
4. Integrating the physics engine for object movement and collision detection.
5. Attaching images to game objects
6. Render text and titles
7. Adding scoring system
8. Providing feedback
9. Add game customization
10. Add sound effects
11. Test and debug (As we go)

## TIMELINE
| Week 1 | Member |  Features | 
| ------ | ----- | ------ | 
| Week 1      |  Divin |  - Physics Engine. <br> - Keyboard Handling. |
| Week 1      |  Shallon |  - Game window initialization. <br> - Scene management. |
| Week 2      |  Divin |  - Object rendering: Rendering titles to trash items. <br> - Collision Detection. |
| Week 2      |  Shallon |  - Object rendering: Attaching images to the bodies. <br> - Score management. |
| Week 3      |  Divin |  - Game screen customization. <br> - Feedback |
| Week 3      |  Shallon | - Sound effects. |


## DISASTER RECOVERY
<u>Shallon Kobusinge</u>: If I fall behind, before we start implementing I plan to explain our game design to my section TA “Max” such that incase I get stuck he can be the second hand I go to for clarification or ideas on how to push back along with consulting my teammate for any issue that might arise. 

<u>Divin Irakiza</u>: Falling behind is not on my bucket list. However, misfortunes do happen. When this happens, I plan to consult my teammate on what to do inorder not to get her stuck. 

<u>Joint Statement</u>:  We have put most of our important features in Week 1 and Week 2 so that when unforeseen circumstances happen, there is some time in Week 3 to catch up. We are also eager to start the next task once one is finished in order to be ahead of time as a way to be in the safe zone when disaster occurs. We are planning to use a task management tool (Jira or Trello or Pivotal Tracker) for a more collaborative experience.




