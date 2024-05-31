typedef struct stack stack_t


/**
 * 
void push_stack(cell_t *cell);


cell_t *pop_stack()
{
    cell_t *removed = first_cell->cell;
    stack_t *p = first_cell;
    first_cell = first_cell->next;
    free(p);
    return removed;
}