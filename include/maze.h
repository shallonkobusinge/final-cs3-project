#ifndef __GENERATE_MAZE_H__
#define __GENERATE_MAZE_H__

#include "cell.h"
#include "state.h"

typedef struct maze_state maze_state_t;

/**
 * Generates maze using DFS.
 * Calls the init_maze function, creates and marks the first cell as visited.
 * Runs the DFS using a stack and it processes the cells until the stack is emptied.
 */
void show_maze(state_t *state, double dt);

/**
 * Initializes maze components and generates maze using DFS.
 * Calls the init_maze function, creates and marks the first cell as visited.
 * Runs the DFS using a stack and it processes the cells until the stack is emptied.
 * @return state of the maze
 */
maze_state_t *maze_init();

void translate_body_movement(state_t *state, body_t *body, int movement_dir);

#endif // #ifndef __MAZE_H__