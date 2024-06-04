#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "landing_page.h"
#include "generate_maze.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"
#include "sound_effect.h"
#include "seeker.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

const vector_t SDL_MIN = {0, 0};
const vector_t SDL_MAX = {1000, 500};
const vector_t SDL_CENTER = {500, 250};

struct state
{
    scene_t *scene;
    size_t page;
    bool maze_generated;
    sound_effect_t *sound_effect;
    seeker_t *seeker;
    
};

state_t *emscripten_init()
{
    asset_cache_init();
    sdl_init(SDL_MIN, SDL_MAX);
    init_sound();
    state_t *state = malloc(sizeof(state_t));
    state->scene = scene_init();
    state->maze_generated = false;
    state->sound_effect = load_game_sounds();
    state->seeker = seeker_init(state->scene);
      SDL_Rect seeker_cell = {((GRID_WIDTH_S - 2) * GRID_CELL_SIZE_S) + GRID_CELL_SIZE_S / 4, ((GRID_WIDTH_S - 3) * GRID_CELL_SIZE_S) + GRID_CELL_SIZE_S / 4, GRID_CELL_SIZE_S / 2, GRID_CELL_SIZE_S / 2}; 
  render_color((rgb_color_t){241, 108, 45});
  render_rect(&seeker_cell);
    state->page = 1;

    game_sound(state->sound_effect);

    return state;
}

bool emscripten_main(state_t *state)
{
    double dt = time_since_last_tick();
    
    sdl_clear();
    if (state->page == 0)
    {
        build_landing_page();
    }
    else if (state->page == 1)
    {
        // introduce_seeker(state->scene, state->seeker, dt, state->sound_effect);
        if (!state->maze_generated)
        {
            state->maze_generated = generate_maze(state);
        }
        sdl_show();
    }
    scene_tick(state->scene, dt);

    return false;
}

void emscripten_free(state_t *state)
{
    scene_free(state->scene);
    asset_cache_destroy();
    free(state);
}
