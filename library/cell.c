#include "cell.h"

#include <stdio.h>
#include <stdlib.h>

const size_t WIDTH = 5;
const size_t HEIGHT = 2;
const size_t CELLS = WIDTH * HEIGHT;

cell_t *get_neighbor(cell_t *current, bool visited[][HEIGHT + 2])
{
    printf("inside 1 se \n");
    cell_t *neighbor = malloc(sizeof(cell_t));

    if ((visited[current->x - 1][current->y] == true) &&
        (visited[current->x][current->y - 1] == true) &&
        (visited[current->x][current->y + 1] == true) &&
        (visited[current->x + 1][current->y] == true))
        return NULL;
    printf("inside 2 se \n");

    do
    {
        int random = (rand() % 4) + 1;
        printf("inside 3 se \n");

        switch (random)
        {
        case 1:
            neighbor->x = current->x - 1;
            neighbor->y = current->y;
            break;
        case 2:
            neighbor->x = current->x;
            neighbor->y = current->y - 1;
            break;
        case 3:
            neighbor->x = current->x;
            neighbor->y = current->y + 1;
            break;
        case 4:
            neighbor->x = current->x + 1;
            neighbor->y = current->y;
            break;
        }
    } while (visited[neighbor->x][neighbor->y] == true);
    printf("inside 4 se \n");

    return neighbor;
}