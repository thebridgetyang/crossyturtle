#include "vector.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const vector_t VEC_ZERO = {0, 0};

typedef struct vector_t {
  double x;
  double y;
} vector_t1;

vector_t vec_add(vector_t v1, vector_t v2) {
  vector_t v;
  v.x = v1.x + v2.x;
  v.y = v1.y + v2.y;
  return v;
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
  vector_t v;
  v.x = v1.x - v2.x;
  v.y = v1.y - v2.y;
  return v;
}

vector_t vec_negate(vector_t v) { return vec_multiply(-1, v); }

vector_t vec_multiply(double scalar, vector_t v) {
  v.x *= scalar;
  v.y *= scalar;
  return v;
}

double vec_dot(vector_t v1, vector_t v2) { return v1.x * v2.x + v1.y * v2.y; }

double vec_cross(vector_t v1, vector_t v2) { return v1.x * v2.y - v2.x * v1.y; }

double vec_magnitude(vector_t vec) {
  return pow(pow(vec.x, 2) + pow(vec.y, 2), 0.5);
}

double vec_distance(vector_t v1, vector_t v2) {
  return vec_magnitude(vec_subtract(v1, v2));
}

vector_t vec_rotate(vector_t v, double angle) {
  vector_t temp;
  temp.x = v.x * cos(angle) - v.y * sin(angle);
  v.y = v.x * sin(angle) + v.y * cos(angle);
  v.x = temp.x;
  return v;
}