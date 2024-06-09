#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "sound_effect.h"

const char *TAGGED_SOUND_PATH = "assets/sound_effects/tagged.wav";
const char *GAME_SOUND_EFFECT = "assets/sound_effects/hide-and-seek.wav";

struct sound_effect {
  Mix_Music *game_sound;
  Mix_Chunk *tagged_sound;
};

void init_sound() {
  if(SDL_Init(SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initiliaze! SDL_mixer Error: %s \n", Mix_GetError());
    exit(1);
  }
  if(Mix_OpenAudio( 48000, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
    printf("SDL_mixer could not initiliaze! SDL_mixer Error: %s \n", Mix_GetError());
    exit(1);
  }
}

sound_effect_t *sound_effect_init() {
  sound_effect_t *sounds = malloc(sizeof(sound_effect_t));
  sounds->game_sound = Mix_LoadMUS(TAGGED_SOUND_PATH);
  if(sounds->game_sound == NULL) {
    printf("Failed to load game sound effect! SDL_mixer Error: %s \n", Mix_GetError());
  }
  sounds->tagged_sound = Mix_LoadWAV(GAME_SOUND_EFFECT);
  if(sounds->tagged_sound == NULL) {
    printf(" %s \n", "EERROR HERE");
    printf("Failed to tagged sound effect! SDL_mixer Error: %s \n", Mix_GetError());
  }
  return sounds;
}

void sound_free(sound_effect_t *sound_effect){
  Mix_FreeMusic(sound_effect->game_sound);
  Mix_FreeChunk(sound_effect->tagged_sound);
  free(sound_effect);
}

void game_sound(sound_effect_t *sound_effect){
     Mix_PlayMusic(sound_effect->game_sound, -1);
}

void tagged_sound(sound_effect_t *sound_effect) {
    Mix_PlayChannel(-1, sound_effect->tagged_sound, 0);
}
