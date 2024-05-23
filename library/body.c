#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "body.h"

const double INITIAL_VELOCITY = 0.0;

struct body {
  polygon_t *poly;

  double mass;

  vector_t force;
  vector_t impulse;
  bool removed;
  vector_t centroid;
  void *info;
  free_func_t info_freer;
};

void body_reset(body_t *body) {
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer) {
  body_t *body = malloc(sizeof(body_t));
  assert(body != NULL);
  body->mass = mass;

  body->poly = polygon_init(shape, VEC_ZERO, INITIAL_VELOCITY, color.r, color.g,
                            color.b);

  body->centroid = polygon_centroid(body->poly);
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
  body->removed = false;
  body->info = info;
  body->info_freer = (free_func_t)info_freer;
  return body;
}

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  return body_init_with_info(shape, mass, color, NULL, NULL);
}

void body_free(body_t *body) {
  free_func_t info_free = (free_func_t)body->info_freer;
  if (info_free != NULL) {
    info_free(body->info);
  }
  polygon_free(body->poly);
  free(body);
}

list_t *body_get_shape(body_t *body) {
  list_t *shape = polygon_get_points(body->poly);
  return list_copy(shape);
}

vector_t body_get_centroid(body_t *body) { return body->centroid; }

vector_t body_get_velocity(body_t *body) {
  double x = polygon_get_velocity_x(body->poly);
  double y = polygon_get_velocity_y(body->poly);

  return (vector_t){x, y};
}

rgb_color_t *body_get_color(body_t *body) {
  return polygon_get_color(body->poly);
}

void body_set_color(body_t *body, rgb_color_t *col) {
  polygon_set_color(body->poly, col);
}

void body_set_centroid(body_t *body, vector_t x) {
  vector_t translation = vec_subtract(x, body->centroid);
  polygon_translate(body->poly, translation);
  body->centroid = x;
}

void body_set_velocity(body_t *body, vector_t v) {
  polygon_set_velocity(body->poly, v);
}

double body_get_rotation(body_t *body) {
  return polygon_get_rotation(body->poly);
}

void body_set_rotation(body_t *body, double angle) {
  polygon_rotate(body->poly, angle, body_get_centroid(body));
  polygon_set_rotation(body->poly, angle);
}

void body_tick(body_t *body, double dt) {
  vector_t force_by_impluse = vec_multiply(1 / dt, body->impulse);
  vector_t force_tot = vec_add(body->force, force_by_impluse);
  vector_t acceleration = vec_multiply(1 / body->mass, force_tot);
  vector_t velocity_init = body_get_velocity(body);
  vector_t new_velocity =
      vec_add(velocity_init, vec_multiply(dt, acceleration));

  vector_t average_velocity =
      vec_multiply(1.0 / 2.0, vec_add(velocity_init, new_velocity));

  vector_t displacement = vec_multiply(dt, average_velocity);
  polygon_translate(body->poly, displacement);

  body_set_velocity(body, new_velocity);
  body->centroid = vec_add(body->centroid, displacement);

  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
}

double body_get_mass(body_t *body) { return body->mass; }

void body_add_force(body_t *body, vector_t force) {
  body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
  body->impulse = vec_add(body->impulse, impulse);
}

void body_remove(body_t *body) { body->removed = true; }

bool body_is_removed(body_t *body) { return body->removed; }

polygon_t *body_get_polygon(body_t *body) { return body->poly; }

void *body_get_info(body_t *body) { return body->info; }
