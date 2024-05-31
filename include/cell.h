#ifndef __CELL_H__
#define __CELL_H__

#include <stdbool.h>

typedef struct cell cell_t;

/**
 * Updates the adjacency matrix to show current cell and neighbor are directly connected
 * @param cell current cell
 * @param neighbor cell neighboard
 */
void adjacency(cell_t *current, cell_t *neighbor);

/**
 *
 * This function checks if there is a direct connection (adjacency) between two cells, Cell and cellNeighbour.
 * @param cell current cell
 * @param neighbor cell neighboard
 * @return true if there is a direct connection between current and neighbor.
 */
bool isAdjacency(cell_t *current, cell_t *neighbor);

/**
 * Finds a random unvisited neighbor of the current cell.
 * @param cell current cell.
 * @returns NULL if all neighbors are visited. Otherwise, it returns a random unvisited neighbor.
 */
cell_t *get_neighbor(cell_t *current);

#endif // #ifndef __CELL_H__
