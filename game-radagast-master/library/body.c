#include "body.h"
#include "color.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "vector.h"
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct body {
  list_t *shape;
  list_t *forces;
  vector_t centroid;
  void *info;
  vector_t velocity;
  vector_t delta_velocity;
  vector_t acceleration;
  double ang_vel;
  double angle;
  double mass;
  rgb_color_t color;
  SDL_Surface *image;
  double width;
  double height;
  bool remove_status;
  free_func_t info_freer;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  body_t *body = malloc(1 * sizeof(body_t));
  body->shape = shape;
  body->forces = list_init(1, free);
  body->centroid = polygon_centroid(shape);
  body->info = NULL;
  body->velocity = VEC_ZERO;
  body->delta_velocity = VEC_ZERO;
  body->acceleration = VEC_ZERO;
  body->ang_vel = 0;
  body->angle = 0;
  body->mass = mass;
  body->color = color;
  body->image = NULL;
  body->width = 0;
  body->height = 0;
  body->remove_status = false;
  body->info_freer = NULL;
  return body;
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            SDL_Surface *image, double height, double width,
                            void *info, free_func_t freer) {
  body_t *body = malloc(1 * sizeof(body_t));
  body->shape = shape;
  body->forces = list_init(1, free);
  body->centroid = polygon_centroid(shape);
  body->info = info;
  body->velocity = VEC_ZERO;
  body->delta_velocity = VEC_ZERO;
  body->acceleration = VEC_ZERO;
  body->ang_vel = 0;
  body->angle = 0;
  body->mass = mass;
  body->color = color;
  body->image = image;
  body->width = width;
  body->height = height;
  body->remove_status = false;
  body->info_freer = freer;
  return body;
}

void body_free(body_t *body) {
  list_free(body->shape);
  list_free(body->forces);
  if (body->info_freer != NULL)
    body->info_freer(body->info);
  SDL_FreeSurface(body->image);
  free(body);
}

list_t *body_get_shape_dev(body_t *body) { return body->shape; }

list_t *body_get_shape(body_t *body) {
  list_t *shape = list_init(list_size(body->shape), free);
  for (size_t i = 0; i < list_size(body->shape); i++) {
    vector_t *element = list_get(body->shape, i);
    vector_t *new_element = malloc(sizeof(vector_t));
    new_element->x = element->x;
    new_element->y = element->y;
    list_add(shape, new_element);
  }
  return shape;
}

vector_t body_get_centroid(body_t *body) { return body->centroid; }

vector_t body_get_velocity(body_t *body) { return body->velocity; }

double body_get_mass(body_t *body) { return body->mass; }

rgb_color_t body_get_color(body_t *body) { return body->color; }

void *body_get_info(body_t *body) { return body->info; }

SDL_Surface *body_get_image(body_t *body) { return body->image; }

double body_get_width(body_t *body) { return body->width; }

double body_get_height(body_t *body) { return body->height; }

double body_get_angle(body_t *body) { return body->angle; }

void body_set_centroid(body_t *body, vector_t x) {
  vector_t relative = vec_subtract(x, body->centroid);
  body->centroid = x;
  polygon_translate(body->shape, relative);
}

void body_set_velocity(body_t *body, vector_t v) { body->velocity = v; }

void body_set_angular_velocity(body_t *body, double ang_vel) {
  body->ang_vel = ang_vel;
}

void body_set_acceleration(body_t *body, vector_t a) { body->acceleration = a; }

void body_set_image(body_t *body, SDL_Surface *img) { body->image = img; }

void body_set_info(body_t *body, void *inf) { body->info = inf; }

void body_set_rotation(body_t *body, double angle) {
  double angle_change = angle - body->angle;
  polygon_rotate(body->shape, angle_change, body->centroid);
  body->angle = angle;
}

void body_add_force(body_t *body, vector_t force) {
  vector_t new_acc = {.x = force.x / body->mass, .y = force.y / body->mass};
  body->acceleration = vec_add(body->acceleration, new_acc);
}

void body_add_impulse(body_t *body, vector_t impulse) {
  body->delta_velocity =
      vec_multiply(1 / body->mass, vec_add(body->delta_velocity, impulse));
}

void body_tick(body_t *body, double dt) {
  vector_t new_vel = {.x = body->velocity.x + body->delta_velocity.x +
                           body->acceleration.x * dt,
                      .y = body->velocity.y + body->delta_velocity.y +
                           body->acceleration.y * dt};
  vector_t avg_vel = {.x = (body->velocity.x + new_vel.x) / 2,
                      .y = (body->velocity.y + new_vel.y) / 2};
  body->delta_velocity = (vector_t){.x = 0, .y = 0};
  body->acceleration = (vector_t){.x = 0, .y = 0};
  vector_t displacement_vec = {.x = avg_vel.x * dt, .y = avg_vel.y * dt};
  body_set_velocity(body, new_vel);
  body_set_rotation(body, body->angle + (body->ang_vel * dt));
  body_set_centroid(body, vec_add(body->centroid, displacement_vec));
}

void body_remove(body_t *body) { body->remove_status = true; }

bool body_is_removed(body_t *body) { return body->remove_status; }
