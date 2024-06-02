
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "cell.h"

typedef struct queue queue_t;

/**
 * Add a new cell to the end of a queue
 * @param cell cell to add to queue
 */ 
void enqueue(cell_t *cell);

/**
 * Remove a cell at the start of the queue 
 */
cell_t *dequeue();


#endif // #ifndef __QUEUE_H__
