# SUMMARY

## TITLE: Trash Trivia
## MEMBERS:
    1. |DivinIrakiza|[Gitlab](https://gitlab.caltech.edu/dirakiza)
    2. |ShallonKobusinge|[Gitlab](https://gitlab.caltech.edu/skobusin)
## CONCEPT STATEMENT: Trivia gets trashy! Sort recyclables and make the environment greener. Answer questions on sustainability & win! 

# GAMEPLAY
This is a visual objects trivia game. Initially, the game's title appears on the screen for about 5 seconds, followed by the game settings screen. This screen features buttons like "Choose Object Speed," where players can select their preferred speed using a mouse click, or “Choose Environment” where players can choose what environment favors them. If no values are selected for these options, the game loads default backgrounds and sounds relevant to sustainability themes.

In the game, two bins labeled "Recyclable" and "Trash" are fixed at the bottom of the game window. Various objects, such as water bottles, batteries, ice cream, headphones, and bags, fall from the top of the screen. Players must sort these objects into the correct bins using the left and right arrow keys. There will be a pause action that can be triggered by either clicking the pause icon of the game window or a space bar.

On the top-right corner of the screen displays the player's score. Each correctly placed object earns 1 point, displays "CORRECT," and triggers an affirmation sound. Incorrectly placed objects deduct 1 point, display "WRONG," and trigger an annoying sound.  The game ends when all objects have fallen, and the final score is displayed.  When the game ends, a summary of the user's performance, including the number of correct answers and any educational messages about the wrong answers. The game will then allow the user to restart the game or exit.

Our physics engine will use vectors to represent velocity, acceleration, position, and other related quantities. Additionally, the engine will take care of the vector graphics velocity, center of mass, rotation and more. The engine will also make collisions such that when a collision occurs that means a user drops an object in a certain bin. A Color struct will be used to manage any color changes throughout the game. The game will implement the SDL library for rendering text and images.



