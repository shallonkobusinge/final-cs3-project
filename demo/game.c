#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "landing_page.h"
#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

const int grid_cell_size = 40;
const int grid_width = 25;
const int grid_height = 12;
const int window_width = (grid_width * grid_cell_size) + 1;
const int window_height = (grid_height * grid_cell_size) + 1;
const int number_of_cells = grid_width * grid_height;

struct state
{
    size_t page;
};

void init_grid()
{
    // draw_color((rgb_color_t){22, 22, 22});
    draw_color((rgb_color_t){210, 210, 210});

    for (int x = 0; x < window_width; x += grid_cell_size)
    {
        render_line(x, 0, x, window_height);
    }
    for (int y = 0; y < window_height; y += grid_cell_size)
    {
        render_line(0, y, window_width, y);
    }

    SDL_Rect start_cell = {(grid_cell_size / 4), (grid_cell_size / 4), (grid_cell_size / 2), (grid_cell_size / 2)};
    draw_color((rgb_color_t){0, 0, 0});

    render_rect(&start_cell);

    SDL_Rect terminal_cell;
    terminal_cell.x = ((grid_width - 1) * grid_cell_size) + grid_cell_size / 4;
    terminal_cell.y = ((grid_height - 1) * grid_cell_size) + grid_cell_size / 4;
    terminal_cell.w = grid_cell_size / 2;
    terminal_cell.h = grid_cell_size / 2;

    render_rect(&terminal_cell);
}

state_t *emscripten_init()
{
    asset_cache_init();
    sdl_init(MIN, MAX);
    state_t *state = malloc(sizeof(state_t));
    state->page = 0;

    return state;
}

bool emscripten_main(state_t *state)
{
    sdl_clear();
    init_grid();
    sdl_show();
    return false;
}

void emscripten_free(state_t *state)
{
    asset_cache_destroy();
    free(state);
}
