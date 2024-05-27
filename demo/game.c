#include "stdbool.h"
#include "asset.h"
#include "sdl_wrapper.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "vector.h"
#include "assert.h"
#include "asset_cache.h"
#include "seeker.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const double OUTER_RADIUS = 60;
const double INNER_RADIUS = 60;
const vector_t START_POS = {0, 45};
const vector_t INITIAL_VELOCITY = {60, 20};
const char *SEEKER_PATH = "assets/seeker_bg.png";

#define NEW_SEEKERS_INTERVAL 30
#define MAX_SEEKERS 50
typedef struct state {
    list_t *body_assets;
    scene_t *scene;
    double last_seeker_time;
}state_t;

void add_new_seeker(state_t *state, bool is_new){

   vector_t seeker_vel = {.x = 0.0, .y = 0.0};
   body_t *seeker;
    if(is_new){
      vector_t seeker_pos = (vector_t){
        .x = rand() % (int)INITIAL_VELOCITY.x,
        .y = rand() % (int)(MAX.y),
    };
     seeker_vel = (vector_t){
        .x = rand() % (int)INITIAL_VELOCITY.x + 20,
        .y = rand() % (int)INITIAL_VELOCITY.y + 10
    };
      seeker = make_seeker(OUTER_RADIUS, INNER_RADIUS, seeker_pos);
    }
    seeker = make_seeker(OUTER_RADIUS, INNER_RADIUS, START_POS);
    // body_set_velocity(seeker, INITIAL_VELOCITY);
    seeker_vel = INITIAL_VELOCITY;
  
    scene_add_body(state->scene, seeker);
    body_set_velocity(seeker, seeker_vel);
    asset_t *new_asset_seeker = asset_make_image_with_body(SEEKER_PATH, seeker);
    list_add(state->body_assets, new_asset_seeker);
    state->last_seeker_time = 0;

}

state_t *emscripten_init() {
     asset_cache_init();
    sdl_init(MIN, MAX);
    state_t *state = malloc(sizeof(state_t));
    srand(time(NULL));
    state->scene = scene_init();
    state->body_assets = list_init(MAX_SEEKERS, (free_func_t)asset_destroy);
    add_new_seeker(state, false);
    // state->last_seeker_time = 0;
    // body_t *seeker = make_seeker(OUTER_RADIUS, INNER_RADIUS, START_POS);
    // body_set_velocity(seeker, INITIAL_VELOCITY);

    // scene_add_body(state->scene, seeker);
    // asset_t *asset_seeker = asset_make_image_with_body(SEEKER_PATH, seeker);
    // list_add(state->body_assets, asset_seeker);
    
    
    return state;
}


bool emscripten_main(state_t *state) {

    double dt = time_since_last_tick();
   
    state->last_seeker_time += dt;
    if(state->last_seeker_time >= NEW_SEEKERS_INTERVAL){
      add_new_seeker(state, true);
    }
    printf(" ELAPSED TIME %f \n", state->last_seeker_time);
    sdl_clear();
    for (size_t i = 0; i < list_size(state->body_assets); i++) {
        asset_render(list_get(state->body_assets, i));
        }
    for(size_t i = 0; i < scene_bodies(state->scene); i++) {
        body_t *seeker = scene_get_body(state->scene, i);
        wrap_seeker_scene(seeker);
    }
    sdl_show();
    scene_tick(state->scene, dt);
  return false;
}

void emscripten_free(state_t *state) {
  list_free(state->body_assets);
  scene_free(state->scene);
  asset_cache_destroy();
  free(state);
}