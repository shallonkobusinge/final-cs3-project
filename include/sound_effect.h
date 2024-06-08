#ifndef __SOUND_EFFECT_H__
#define __SOUND_EFFECT_H__

typedef struct sound_effect sound_effect_t;

/**
 * Initialize SDL and open audio device.
*/
void init_sound();

/**
 * Allocates memory for the an empty sound effect.
 * Loads .wav files into a music object and a chunk
 * 
 * @return the new sound effect.
*/
sound_effect_t *sound_effect_init();

/**
 * Realeases the allocated memory for a given sound effect.
 * Add the music object and chunk that it contains.
 * @param sound_effect a pointer to a sound_effect returned from sound_effect_init();
*/
void sound_free(sound_effect_t *sound_effect);

/**
 * Plays a music object. 
*/
void game_sound(sound_effect_t *sound_effect);

/**
 * Plays an audio chunk on the first free channel.
*/
void tagged_sound(sound_effect_t *sound_effect);



#endif // #ifndef __SOUND_EFFECT_H__