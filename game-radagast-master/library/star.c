#include "star.h"
#include "body.h"
#include "polygon.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

body_t *star_init(vector_t center, int num_vertices, double in_radius,
                  double out_radius, vector_t velocity, vector_t acceleration,
                  double angular_velocity, double density, rgb_color_t color) {
  list_t *star_shape = list_init(2 * num_vertices + 1, free);

  for (int i = 0; i < num_vertices; i++) {
    float angle = M_PI / (num_vertices);
    // Creating outer tip
    double outer_x = center.x + cos(2 * i * angle) * out_radius;
    double outer_y = center.y + sin(2 * i * angle) * out_radius;
    vector_t *outer_vertex = malloc(1 * sizeof(vector_t));
    outer_vertex->x = outer_x;
    outer_vertex->y = outer_y;
    list_add(star_shape, outer_vertex);

    // Creating inner tip
    double inner_x = center.x + cos((2 * i + 1) * angle) * in_radius;
    double inner_y = center.y + sin((2 * i + 1) * angle) * in_radius;
    vector_t *inner_vertex = malloc(sizeof(vector_t));
    inner_vertex->x = inner_x;
    inner_vertex->y = inner_y;
    list_add(star_shape, inner_vertex);
  }
  body_t *star =
      body_init(star_shape, polygon_area(star_shape) * density, color);
  body_set_velocity(star, velocity);
  body_set_rotation(star, angular_velocity);
  return star;
}

void star_bounce(body_t *star, vector_t window, double elasticity) {
  list_t *star_shape = body_get_shape(star);
  vector_t velocity = body_get_velocity(star);
  for (size_t i = 0; i < list_size(star_shape); i++) {
    vector_t *point = (vector_t *)list_get(star_shape, i);
    if (point->x < 0)
      velocity.x = fabs(velocity.x * elasticity);
    else if (point->x > window.x)
      velocity.x = -fabs(velocity.x * elasticity);
    if (point->y < 0)
      velocity.y = fabs(velocity.y * elasticity);
    else if (point->y > window.y)
      velocity.y = -fabs(velocity.y * elasticity);
  }
  body_set_velocity(star, velocity);
}

void side_top_bound(body_t *body, vector_t window, double elasticity) {
  list_t *body_shape = body_get_shape(body);
  vector_t velocity = body_get_velocity(body);
  for (size_t i = 0; i < list_size(body_shape); i++) {
    vector_t *point = (vector_t *)list_get(body_shape, i);
    if (point->x < 0)
      velocity.x = fabs(velocity.x * elasticity);
    else if (point->x > window.x)
      velocity.x = -fabs(velocity.x * elasticity);
    if (point->y > window.y)
      velocity.y = -fabs(velocity.y * elasticity);
  }
  body_set_velocity(body, velocity);
}

void gravity_bounce(body_t *star, vector_t window, double elasticity) {
  list_t *star_shape = body_get_shape(star);
  vector_t velocity = body_get_velocity(star);
  for (size_t i = 0; i < list_size(star_shape); i++) {
    vector_t *point = (vector_t *)list_get(star_shape, i);
    if (point->y < 0) {
      velocity.y = fabs(velocity.y * elasticity);
    }
    body_set_velocity(star, velocity);
  }
}