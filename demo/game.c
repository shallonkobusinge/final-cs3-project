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

const vector_t START_POS = {500, 45};
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


body_t *make_seeker(double outer_radius, double inner_radius, vector_t center) {
    center = vec_add(VEC_ZERO, center);
    center.y += inner_radius;
    list_t *shape = list_init(S_NUM_POINTS, free);
    for (size_t i = 0; i < S_NUM_POINTS; i++){
        double angle = 2 * M_PI * i / S_NUM_POINTS;
        vector_t *vert = malloc(sizeof(*vert));
        *vert = (vector_t) {.x = center.x + outer_radius * cos(angle),
                            .y = center.y + inner_radius * cos(angle)};
        list_add(shape, vert);
    }
    body_t *seeker_b = body_init(shape, 10.0, seeker_color);
    return seeker_b;
}


state_t *emscripten_init() {
    sdl_init(MIN, MAX);
    state_t *state = malloc(sizeof(state_t));
    asset_cache_init();
    state->scene = scene_init();
    state->body_assets = list_init(MAX_SEEKERS, (free_func_t)asset_destroy);
    state->last_seeker_time = 0;
    // for(int i = 0; i < STARTING_SEEKERS; i++) {
        body_t *seeker = make_seeker(OUTER_RADIUS, INNER_RADIUS, START_POS);
        // vector_t vel = {.x = rand() % 200 - 100, .y = rand() % 200 - 100 };
        body_set_velocity(seeker, (vector_t){100, 0});
        // body_set_centroid(seeker, START_POS);
        scene_add_body(state->scene, seeker);
        asset_t *asset_seeker = asset_make_image_with_body(SEEKER_PATH, seeker);
        list_add(state->body_assets, asset_seeker);
    // }
    
    return state;
}

// double introduce_new_seeker(state_t *state, double previous_time, double current_time){
//     if (current_time - previous_time >= NEW_SEEKERS_INTERVAL) {
//         if(list_size(state->body_assets) < MAX_SEEKERS) {
//         body_t *new_seeker = make_seeker(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
//         scene_add_body(state->scene, new_seeker);
//         asset_t *asset_seeker = asset_make_image_with_body(SEEKER_PATH, new_seeker);
//         list_add(state->body_assets, asset_seeker);
//     }
      
// }
//     previous_time = current_time;
//     return previous_time;
// }

// void get_new_velocity_seeker(body_t *seeker, double dt) {
//     vector_t velocity = body_get_velocity(seeker);
//     vector_t displacement = vec_multiply(dt, velocity);
//     vector_t new_poisition = vec_add(body_get_centroid(seeker), displacement);

//     if(new_poisition.x < MIN.x || new_poisition.x > MAX.x) {
//         velocity.x = -velocity.x;
//     }
//     if(new_poisition.y < MIN.y || new_poisition.y > MIN.y) {
//         velocity.x = -velocity.y;
//     }
//     // vector_t center = vec_add(body_get_centroid(seeker), vec_multiply(dt, velocity));
//     body_set_velocity(seeker, velocity);
//     // body_set_centroid(seeker, center);
//     body_set_rotation(seeker,  3 * M_PI / 2);

// }

bool emscripten_main(state_t *state) {
    sdl_clear();
    double dt = time_since_last_tick();
   
    for(size_t i = 0; i < scene_bodies(state->scene); i++) {
        body_t *seeker = scene_get_body(state->scene, i);
        get_new_velocity_seeker(seeker, dt);

    }
     list_t *assets_b = state->body_assets;
    for (size_t i = 0; i < list_size(state->body_assets); i++) {
        asset_render(list_get(assets_b, i));
        }
    // double previous_time = 0;

    // previous_time = introduce_new_seeker(state, previous_time, dt);
    scene_tick(state->scene, dt);
    sdl_show();
  return false;
}

void emscripten_free(state_t *state) {
  list_free(state->body_assets);
  scene_free(state->scene);
  asset_cache_destroy();
  free(state);
}