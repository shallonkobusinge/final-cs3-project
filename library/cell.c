#include "cell.h"

#include <stdio.h>
#include <stdlib.h>

const size_t WIDTH = 25;
const size_t HEIGHT = 12;
const size_t CELLS = WIDTH * HEIGHT;

void adjacency(cell_t *current, cell_t *neighbor, bool adj_matrix[][CELLS])
{
    int curr_pos = (current->x - 1) * HEIGHT + (current->y - 1);
    int neighbor_pos = (neighbor->x - 1) * HEIGHT + (neighbor->y - 1);
    adj_matrix[curr_pos][neighbor_pos] = true;
    adj_matrix[neighbor_pos][curr_pos] = true;
}

bool isAdjacency(cell_t *current, cell_t *neighbor, bool adj_matrix[][CELLS])
{
    int curr_pos = (current->x - 1) * HEIGHT + (current->y - 1);
    int neighbor_pos = (neighbor->x - 1) * HEIGHT + (neighbor->y - 1);
    return adj_matrix[curr_pos][neighbor_pos];
}

cell_t *get_neighbor(cell_t *current, bool visited[][HEIGHT + 2])
{
    cell_t *neighbor = malloc(sizeof(cell_t));

    if ((visited[current->x - 1][current->y] == true) &&
        (visited[current->x][current->y - 1] == true) &&
        (visited[current->x][current->y + 1] == true) &&
        (visited[current->x + 1][current->y] == true))
        return NULL;
    do
    {
        int random = (rand() % 4) + 1;
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

    return neighbor;
}