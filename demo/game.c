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
    state->seeker = seeker_init(state);
    add_new_seeker(state);
    state->page = 1;

    game_sound(state->sound_effect);

    return state;
}

bool emscripten_main(state_t *state)
{
    double dt = time_since_last_tick();
    render_seeker(state, dt);
    sdl_clear();
    if (state->page == 0)
    {
        build_landing_page();
    }
    else if (state->page == 1)
    {
        
        if (!state->maze_generated)
        {
            state->maze_generated = generate_maze(state, dt);
        }
        render_seeker_bodies(state->seeker);
        for(size_t i = 0; i < scene_bodies(state->scene); i++) {
            body_t *seeker = scene_get_body(state->scene, i);
            rgb_color_t *color = body_get_color(seeker);
            if(color->r == 0.1 && color->g == 0.9 && color->b == 0.2) {
                random_move_seeker(seeker);

            }
            
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
