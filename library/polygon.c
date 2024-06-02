#include "polygon.h"
#include "color.h"
#include "list.h"
#include <math.h>
#include <stdlib.h>

const vector_t GRAVITY = {.x = 0, .y = -9.8};
typedef struct polygon
{
  list_t *points;
  vector_t v;
  double v_rot;
  rgb_color_t *color;
  vector_t centroid;
} polygon_t;

polygon_t *polygon_init(list_t *points, vector_t initial_velocity,
                        double rotation_speed, double red, double green,
                        double blue)
{
  polygon_t *polygon = malloc(sizeof(polygon_t));

  polygon->points = points;
  polygon->v = initial_velocity;
  polygon->v_rot = rotation_speed;
  polygon->color = color_init(red, green, blue);
  polygon->centroid = polygon_centroid(polygon);

  return polygon;
}

list_t *polygon_get_points(polygon_t *polygon) { return polygon->points; }

void polygon_move(polygon_t *polygon, double time_elapsed)
{
  vector_t translate = vec_multiply(time_elapsed, polygon->v);
  double rot_angle = polygon->v_rot * time_elapsed;

  list_t *points = polygon->points;
  for (size_t i = 0; i < list_size(points); i++)
  {
    vector_t *vertex = list_get(points, i);
    *vertex = vec_add(*vertex, translate);
    double x = vertex->x;
    double y = vertex->y;
    vertex->x = x * cos(rot_angle) - y * sin(rot_angle);
    vertex->y = x * sin(rot_angle) + y * cos(rot_angle);
  }
  vector_t gravity = GRAVITY;
  polygon->v = vec_add(polygon->v, vec_multiply(time_elapsed, gravity));
}

void polygon_set_velocity(polygon_t *polygon, vector_t vel)
{
  polygon->v.x = vel.x;
  polygon->v.y = vel.y;
}

void polygon_free(polygon_t *polygon)
{
  color_free(polygon->color);
  list_free(polygon->points);
  free(polygon);
}

double polygon_get_velocity_x(polygon_t *polygon) { return polygon->v.x; }

double polygon_get_velocity_y(polygon_t *polygon) { return polygon->v.y; }

double polygon_area(polygon_t *polygon)
{
  double area = 0.0;
  size_t size = list_size(polygon->points);

  for (size_t i = 0; i < size; i++)
  {
    vector_t *v1 = list_get(polygon->points, i);
    vector_t *v2 = list_get(polygon->points, ((i + 1) % size));
    area += (v1->x * v2->y) - (v2->x * v1->y);
  }

  return (fabs(area) / 2.0);
}

vector_t polygon_centroid(polygon_t *polygon)
{
  double signed_area = 0.0;
  double c_x = 0.0;
  double c_y = 0.0;

  size_t size = list_size(polygon->points);

  for (size_t i = 0; i < size; i++)
  {
    vector_t *v1 = list_get(polygon->points, i);
    vector_t *v2 = list_get(polygon->points, ((i + 1) % size));

    double area = (v1->x * v2->y) - (v2->x * v1->y);
    signed_area += area;
    c_x += (v1->x + v2->x) * area;
    c_y += (v1->y + v2->y) * area;
  }
  signed_area *= 0.5;
  c_x /= (6.0 * signed_area);
  c_y /= (6.0 * signed_area);

  return (vector_t){.x = c_x, .y = c_y};
}

void polygon_translate(polygon_t *polygon, vector_t translation)
{
  size_t size = list_size(polygon->points);

  for (size_t i = 0; i < size; i++)
  {
    vector_t *v = list_get(polygon->points, i);
    v->x += translation.x;
    v->y += translation.y;
  }
}

void polygon_rotate(polygon_t *polygon, double angle, vector_t point)
{
  size_t size = list_size(polygon->points);

  for (size_t i = 0; i < size; i++)
  {
    vector_t *v = list_get(polygon->points, i);
    double x = v->x - point.x;
    double y = v->y - point.y;
    v->x = point.x + (x * cos(angle) - y * sin(angle));
    v->y = point.y + (x * sin(angle) + y * cos(angle));
  }
}

rgb_color_t *polygon_get_color(polygon_t *polygon) { return polygon->color; }

void polygon_set_color(polygon_t *polygon, rgb_color_t *color)
{
  polygon->color->b = color->b;
  polygon->color->g = color->g;
  polygon->color->r = color->r;
}

void polygon_set_center(polygon_t *polygon, vector_t centroid)
{
  polygon->centroid.x = centroid.x;
  polygon->centroid.y = centroid.y;
}

vector_t polygon_get_center(polygon_t *polygon)
{
  return polygon_centroid(polygon);
}

void polygon_set_rotation(polygon_t *polygon, double rot)
{
  polygon->v_rot = rot;
}

double polygon_get_rotation(polygon_t *polygon) { return polygon->v_rot; }
