
#ifndef __SEEKER_H__
#define __SEEKER_H__

#include "stdbool.h"
#include "body.h"
#include "sound_effect.h"

typedef struct seeker seeker_t;

seeker_t *seeker_init();

body_t *make_seeker(double width, double height, vector_t center);

void wrap_seeker_scene(body_t *seeker);

/*
* Add a new seeker either at the start of the game or
* after 30 seconds into the game
*/
void add_new_seeker(seeker_t *seeker, bool is_new);

void introduce_seeker(seeker_t *seeker, double dt, sound_effect_t *sound_effect);

void free_seeker(seeker_t *seeker);

#endif // #ifndef __SEEKER_H__