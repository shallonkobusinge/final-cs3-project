#ifndef __SOUND_EFFECT_H__
#define __SOUND_EFFECT_H__

typedef struct sound_effect sound_effect_t;

void init_sound();

sound_effect_t *load_game_sounds();

void free_sound(sound_effect_t *sound_effect);

void game_sound(sound_effect_t *sound_effect);

void tagged_sound(sound_effect_t *sound_effect);



#endif // #ifndef __SOUND_EFFECT_H__