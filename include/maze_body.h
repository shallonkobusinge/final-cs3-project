
#ifndef __MAZE_BODY_H__
#define __MAZE_BODY_H__

#include "stdbool.h"
#include "body.h"
#include "sound_effect.h"
#include "scene.h"
#include "state.h"

typedef struct maze_body
{
    vector_t position;
    rgb_color_t color;
    const char *img_path;
} maze_body_t;

typedef struct maze_bodies
{
    double last_render;
    size_t num_bodies;
    maze_body_t bodies[];
} maze_bodies_state_t;

void seekers_random_movement(state_t *state);

/**
 * Allocates memory for an empty seeker.
 * Adds one seeker and one hider to the scene.
 * @param state state struct of the game.
 *
 * @return the new seeker.
 */
// seeker_t *seeker_init(state_t *state);

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
void render_seeker(state_t *state, maze_bodies_state_t *bodies, double dt);

/**
 * Release memory allocated for a given seeker.
 * @param seeker a pointer to a seeker returned from seeker_init();
 */
// void seeker_free(seeker_t *seeker);

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
 * @param maze_body maze body to add to the scene.
 */
void add_to_scene(state_t *state, maze_body_t *maze_body);

maze_bodies_state_t *hider_seeker_init(state_t *state);

#endif // #ifndef __MAZE_BODY_H__