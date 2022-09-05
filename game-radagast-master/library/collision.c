#include "collision.h"
#include "polygon.h"
#include "vector.h"
#include <math.h>

const size_t NUM_AXIS = 2048;

typedef vector_t range_t;

// Assumes axis is a normal vector
range_t get_projection(list_t *shape, vector_t axis) {
  range_t proj_range = {INFINITY, -INFINITY};
  for (size_t i = 0; i < list_size(shape); i++) {
    double proj = vec_dot(*(vector_t *)list_get(shape, i), axis);
    if (proj < proj_range.x)
      proj_range.x = proj;
    if (proj > proj_range.y)
      proj_range.y = proj;
  }
  return proj_range;
}

collision_info_t find_collision(list_t *shape1, list_t *shape2) {
  collision_info_t info;
  double min_overlap = INFINITY;
  for (size_t i = 0; i < NUM_AXIS; i++) {
    double angle = ((double)i) * M_PI / ((double)NUM_AXIS);
    vector_t axis = (vector_t){cos(angle), sin(angle)};
    range_t proj1 = get_projection(shape1, axis);
    range_t proj2 = get_projection(shape2, axis);
    if (proj1.x > proj2.y || proj2.x > proj1.y) {
      info.collided = false;
      info.axis = VEC_ZERO;
      return info;
    } else {
      if (proj1.y >= proj2.x && proj1.y - proj2.x < min_overlap) {
        min_overlap = proj1.y - proj2.x;
        info.axis = axis;
      }
      if (proj2.y >= proj1.x && proj2.y - proj1.x < min_overlap) {
        min_overlap = proj2.y - proj1.x;
        info.axis = vec_negate(axis);
      }
    }
  }
  info.collided = true;
  return info;
}