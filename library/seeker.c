#include "stdbool.h"
#include "stdlib.h"
#include "seeker.h"
#include "scene.h"
#include "list.h"
#include "asset.h"

// SEEKING CONSTANTS
#define STARTING_SEEKERS 1
#define S_NUM_POINTS 20
#define S_RADIUS 0.1

const vector_t MIN_WINDOW = {0, 0};
const vector_t MAX_WINDOW = {1000, 500};


const rgb_color_t seeker_color = (rgb_color_t){0.1, 0.9, 0.2};


body_t *make_seeker(double w, double h, vector_t center) {
    list_t *c = list_init(4, free);
  vector_t *v1 = malloc(sizeof(vector_t));
  *v1 = (vector_t){center.x, 0};
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
  body_t *obstacle = body_init(c, 1, seeker_color);
  body_set_centroid(obstacle, center);
  return obstacle;
}

void wrap_seeker_scene(body_t *seeker) {
  vector_t centroid = body_get_centroid(seeker);
  vector_t velocity = body_get_velocity(seeker);
  
  if (centroid.x >= MAX_WINDOW.x || centroid.x <= MIN_WINDOW.x) {
        velocity.x = -velocity.x;
  }
  if(centroid.y >= MAX_WINDOW.y || centroid.y <= MIN_WINDOW.y) {
    velocity.y = -velocity.y;
  }
  body_set_velocity(seeker, velocity);
}

