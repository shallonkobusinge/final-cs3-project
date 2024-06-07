#ifndef __STACK_H__
#define __STACK_H__

#include "cell.h"

typedef struct stack stack_t;
typedef struct node node_t;

/**
 * Push a new node onto the stack.
 * Allocates memory for a new node, sets its coordinates to the given (x, y) values,
 * and pushes it onto the stack.
 *
 * @param stack A double pointer to the top of the stack. The stack is implemented as a linked list.
 * @param x The x-coordinate to store in the new node.
 * @param y The y-coordinate to store in the new node.
 */
void push(node_t **stack, size_t x, size_t y);

/**
 * Pop a node from the stack.
 * Removes the top node from the stack, retrieves its coordinates, and frees the
 * memory allocated for the node.
 *
 * @param stack A double pointer to the top of the stack. The stack is implemented as a linked list.
 * @param x A pointer to a size_t variable where the x-coordinate of the popped node will be stored.
 * @param y A pointer to a size_t variable where the y-coordinate of the popped node will be stored.
 *
 */
void pop(node_t **stack, size_t *x, size_t *y);

/**
 * Checks if the stack is empty by determining if the stack pointer is NULL.
 *
 * @param stack A pointer to the top of the stack. The stack is implemented as a linked list.
 * @return true if the stack is empty, false otherwise.
 */
bool is_empty(node_t *stack);

#endif // #ifndef __STACK_H__
