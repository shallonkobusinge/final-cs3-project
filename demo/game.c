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

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};

// SEEKING CONSTANTS
#define STARTING_SEEKERS 1
#define MAX_SEEKERS 50
#define NEW_SEEKERS_INTERVAL 30
#define S_NUM_POINTS 20
#define S_RADIUS 0.1

const vector_t START_POS = {0, 45};
const int16_t H_STEP = 20;
const int16_t V_STEP = 40;
const double OUTER_RADIUS = 60;
const double INNER_RADIUS = 60;

const rgb_color_t seeker_color = (rgb_color_t){0.1, 0.9, 0.2};

const char *SEEKER_PATH = "assets/seeker_bg.png";


typedef struct state {
    list_t *body_assets;
    scene_t *scene;
    double last_seeker_time;
}state_t;


body_t *make_seeker(double w, double h, vector_t center) {
    list_t *c = list_init(4, free);
  vector_t *v1 = malloc(sizeof(vector_t));
  *v1 = (vector_t){0, 0};
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
  body_t *obstacle = body_init(c, 1, seeker_color);
  body_set_centroid(obstacle, center);
  return obstacle;
}

void wrap_edges(body_t *seeker) {
  vector_t centroid = body_get_centroid(seeker);
  vector_t velocity = body_get_velocity(seeker);
  
  if (centroid.x >= MAX.x || centroid.x <= MIN.x) {
        velocity.x = -velocity.x;
  }
  if(centroid.y >= MAX.y || centroid.y <= MIN.y) {
    velocity.y = -velocity.y;
  }
  body_set_velocity(seeker, velocity);
}

state_t *emscripten_init() {
     asset_cache_init();
    sdl_init(MIN, MAX);
    state_t *state = malloc(sizeof(state_t));
    srand(time(NULL));
    state->scene = scene_init();
    state->body_assets = list_init(MAX_SEEKERS, (free_func_t)asset_destroy);
    state->last_seeker_time = 0;
    body_t *seeker = make_seeker(OUTER_RADIUS, INNER_RADIUS, START_POS);
    body_set_velocity(seeker, (vector_t){.x = 60, .y = 20});
    scene_add_body(state->scene, seeker);
    asset_t *asset_seeker = asset_make_image_with_body(SEEKER_PATH, seeker);
    list_add(state->body_assets, asset_seeker);
    
    return state;
}

bool emscripten_main(state_t *state) {

    double dt = time_since_last_tick();
    for(size_t i = 0; i < scene_bodies(state->scene); i++) {
        body_t *seeker = scene_get_body(state->scene, i);
        wrap_edges(seeker);
    }
    sdl_clear();
    for (size_t i = 0; i < list_size(state->body_assets); i++) {
        asset_render(list_get(state->body_assets, i));
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