
#ifndef __SEEKER_H__
#define __SEEKER_H__

#include "stdbool.h"
#include "body.h"
#include "sound_effect.h"
#include "scene.h"
#include "state.h"

typedef struct seeker seeker_t;

seeker_t *seeker_init(state_t *state);

body_t *make_seeker(double width, double height, vector_t center);

void wrap_seeker_scene(body_t *seeker);

/*
* Add a new seeker either at the start of the game or
* after 30 seconds into the game
*/
void add_new_seeker(state_t *state, bool is_new);

void render_seeker(state_t *state, double dt);

void seeker_free(seeker_t *seeker);

void render_seeker_bodies(seeker_t *seeker);

/*
 * Moves the seeker cell to a random
 * adjacent cell 
*/
void random_move_seeker (body_t *seeker);

#endif // #ifndef __SEEKER_H__