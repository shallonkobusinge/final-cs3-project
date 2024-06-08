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

#include "asset_cache.h"
#include "sdl_wrapper.h"
#include "landing_page.h"
#include "maze.h"
#include "sound_effect.h"
#include "seeker.h"

#define STARTING_SEEKERS 10

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

const vector_t SDL_MIN = {0, 0};
const vector_t SDL_MAX = {1000, 500};
const vector_t SDL_CENTER = {500, 250};

typedef struct maze_state maze_state_t;
typedef struct landing_page_state landing_page_state_t;
typedef struct seeker seeker_t;
typedef struct sound_effect sound_effect_t;

struct state
{
    scene_t *scene;
    size_t page;
    maze_state_t *maze_state;
    landing_page_state_t *landing_page_state;
    sound_effect_t *sound_effect;
    seeker_t *seeker;
    list_t *body_assets;
};

state_t *emscripten_init()
{
    asset_cache_init();
    sdl_init(SDL_MIN, SDL_MAX);
    init_sound();
    state_t *state = malloc(sizeof(state_t));
    state->scene = scene_init();
    state->page = 2;
    state->maze_state = maze_init();
    state->landing_page_state = landing_page_init();
    state->sound_effect = load_game_sounds();
    state->body_assets = list_init(STARTING_SEEKERS, (free_func_t)asset_destroy);
    state->seeker = seeker_init(state);
    hider_init(state);
    add_new_seeker(state, false);
    game_sound(state->sound_effect);

    return state;
}

bool emscripten_main(state_t *state)
{
    double dt = time_since_last_tick();
    // render_seeker(state, dt);
    //      for(size_t i = 1; i < scene_bodies(state->scene); i++) {
    //         body_t *seeker = scene_get_body(state->scene, i);
    //         rgb_color_t *color = body_get_color(seeker);
    //         if(color->r == 0.1 && color->g == 0.9 && color->b == 0.2) {
    //             random_move_seeker(seeker);
    //         }
            
    //     }
    sdl_clear();

    if (state->page == 1)
    {
        show_landing_page(state->landing_page_state);
    }
    else if (state->page == 2)
    {
        show_maze(state->maze_state);
         render_seeker_bodies(state);
    }
    scene_tick(state->scene, dt);

    sdl_show();

    return false;
}

void emscripten_free(state_t *state)
{   list_free(state->body_assets);
    seeker_free(state->seeker);
    scene_free(state->scene);
    sound_free(state->sound_effect);
    asset_cache_destroy();
    free(state);
}