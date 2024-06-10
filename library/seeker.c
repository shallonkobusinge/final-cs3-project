#include "stdbool.h"
#include "stdlib.h"
#include "seeker.h"
#include "scene.h"
#include "list.h"
#include "asset.h"
#include "sound_effect.h"
#include "landing_page.h"
#include "maze.h"
#include "forces.h"

const char *SEEKER_PATH = "assets/images/scenery/seeker_bg.png";
const char *BEAVER_PATH = "assets/images/scenery/beaver.png";

const vector_t MIN_WINDOW = {0, 0};
const vector_t MAX_WINDOW = {1000, 500};

extern const size_t GRID_WIDTH;
extern const size_t GRID_HEIGHT;
extern const size_t NUM_CELLS;
extern const size_t GRID_CELL_SIZE;
extern const size_t MAZE_WINDOW_WIDTH;
extern const size_t MAZE_WINDOW_HEIGHT;

// SEEKING CONSTANTS
const size_t S_NUM_POINTS = 20;
const double S_RADIUS = 0.1;
const size_t NEW_SEEKERS_INTERVAL = 60;

const rgb_color_t SEEKER_COLOR = (rgb_color_t){0.0, 0.0, 0.0};

typedef struct seeker
{
  double last_render;
} seeker_t;

typedef struct state
{
  scene_t *scene;
  size_t page;
  maze_state_t *maze_state;
  landing_page_state_t *landing_page_state;
  end_page_state_t *end_page_state;
  sound_effect_t *sound_effect;
  seeker_t *seeker;
  list_t *body_assets;
} state_t;

body_t *make_body(vector_t center, rgb_color_t color)
{
  double size = (double)GRID_CELL_SIZE;
  list_t *c = list_init(4, free);
  vector_t *v1 = malloc(sizeof(vector_t));
  *v1 = (vector_t){-size / 2, -size / 2};
  list_add(c, v1);

  vector_t *v2 = malloc(sizeof(vector_t));
  *v2 = (vector_t){size / 2, -size / 2};
  list_add(c, v2);

  vector_t *v3 = malloc(sizeof(vector_t));
  *v3 = (vector_t){size / 2, size / 2};
  list_add(c, v3);

  vector_t *v4 = malloc(sizeof(vector_t));
  *v4 = (vector_t){-size / 2, size / 2};
  list_add(c, v4);
  body_t *body = body_init(c, 1, color);
  body_set_centroid(body, center);
  return body;
}

void add_to_scene(state_t *state, vector_t center, rgb_color_t color, const char *path)
{
  body_t *body = make_body(center, color);
  scene_add_body(state->scene, body);
  asset_t *asset_body = asset_make_image_with_body(path, body);
  list_add(state->body_assets, asset_body);
}

void move_body(body_t *body, vector_t vec)
{
  body_set_centroid(body, vec_add(body_get_centroid(body), vec));
}

/**
 * Display the time elapsed in a human-readable format.
 *
 * This function takes the remaining time in seconds and calculates the
 * time elapsed from a predefined total game time. It then formats and
 * prints the remaining time in minutes and seconds if at least one minute
 * has passed, or just seconds if less than a minute has passed.
 *
 * @param remaining_seconds The remaining time in seconds.
 */
static void display_time_elapsed(int32_t remaining_seconds)
{
  int32_t time_elapsed = NEW_SEEKERS_INTERVAL - remaining_seconds;

  printf("TIME UNTIL NEXT SEEKER: %d sec\n", time_elapsed);
}

/**
 * Adding a seeker body to the scene.
 * Add seeker's bodies asset to the state->list_body_asset
 * Generate a random seeker position if it's a new seeker to be added after the 30 seconds.
 * If it's the initial seeker use a defined position.
 * @param state struct state of the game.
 * @param is_new determine if is it's the after 30 sec or initial seeker to be added.
 */
static void add_new_seeker(state_t *state, bool is_new)
{

  vector_t seeker_pos = VEC_ZERO;
  if (is_new)
  {
    seeker_pos = (vector_t){
        .x = (rand() % (GRID_WIDTH)*GRID_CELL_SIZE) + (GRID_CELL_SIZE) / 2,
        .y = (rand() % (GRID_HEIGHT - 4) * GRID_CELL_SIZE) - (GRID_CELL_SIZE / 3),
    };
    state->seeker->last_render = 0;
  }
  else
  {
    seeker_pos = (vector_t){.x = (((GRID_WIDTH - 2) * GRID_CELL_SIZE) + (GRID_CELL_SIZE) / 2),
                            .y = (((GRID_HEIGHT - 6) * GRID_CELL_SIZE) - (GRID_CELL_SIZE / 3))};
  }
  add_to_scene(state, seeker_pos, SEEKER_COLOR, SEEKER_PATH);
}

void render_seeker(state_t *state, double dt)
{

  state->seeker->last_render += dt;
  display_time_elapsed(state->seeker->last_render);
  if (state->seeker->last_render >= NEW_SEEKERS_INTERVAL)
  {
    add_new_seeker(state, true);
  }
  for (size_t i = 1; i < list_size(state->body_assets); i++)
  {

    rgb_color_t *color = body_get_color(scene_get_body(state->scene, i));
    if (color->r == 0.0 && color->g == 0.0 && color->b == 0.0)
    {
      asset_render(list_get(state->body_assets, i));
    }
  }
}

/**
 * Adds a hider body returned by make_body() to the scene.
 * Creates and adds a body asset of the hider to the list of body_assets in the state.
 * @param state state struct of the game.
 */
static void hider_init(state_t *state)
{
  vector_t center = (vector_t){.y = (((GRID_HEIGHT - 1) * GRID_CELL_SIZE) - (GRID_CELL_SIZE / 3)),
                               .x = (((GRID_WIDTH - 22) * GRID_CELL_SIZE) + (GRID_CELL_SIZE) / 2)};
  add_to_scene(state, center, (rgb_color_t){50, 129, 110}, BEAVER_PATH);
}

seeker_t *seeker_init(state_t *state)
{
  seeker_t *seeker = malloc(sizeof(seeker_t));
  seeker->last_render = 0;
  hider_init(state);
  add_new_seeker(state, false);
  return seeker;
}

void render_bodies(list_t *bodies)
{
  for (size_t i = 0; i < list_size(bodies); i++)
  {
    asset_render(list_get(bodies, i));
  }
}

void seekers_random_movement(state_t *state)
{
  for (size_t i = 1; i < scene_bodies(state->scene); i++)
  {
    body_t *seeker = scene_get_body(state->scene, i);
    rgb_color_t *color = body_get_color(seeker);
    if (color->r == 0.0 && color->g == 0.0 && color->b == 0.0)
    {
      vector_t body_centroid = body_get_centroid(seeker);
      vector_t new_centroid = traverse_maze(state, body_centroid, -1);
      SDL_Delay(96);
      move_body(seeker, new_centroid);
    }
  }
}

/**
 * Function handler whenever the bodies collide.
 * @param body1 the first body.
 * @param body2 the second body.
 * @param axis unit vector pointing from body1 toward body2 that
 *    that defines the direction the two bodies are colliding in
 * @param aux the auxiliary value passed to create_collision.
 * @param force_const the force constant passed to create_collision()
 */
static void end_game(body_t *body1, body_t *body2, vector_t axis, void *aux,
                     double force_const)
{
  size_t *page_ptr = (size_t *)aux;
  *page_ptr = 4;
}

void hider_seeker_collision(state_t *state)
{
  for (size_t i = 1; i < scene_bodies(state->scene); i++)
  {

    body_t *seeker = scene_get_body(state->scene, i);
    rgb_color_t *color = body_get_color(seeker);
    if (color->r == 0.0 && color->g == 0.0 && color->b == 0.0)
    {
      create_collision(state->scene, scene_get_body(state->scene, 0), seeker, end_game, &state->page, 0.0);
    }
  }
}

void seeker_free(seeker_t *seeker)
{
  free(seeker);
}