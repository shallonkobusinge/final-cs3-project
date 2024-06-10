
#ifndef __SEEKER_H__
#define __SEEKER_H__

#include "stdbool.h"
#include "body.h"
#include "sound_effect.h"
#include "scene.h"
#include "state.h"

typedef struct seeker seeker_t;

/**
 * Allocates memory for an empty seeker.
 * Adds one seeker and one hider to the scene.
 * @param state state struct of the game.
 * 
 * @return the new seeker.
*/
seeker_t *seeker_init(state_t *state);

/**
 * Create a four vector shaped body.
 * Translates the body to the new position.
 * @param position cell position to place the body in the maze.
 * @param color body color
 * 
 * @return the new body.
*/
body_t *make_body(vector_t position, rgb_color_t color);

/*
* Add a new seeker either at the start of the game or
* after 30 seconds into the game
*/

/**
 * Add a another seeker into the scene after every 30 seconds of the game.
 * @param state state struct of the game.
 * @param dt the number of seconds that have elapsed.
*/
void render_seeker(state_t *state, double dt);

/**
 * Release memory allocated for a given seeker.
 * @param seeker a pointer to a seeker returned from seeker_init();
*/
void seeker_free(seeker_t *seeker);

/**
 * Renders all bodies (seeker, buildings, hider) in the scene.
 * @param bodies list of all bodies in the scene.
*/
void render_bodies(list_t *bodies);

/**
 * Translates a body by a vector.
 * @param body body to be translated.
 * @param vec vector that translates a body.
*/
void move_body(body_t *body, vector_t vec);

/**
 * Creates collision between the seeker and hider in the state.
 * @param state state struct of the game.
*/
void hider_seeker_collision(state_t *state);

/**
 * Creates collision between the hider and a building.
 * @param state state struct of the game
*/
void hider_building_collision(state_t *state);

/**
 * Add a body to scene and add the body asset to list of body_assets in the state
 * @param state state struct of the game.
 * @param center vector of the center of the body.
 * @param color color of the body.
 * @param path file path of the image file.
*/
void add_to_scene(state_t *state, vector_t center, rgb_color_t color, const char *path);

#endif // #ifndef __SEEKER_H__