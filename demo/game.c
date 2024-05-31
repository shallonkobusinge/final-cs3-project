#include "stdbool.h"
#include "asset.h"
#include "sdl_wrapper.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "vector.h"
#include "assert.h"
#include "asset_cache.h"
#include "seeker.h"
#include "sound_effect.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};


typedef struct state {
    scene_t *scene;
    seeker_t *seeker;
    sound_effect_t *sound_effect;
}state_t;


state_t *emscripten_init() {
    asset_cache_init();
    sdl_init(MIN, MAX);
    init_sound();
    state_t *state = malloc(sizeof(state_t));
    state->scene = scene_init();
    state->sound_effect = load_game_sounds();
    state->seeker = seeker_init(state->scene);
    game_sound(state->sound_effect);
    return state;
}

bool emscripten_main(state_t *state) {

    double dt = time_since_last_tick();
    introduce_seeker(state->scene, state->seeker, dt, state->sound_effect);
    sdl_clear();
    render_seeker_bodies(state->seeker);
    
    for(size_t i = 0; i < scene_bodies(state->scene); i++) {
        body_t *seeker = scene_get_body(state->scene, i);
        wrap_seeker_scene(seeker);
    }
    sdl_show();
    scene_tick(state->scene, dt);
  return false;
}

void emscripten_free(state_t *state) {
  free_seeker(state->seeker);
  scene_free(state->scene);
  free_sound(state->sound_effect);
  asset_cache_destroy();
  free(state);
}