
#ifndef __SEEKER_H__
#define __SEEKER_H__

#include "stdbool.h"
#include "body.h"

typedef struct state state_t;

body_t *make_seeker(double width, double height, vector_t center);

void wrap_seeker_scene(body_t *seeker);

/*
* Add a new seeker either at the start of the game or
* after 30 seconds into the game
*/
void add_new_seeker(state_t *state, bool is_new);

#endif // #ifndef __SEEKER_H__