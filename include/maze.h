#ifndef __GENERATE_MAZE_H__
#define __GENERATE_MAZE_H__

#include "cell.h"
#include "state.h"

typedef struct maze_state maze_state_t;

/**
 * Generates maze using DFS.
 * Calls the init_maze function, creates and marks the first cell as visited.
 * Runs the DFS using a stack and it processes the cells until the stack is emptied.
 * @param state state of game
 * @param dt elapsed time
 */
void show_maze(state_t *state, double dt);

/**
 * Show target building for an instance of a game play
 * @param state state of game
 */
void show_mission(state_t *state);

/**
 * Initializes maze components and generates maze using DFS.
 * Calls the init_maze function, creates and marks the first cell as visited.
 * Runs the DFS using a stack and it processes the cells until the stack is emptied.
 * @return state of the maze
 */
maze_state_t *maze_init();

/**
 * Traverse traverse in the body's current.
 * Finds the exact cell of the body in the maze.
 * Finds the cell's open wall and return the direction.
 * @param state state struct of the game
 * @param vector current body centroid of the body.
 * @param movement_direction movement direction of the body.
 *              0 -> North, 1 -> West, 2 -> South , 3 -> East, -1 -> seleect a random movement.
 * 
 * @return rotation direction vector.
 */
vector_t traverse_maze(state_t *state, vector_t vector, size_t movement_direction);

/**
 * Randomly moves only the seeker bodies in the scene.
 * @param state state struct of the game.s
*/
void seekers_random_movement(state_t *state);

#endif // #ifndef __MAZE_H__