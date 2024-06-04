#ifndef __GENERATE_MAZE_H__
#define __GENERATE_MAZE_H__

#include "cell.h"
#include "state.h"

/**
 * Generates maze using DFS.
 * Calls the init_maze function, creates and marks the first cell as visited.
 * Runs the DFS using a stack and it processes the cells until the stack is emptied.
 */
bool generate_maze(state_t *state);

#endif // #ifndef __GENERATE_MAZE_H__