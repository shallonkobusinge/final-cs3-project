#ifndef __STACK_H__
#define __STACK_H__

#include "cell.h"

typedef struct stack stack_t;
typedef struct node_t Node;

/**
 * Add a new cell to the start of the stack
 * @param head stack to push to
 * @param cell cell to push to stack
 */
// void push_stack(stack_t **head, cell_t *cell);

/**
 * Remove a cell at the start of the stack
 * @param head stack to pop from
 * @param cell cell to pop from stack
 */
// cell_t *pop_stack(stack_t **head);

void push(Node **stack, size_t x, size_t y);

void pop(Node **stack, int *x, int *y);

bool is_empty(Node *stack);

#ifndef __STACK_H__
