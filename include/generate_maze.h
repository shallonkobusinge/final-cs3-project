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
bool generate_maze(maze_state_t *maze_state);

maze_state_t *maze_init();

#endif // #ifndef __GENERATE_MAZE_H__