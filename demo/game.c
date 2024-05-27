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

const size_t SHIP_NUM_POINTS = 20;
const double OUTER_RADIUS = 15;
const double INNER_RADIUS = 15;
const vector_t START_POS = {500, 30};
const int16_t H_STEP = 20;
const int16_t V_STEP = 40;
const double EXTRA_VEL_MULT = 10;
const double VEL_MULT_PROB = 0.2;
const vector_t RESET_POS = {500, 45};

const size_t ROWS = 8;

const rgb_color_t seeker_color = (rgb_color_t){0.1, 0.9, 0.2};

const char *SEEKER_PATH = "assets/seeker.png";


typedef struct state {
    list_t *body_assets;
    scene_t *scene;
}state_t;


body_t *make_seeker(double outer_radius, double inner_radius, vector_t center) {
  center.y += inner_radius;
  list_t *c = list_init(SHIP_NUM_POINTS, free);
  for (size_t i = 0; i < SHIP_NUM_POINTS; i++) {
    double angle = 2 * M_PI * i / SHIP_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + inner_radius * cos(angle),
                    center.y + outer_radius * sin(angle)};
    list_add(c, v);
  }
  body_t *froggy = body_init(c, 1, seeker_color);
  return froggy;
}

// void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
//   body_t *froggy = scene_get_body(state->scene, 0);
//   vector_t translation = (vector_t){0, 0};
//   if (type == KEY_PRESSED && type != KEY_RELEASED) {
//     switch (key) {
//     case LEFT_ARROW:
//       translation.x = -H_STEP;
//       break;
//     case RIGHT_ARROW:
//       translation.x = H_STEP;
//       break;
//     case UP_ARROW:
//       translation.y = V_STEP;
//       break;
//     case DOWN_ARROW:
//       if (body_get_centroid(froggy).y > START_POS.y) {
//         translation.y = -V_STEP;
//       }
//       break;
//     }
//     vector_t new_centroid = vec_add(body_get_centroid(froggy), translation);
//     body_set_centroid(froggy, new_centroid);
//   }
// }
state_t *emscripten_init() {
    sdl_init(MIN, MAX);
    state_t *state = malloc(sizeof(state_t));
    asset_cache_init();
    state->scene = scene_init();
    state->body_assets = list_init(MAX_SEEKERS, (free_func_t)asset_destroy);
    body_t *seeker = make_seeker(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
    dy_set_centroid(seeker, RESET_POS);
    scene_add_body(state->scene, seeker);
    asset_t *asset_seeker = asset_make_image_with_body(SEEKER_PATH, seeker);
    list_add(state->body_assets, asset_seeker);
    // sdl_on_key((key_handler_t)on_key);
    return state;
}

double introduce_new_seeker(state_t *state, double previous_time, double current_time){
    if (current_time - previous_time >= NEW_SEEKERS_INTERVAL) {
        if(list_size(state->body_assets) < MAX_SEEKERS) {
        body_t *new_seeker = make_seeker(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
        scene_add_body(state->scene, new_seeker);
        asset_t *asset_seeker = asset_make_image_with_body(SEEKER_PATH, new_seeker);
        list_add(state->body_assets, asset_seeker);
    }
      
}
    previous_time = current_time;
    return previous_time;
}

bool emscripten_main(state_t *state) {
    sdl_clear();
    double dt = time_since_last_tick();
    list_t *assets_b = state->body_assets;
    for (size_t i = 0; i < list_size(state->body_assets); i++){
        asset_render(list_get(assets_b, i));
        }
    double previous_time = 0;
    previous_time = introduce_new_seeker(state, previous_time, dt);
    // printf("PREVIOUS %f CURRENT TIME %f \n", previous_time, dt);
    sdl_show();

//   
//   scene_tick(state->scene, dt);
  return false;
}

void emscripten_free(state_t *state) {
  list_free(state->body_assets);
  scene_free(state->scene);
  asset_cache_destroy();
  free(state);
}