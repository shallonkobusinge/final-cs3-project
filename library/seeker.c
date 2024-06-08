#include "stdbool.h"
#include "stdlib.h"
#include "seeker.h"
#include "scene.h"
#include "list.h"
#include "asset.h"
#include "sound_effect.h"
#include "landing_page.h"
#include "maze.h"

const char *SEEKER_PATH = "assets/images/seeking/seeker_bg.png";


const char *BEAVER_PATH = "assets/images/seeking/beaver.png";

const vector_t MIN_WINDOW = {0, 0};
const vector_t MAX_WINDOW = {1000, 500};
const vector_t INITIAL_VELOCITY = {12, 12};

const size_t GRID_WIDTH_S = 25;
const size_t GRID_HEIGHT_S = 12;
const size_t NUM_CELLS_S = GRID_WIDTH_S * GRID_HEIGHT_S;

const int GRID_CELL_SIZE_S = 40;
const int window_width_s = (GRID_WIDTH_S * GRID_CELL_SIZE_S) + 1;
const int window_height_s = (GRID_HEIGHT_S * GRID_CELL_SIZE_S) + 1;
const vector_t START_POS = (vector_t){.x = (((GRID_WIDTH_S - 2) * GRID_CELL_SIZE_S) + GRID_CELL_SIZE_S / 4), .y = (((GRID_HEIGHT_S - 6) * GRID_CELL_SIZE_S) + GRID_CELL_SIZE_S / 4)};
const double OUTER_RADIUS =  GRID_CELL_SIZE_S;
const double INNER_RADIUS = GRID_CELL_SIZE_S;

// SEEKING CONSTANTS
#define S_NUM_POINTS 20
#define S_RADIUS 0.1
#define NEW_SEEKERS_INTERVAL 3

const rgb_color_t seeker_color = (rgb_color_t){0.1, 0.9, 0.2};

typedef struct seeker {
    double last_seeker_time;
}seeker_t;

typedef struct state {
    scene_t *scene;
    size_t page;
    maze_state_t *maze_state;
    landing_page_state_t *landing_page_state;
    sound_effect_t *sound_effect;
    seeker_t *seeker;
    list_t *body_assets;
}state_t;

body_t *make_seeker(double w, double h, vector_t center) {

  list_t *c = list_init(4, free);
   vector_t *v1 = malloc(sizeof(vector_t));
  *v1 = (vector_t){0 , 0};
  list_add(c, v1);

  vector_t *v2 = malloc(sizeof(vector_t));
  *v2 = (vector_t){w, 0};
  list_add(c, v2);

  vector_t *v3 = malloc(sizeof(vector_t));
  *v3 = (vector_t){w, h};
  list_add(c, v3);

  vector_t *v4 = malloc(sizeof(vector_t));
  *v4 = (vector_t){0, h};
  list_add(c, v4);
  body_t *seeker = body_init(c, 6, seeker_color);
  body_set_centroid(seeker, center);
  return seeker;
}

void move_body(body_t *body, vector_t vec) {
  body_set_centroid(body, vec_add(body_get_centroid(body), vec));
}

void add_new_seeker(state_t *state, bool is_new){
   
   body_t *seeker;
    if(is_new){
      vector_t seeker_pos = (vector_t){
        .x = (rand() % (GRID_WIDTH_S ) * GRID_CELL_SIZE_S) + GRID_CELL_SIZE_S / 4,
        .y = (rand() % (GRID_HEIGHT_S - 4) * GRID_CELL_SIZE_S) + GRID_CELL_SIZE_S / 4,
    };
    
    seeker = make_seeker(OUTER_RADIUS, INNER_RADIUS, seeker_pos);
    state->seeker->last_seeker_time = 0;
    }else{
       seeker = make_seeker(OUTER_RADIUS, INNER_RADIUS, START_POS);
    }
   
    scene_add_body(state->scene, seeker);
    asset_t *new_asset_seeker = asset_make_image_with_body(SEEKER_PATH, seeker);
    list_add(state->body_assets, new_asset_seeker);
}

void render_seeker(state_t *state, double dt){
    state->seeker->last_seeker_time += dt;
    if(state->seeker->last_seeker_time >= NEW_SEEKERS_INTERVAL){
      add_new_seeker(state, true);
      //  tagged_sound(state->sound_effect);
    }
    for (size_t i = 1; i < list_size(state->body_assets); i++) {

        rgb_color_t *color = body_get_color(scene_get_body(state->scene, i));
        if(color->r == 0.1 && color->g == 0.9 && color->b == 0.2) {
              asset_render(list_get(state->body_assets, i));
            }
 
    }
}
void hider_init(state_t *state){
    vector_t center = (vector_t){.x = (((GRID_WIDTH_S - 24) * GRID_CELL_SIZE_S) + GRID_CELL_SIZE_S / 4), .y = (((GRID_HEIGHT_S - 11) * GRID_CELL_SIZE_S) + GRID_CELL_SIZE_S / 4)};

    body_t *beaver = make_seeker(35, 35, center);
    scene_add_body(state->scene, beaver);

    asset_t *asset_beaver = asset_make_image_with_body(BEAVER_PATH, beaver);
    list_add(state->body_assets, asset_beaver);
}

seeker_t *seeker_init(state_t *state){
  seeker_t *seeker = malloc(sizeof(seeker_t));
  seeker->last_seeker_time = 10;
  return seeker;
}

void render_seeker_bodies(state_t *state) {
   for (size_t i = 0; i < list_size(state->body_assets); i++) {
          asset_render(list_get(state->body_assets, i));
        }
}


void random_move_seeker (body_t *seeker) {
    SDL_Delay(650);
    int direction = rand() % 4;
    
    vector_t centroid = VEC_ZERO;

    switch (direction) {
    case 0: { // move left
            centroid.x -= GRID_CELL_SIZE_S;
        break;
    }
    case 1: { // move right
            centroid.x += GRID_CELL_SIZE_S;
        break;
    }
    case 2: { // move up
            centroid.y -= GRID_CELL_SIZE_S;
        break;
    }
    case 3: { // move down
            centroid.y += GRID_CELL_SIZE_S;
        break;
    }
    default:
        break;
    }
    list_t *shape = body_get_shape(seeker);
    bool move_valid = true;
    for(size_t i = 0; i < list_size(shape); i++) {
      vector_t vertex = *(vector_t *)list_get(shape, i);
      vector_t new_vertex = vec_add(vertex, centroid);
      if(new_vertex.x < 0 || new_vertex.y < 0 || new_vertex.x >= window_width_s || new_vertex.y >= window_height_s){
        move_valid = false;
        break;
      }
    }
    list_free(shape);
    if(move_valid){
      move_body(seeker, centroid);
    }

}

void seeker_free(seeker_t *seeker) {
  free(seeker);
}
