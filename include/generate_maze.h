#ifndef __GENERATE_MAZE_H__
#define __GENERATE_MAZE_H__

#include "cell.h"

/**
 * Initialize and draw the Maze Grid.
 */
void init_grid();

/**
 * Generates maze using DFS.
 * Calls the init_maze function, creates and marks the first cell as visited.
 * Runs the DFS using a stack and it processes the cells until the stack is emptied.
 */
int generate_maze(void *ptr);

#endif // #ifndef __GENERATE_MAZE_H__