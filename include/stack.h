#ifndef __STACK_H__
#define __STACK_H__

#include "cell.h"

typedef struct stack stack_t;

/**
 * Add a new cell to the start of the stack
 * @param cell cell to push to stack
 */ 
void push_stack(cell_t *cell);


/**
 * Remove a cell at the start of the stack
 * @param cell cell to pop from stack
 */ 
cell_t *pop_stack();

#endif // #ifndef __STACK_H__
