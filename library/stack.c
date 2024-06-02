#include "stack.h"
#include "cell.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct stack
{
    cell_t *cell;
    struct stack *next;
} stack_t;

void push_stack(stack_t *head, cell_t *cell)
{
    stack_t *stack = malloc(sizeof(stack_t));
    stack->cell = cell;
    stack->next = head;
    head = stack;
}

cell_t *pop_stack(stack_t *head)
{
    cell_t *removed = head->cell;
    stack_t *temp = head;
    head = head->next;
    free(temp);
    return removed;
}