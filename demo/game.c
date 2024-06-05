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

const char *BEAVER_PATH = "assets/images/seeking/beaver.png";

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

const vector_t SDL_MIN = {0, 0};
const vector_t SDL_MAX = {1000, 500};
const vector_t SDL_CENTER = {500, 250};
const size_t GRID_WIDTH_H = 25;
const size_t GRID_HEIGHT_H = 12;
const size_t NUM_CELLS_H = GRID_WIDTH_H * GRID_HEIGHT_H;
const int GRID_CELL_SIZE_H = 40;
const rgb_color_t beaver_color = (rgb_color_t){150, 75, 0};

struct state
{
    scene_t *scene;
    size_t page;
    bool maze_generated;
    sound_effect_t *sound_effect;
    seeker_t *seeker;
    list_t *body_assets;
};

static body_t *make_beaver(double w, double h, vector_t center){
    // list_t *beaver_v = list_init(6, free);

    // vector_t *v1 = malloc(sizeof(vector_t));
    // *v1 = (vector_t){0.0, 0.0};
    // list_add(beaver_v, v1);

    // vector_t *v2 = malloc(sizeof(vector_t));
    // *v2 = (vector_t){0.5 * BEAVER_WIDTH, 0.0};
    // list_add(beaver_v, v2);

    // vector_t *v3 = malloc(sizeof(vector_t));
    // *v3 = (vector_t){0.0, 0.5 * BEAVER_HEIGHT};
    // list_add(beaver_v, v3);

    // vector_t *v4 = malloc(sizeof(vector_t));
    // *v4 = (vector_t){0.5 * BEAVER_WIDTH, BEAVER_HEIGHT};
    // list_add(beaver_v, v4);

    // vector_t *v5 = malloc(sizeof(vector_t));
    // *v4 = (vector_t){0.0, BEAVER_HEIGHT};
    // list_add(beaver_v, v5);

    // vector_t *v6 = malloc(sizeof(vector_t));
    // *v5 = (vector_t){-0.5 * BEAVER_WIDTH, 0.5 * BEAVER_HEIGHT};
    // list_add(beaver_v, v6);

    // body_t *beaver = body_init(beaver_v, 1.0, beaver_color);
    // body_set_centroid(beaver, center);
    // return beaver;
    list_t *c = list_init(4, free);
    vector_t *v1 = malloc(sizeof(vector_t));
    *v1 = (vector_t){0 , 0};
    list_add(c, v1);

    vector_t *v2 = malloc(sizeof(vector_t));
    *v2 = (vector_t){w, 0};
    list_add(c, v2);

    vector_t *v3 = malloc(sizeof(vector_t));
    *v3 = (vector_t){w, h};
    list_add(c, v3);

    vector_t *v4 = malloc(sizeof(vector_t));
    *v4 = (vector_t){0, h};
    list_add(c, v4);
    body_t *seeker = body_init(c, 6, beaver_color);
    // body_set_centroid(seeker, center);
    return seeker;
    }


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
    add_new_seeker(state, false);
    state->page = 1;
        vector_t center = (vector_t){.x = (((GRID_WIDTH_H - 24) * GRID_CELL_SIZE_H) + GRID_CELL_SIZE_H / 4), .y = (((GRID_HEIGHT_H - 11) * GRID_CELL_SIZE_H) + GRID_CELL_SIZE_H / 4)};

    body_t *beaver = make_beaver(35, 35, center);
    scene_add_body(state->scene, beaver);

    asset_t *asset_beaver = asset_make_image_with_body(BEAVER_PATH, beaver);
    list_add(state->body_assets, asset_beaver);
    // game_sound(state->sound_effect);

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
        render_seeker_bodies(state);
        // for(size_t i = 0; i < scene_bodies(state->scene); i++) {
        //     body_t *seeker = scene_get_body(state->scene, i);
        //     rgb_color_t *color = body_get_color(seeker);
        //     if(color->r == 0.1 && color->g == 0.9 && color->b == 0.2) {
        //         random_move_seeker(seeker);

        //     }
            
        // }
        sdl_show();
    }
    scene_tick(state->scene, dt);

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
