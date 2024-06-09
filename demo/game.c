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

const size_t STARTING_SEEKERS = 50;

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

static void load_game_screen(state_t *state)
{
    printf("here\n");
    state->page = 2;
}

btn_element_t btn_elements[] = {
    {
        .text.frame = (SDL_Rect){MAX.x - 20, CENTER.y + 45, 90, 48},
        .text.font_path = "assets/fonts/Inter-Regular.ttf",
        .text.color = (rgb_color_t){0, 0, 0},
        .text.text = "PLAY",
        .img.file_path = "assets/images/landing-page/play_btn.png",
        .img.frame = (SDL_Rect){MAX.x - 50, CENTER.y + 30, 200, 80},
        .handler = (void *)load_game_screen,
    },
};

state_t *emscripten_init()
{
    asset_cache_init();
    sdl_init(SDL_MIN, SDL_MAX);
    init_sound();
    state_t *state = malloc(sizeof(state_t));
    state->scene = scene_init();
    state->page = 1;
    state->maze_state = maze_init();
    state->landing_page_state = landing_page_init(&btn_elements);
    state->sound_effect = sound_effect_init();
    state->body_assets = list_init(STARTING_SEEKERS, (free_func_t)asset_destroy);
    state->seeker = seeker_init(state);
    game_sound(state->sound_effect);

    return state;
}

bool emscripten_main(state_t *state)
{
    double dt = time_since_last_tick();
    sdl_clear();

    if (state->page == 1)
    {
        show_landing_page(state->landing_page_state);
    }
    else if (state->page == 2)
    {
        show_maze(state, dt);
    }
    scene_tick(state->scene, dt);

    sdl_show();

    return false;
}

void emscripten_free(state_t *state)
{
    list_free(state->body_assets);
    seeker_free(state->seeker);
    scene_free(state->scene);
    sound_free(state->sound_effect);
    asset_cache_destroy();
    free(state);
}
