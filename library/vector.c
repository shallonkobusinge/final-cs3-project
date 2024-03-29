#include "vector.h"
#include <math.h>
#include <stdlib.h>

const vector_t VEC_ZERO = {.x = 0, .y = 0};

vector_t vec_add(vector_t v1, vector_t v2)
{
  return (vector_t){.x = (v1.x + v2.x), .y = (v1.y + v2.y)};
}

vector_t vec_subtract(vector_t v1, vector_t v2)
{
  return (vector_t){.x = (v1.x - v2.x), .y = (v1.y - v2.y)};
}

vector_t vec_negate(vector_t v)
{
  return (vector_t){.x = (v.x * -1), .y = (v.y * -1)};
}

vector_t vec_multiply(double scalar, vector_t v)
{
  return (vector_t){.x = (scalar * v.x), .y = (scalar * v.y)};
}

double vec_dot(vector_t v1, vector_t v2)
{
  return ((v1.x * v2.x) + (v1.y * v2.y));
}

double vec_cross(vector_t v1, vector_t v2)
{
  return ((v1.x * v2.y) - (v1.y * v2.x));
};

vector_t vec_rotate(vector_t v, double angle)
{
  return (vector_t){.x = (v.x * cos(angle) - v.y * sin(angle)),
                    .y = (v.x * sin(angle) + v.y * cos(angle))};
}
double vec_get_length(vector_t v) { return sqrt(pow(v.x, 2) + pow(v.y, 2)); }
