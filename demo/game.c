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

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const double OUTER_RADIUS = 60;
const double INNER_RADIUS = 60;
const vector_t START_POS = {0, 45};
const vector_t INITIAL_VELOCITY = {60, 20};
const char *SEEKER_PATH = "assets/seeker_bg.png";
const char *TAGGED_SOUND_PATH = "assets/sound_effects/tagged.wav";
const char *GAME_SOUND_EFFECT = "assets/sound_effects/hide_and_seek.wav";

#define NEW_SEEKERS_INTERVAL 30

typedef struct sound_effect {
  Mix_Music *game_sound;
  Mix_Chunk *tagged_sound;
} sound_effect_t;

typedef struct state {
    list_t *body_assets;
    scene_t *scene;
    double last_seeker_time;
    double max_seekers;
    sound_effect_t sound_effect;
}state_t;

void init_sound() {
  if(SDL_Init(SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initiliaze! SDL_mixer Error: %s \n", Mix_GetError());
    exit(1);
  }
  if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    printf("SDL_mixer could not initiliaze! SDL_mixer Error: %s \n", Mix_GetError());
    exit(1);
  }
}

sound_effect_t load_game_sounds() {
  sound_effect_t *sounds = malloc(sizeof(sound_effect_t));
  sounds->game_sound = Mix_LoadMUS(GAME_SOUND_EFFECT);
  if(sounds->game_sound == NULL) {
    printf("Failed to load game sound effect! SDL_mixer Error: %s \n", Mix_GetError());
  }
  sounds->tagged_sound = Mix_LoadWAV("assets/sound_effects/hide_and_seek.wav");
  if(sounds->tagged_sound == NULL) {
    printf(" %s \n", "EERROR HERE");
    printf("Failed to tagged sound effect! SDL_mixer Error: %s \n", Mix_GetError());
  }
  return *sounds;
}

void free_sound(sound_effect_t sound_effect){
  Mix_FreeMusic(sound_effect.game_sound);
  Mix_FreeChunk(sound_effect.tagged_sound);
}

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
    seeker_vel = INITIAL_VELOCITY;
  
    scene_add_body(state->scene, seeker);
    body_set_velocity(seeker, seeker_vel);
    asset_t *new_asset_seeker = asset_make_image_with_body(SEEKER_PATH, seeker);
    list_add(state->body_assets, new_asset_seeker);
    state->last_seeker_time = 0;
    state->max_seekers += 1;

}

state_t *emscripten_init() {
    asset_cache_init();
    sdl_init(MIN, MAX);
    init_sound();
    state_t *state = malloc(sizeof(state_t));
    srand(time(NULL));
    state->scene = scene_init();
    state->max_seekers = 50;
    state->body_assets = list_init(state->max_seekers, (free_func_t)asset_destroy);
    add_new_seeker(state, false);
    state->sound_effect = load_game_sounds();
    Mix_PlayMusic(state->sound_effect.game_sound, -1);
    return state;
}


bool emscripten_main(state_t *state) {

    double dt = time_since_last_tick();
   
    state->last_seeker_time += dt;
    if(state->last_seeker_time >= NEW_SEEKERS_INTERVAL){
      add_new_seeker(state, true);
      Mix_PlayChannel(-1, state->sound_effect.tagged_sound, 0);
    }
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
  free_sound(state->sound_effect);
  asset_cache_destroy();
  free(state);
}