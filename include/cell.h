#ifndef __CELL_H__
#define __CELL_H__

#include <stdbool.h>
#include <stdlib.h>
#include "sdl_wrapper.h"

extern const size_t WIDTH;
extern const size_t HEIGHT;
extern const size_t CELLS;

typedef struct cell
{
    SDL_Rect box;
    bool west, north, east, south;
    bool visited, start, end;
} cell_t;

// /**
//  * Finds a random unvisited neighbor of the current cell.
//  * @param cell current cell.
//  * @returns NULL if all neighbors are visited. Otherwise, it returns a random unvisited neighbor.
//  */
// cell_t *get_neighbor(cell_t *current, bool visited[][HEIGHT + 2]);

#endif // #ifndef __CELL_H__
