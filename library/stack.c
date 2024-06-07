#include "stack.h"
#include "cell.h"
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    size_t x, y;
    struct Node *next;
} Node;

typedef struct stack
{
    cell_t *cell;
    struct stack *next;
} stack_t;

void push(Node **stack, size_t x, size_t y)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->x = x;
    node->y = y;
    node->next = *stack;
    *stack = node;
}

void pop(Node **stack, int *x, int *y)
{
    Node *north = *stack;
    *x = north->x;
    *y = north->y;
    *stack = north->next;
    free(north);
}

bool is_empty(Node *stack)
{
    return stack == NULL;
}