
#ifndef __SEEKER_H__
#define __SEEKER_H__

#include "stdbool.h"
#include "body.h"
#include "sound_effect.h"
#include "scene.h"
#include "state.h"

typedef struct seeker seeker_t;

/**
 * Initilize a seeker
 * @param state state struct of the seeker.
*/
seeker_t *seeker_init(state_t *state);

body_t *make_body(double width, double height, vector_t center, rgb_color_t color);

void wrap_seeker_scene(body_t *seeker);

/*
* Add a new seeker either at the start of the game or
* after 30 seconds into the game
*/
void add_new_seeker(state_t *state, bool is_new);

void render_seeker(state_t *state, double dt);

void seeker_free(seeker_t *seeker);

void render_seeker_bodies(state_t *state);

void move_body(body_t *body, vector_t vec);

void hider_init(state_t *state);

void seeker_collision(state_t *state);

void seekers_random_movement(state_t *state);

#endif // #ifndef __SEEKER_H__