
typedef struct _queue
{
    cell_t *cell;
    struct _queue *next;
} queue_t;

static queue_t *firstCell;

void enqueue(cell_t *cell)
{
    queue_t *newCell = malloc(sizeof(cell_t));
    newCell->cell = cell;
    newCell->next = NULL;
    if (firstCell == NULL)
    {
        firstCell = newCell;
        return;
    }
    queue_t *p = firstCell;
    queue_t *predCell;
    while (p != NULL)
    {
        predCell = p;
        p = p->next;
    }
    predCell->next = newCell;
}

cell_t *dequeue()
{
    cell_t *removedCell = firstCell->cell;
    queue_t *p = firstCell;
    firstCell = firstCell->next;
    free(p);
    return removedCell;
}