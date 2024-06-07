#include "stack.h"
#include "cell.h"
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    size_t x, y;
    struct node *next;
} node_t;

typedef struct stack
{
    cell_t *cell;
    struct stack *next;
} stack_t;

void push(node_t **stack, size_t x, size_t y)
{
    node_t *node = malloc(sizeof(node_t));
    node->x = x;
    node->y = y;
    node->next = *stack;
    *stack = node;
}

void pop(node_t **stack, size_t *x, size_t *y)
{
    node_t *north = *stack;
    *x = north->x;
    *y = north->y;
    *stack = north->next;
    free(north);
}

bool is_empty(node_t *stack)
{
    return stack == NULL;
}