#ifndef __GENERATE_MAZE_H__
#define __GENERATE_MAZE_H__

#include "sdl_wrapper.h"


/**
 * Initialize and draw the Maze Grid.
*/
void init_grid();

/**
 * Initializes the maze by marking all cells as unvisited and setting the border cells as visited.
 * and initializes the adjacency matrix and sets the random seed.
 */
static void init_maze();

/**
 * Removes the wall between the current cell and its neighbor.
 * Checks whether the neighbor is in the same row or column and draws a line to remove the wall. 
 * @param cell current cell
 * @param neighbor cell neighbor 
 */ 
void remove_wall(cell_t *cell, cell_t *neighbor);

/**
 * Generates maze using DFS.
 * Calls the init_maze function, creates and marks the first cell as visited.
 * Runs the DFS using a stack and it processes the cells until the stack is emptied.
 */
void generate_maze();

#endif // #ifndef __GENERATE_MAZE_H__