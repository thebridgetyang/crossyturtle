#include "forces.h"
#include "body.h"
#include "collision.h"
#include "scene.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

const double MIN_DISTANCE = 5;

// gravity and spring force
typedef struct gravity_spring_aux {
  double const_aux;
  body_t *body1_aux;
  body_t *body2_aux;
} gravity_spring_aux_t;

gravity_spring_aux_t *gravity_aux_init(double G, body_t *body1, body_t *body2) {
  gravity_spring_aux_t *gravity_aux = malloc(sizeof(gravity_spring_aux_t));
  assert(gravity_aux != NULL);
  gravity_aux->const_aux = G;
  gravity_aux->body1_aux = body1;
  gravity_aux->body2_aux = body2;
  return gravity_aux;
}

void gravity_forcer(gravity_spring_aux_t *gravity_aux) {
  vector_t body1_centroid = body_get_centroid(gravity_aux->body1_aux);
  vector_t body2_centroid = body_get_centroid(gravity_aux->body2_aux);
  double r = vec_distance(body1_centroid, body2_centroid);
  if (r < MIN_DISTANCE)
    return;
  double body1_mass = body_get_mass(gravity_aux->body1_aux);
  double body2_mass = body_get_mass(gravity_aux->body2_aux);
  double force =
      -(gravity_aux->const_aux * body1_mass * body2_mass) / pow(r, 2);
  vector_t unit12 =
      vec_multiply(1 / r, vec_subtract(body1_centroid, body2_centroid));
  vector_t force_vec = vec_multiply(force, unit12);

  body_add_force(gravity_aux->body1_aux, force_vec);
  body_add_force(gravity_aux->body2_aux, vec_negate(force_vec));
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1,
                              body_t *body2) {
  gravity_spring_aux_t *gravity_aux = gravity_aux_init(G, body1, body2);
  list_t *bodies = list_init(3, (free_func_t)NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, (force_creator_t)gravity_forcer,
                                 gravity_aux, bodies, (free_func_t)free);
}

gravity_spring_aux_t *spring_aux_init(double k, body_t *body1, body_t *body2) {
  gravity_spring_aux_t *spring_bodies = malloc(sizeof(gravity_spring_aux_t));
  assert(spring_bodies != NULL);
  spring_bodies->const_aux = k;
  spring_bodies->body1_aux = body1;
  spring_bodies->body2_aux = body2;
  return spring_bodies;
}

void spring_force_creator(gravity_spring_aux_t *bodies) {
  vector_t radius = vec_subtract(body_get_centroid(bodies->body1_aux),
                                 body_get_centroid(bodies->body2_aux));
  vector_t spring_force = vec_negate(vec_multiply(bodies->const_aux, radius));
  body_add_force(bodies->body1_aux, spring_force);
  body_add_force(bodies->body2_aux, spring_force);
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {
  gravity_spring_aux_t *spring_bodies = spring_aux_init(k, body1, body2);
  list_t *bodies = list_init(1, (free_func_t)NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, (force_creator_t)spring_force_creator,
                                 spring_bodies, bodies, (free_func_t)free);
}

// drag force
typedef struct drag_aux {
  double gamma_aux;
  body_t *body_aux;
} drag_aux_t;

drag_aux_t *drag_aux_init(double gamma, body_t *body) {
  drag_aux_t *drag_aux = malloc(sizeof(drag_aux_t));
  assert(drag_aux != NULL);
  drag_aux->gamma_aux = gamma;
  drag_aux->body_aux = body;
  return drag_aux;
}

void drag_forcer(drag_aux_t *drag_aux) {
  vector_t v = body_get_velocity(drag_aux->body_aux);
  vector_t force = vec_multiply(-1.0 * (drag_aux->gamma_aux), v);
  body_add_force((body_t *)drag_aux->body_aux, force);
}

void create_drag(scene_t *scene, double gamma, body_t *body) {
  drag_aux_t *drag_aux = drag_aux_init(gamma, body);
  list_t *bodies = list_init(2, (free_func_t)NULL);
  list_add(bodies, body);
  scene_add_bodies_force_creator(scene, (force_creator_t)drag_forcer, drag_aux,
                                 bodies, (free_func_t)free);
}

typedef struct collision_aux {
  collision_handler_t collision_handler;
  body_t *body1_aux;
  body_t *body2_aux;
  bool collided;
  void *aux;
  free_func_t freer;
} collision_aux_t;

collision_aux_t *collision_aux_init(collision_handler_t collision_handler,
                                    body_t *body1_aux, body_t *body2_aux,
                                    void *aux, free_func_t freer) {
  collision_aux_t *collision_aux = malloc(sizeof(collision_aux_t));
  collision_aux->collision_handler = collision_handler;
  collision_aux->body1_aux = body1_aux;
  collision_aux->body2_aux = body2_aux;
  collision_aux->collided = false;
  collision_aux->aux = aux;
  collision_aux->freer = freer;
  return collision_aux;
}

void collision_aux_freer(collision_aux_t *collision_aux) {
  if (collision_aux->freer != NULL)
    collision_aux->freer(collision_aux->aux);
  free(collision_aux);
}

void collision_forcer(collision_aux_t *collision_aux) {
  list_t *shape1 = body_get_shape(collision_aux->body1_aux);
  list_t *shape2 = body_get_shape(collision_aux->body2_aux);
  if (find_collision(shape1, shape2).collided) {
    if (collision_aux->collided)
      return;
    vector_t axis = vec_subtract(body_get_centroid(collision_aux->body1_aux),
                                 body_get_centroid(collision_aux->body2_aux));
    axis = vec_multiply(1.0 / vec_magnitude(axis), axis);
    collision_aux->collision_handler(collision_aux->body1_aux,
                                     collision_aux->body2_aux, axis,
                                     collision_aux->aux);
    collision_aux->collided = true;
  } else
    collision_aux->collided = false;

  list_free(shape1);
  list_free(shape2);
}

void destructive_collision(body_t *body1, body_t *body2, vector_t axis,
                           void *aux) {
  body_remove(body1);
  body_remove(body2);
}

void elastic_collision(body_t *body1, body_t *body2, vector_t axis,
                       void *elasticity) {
  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);
  collision_info_t collision_info = find_collision(shape1, shape2);
  double impulse;
  double C_R = *(double *)elasticity;
  double m_a = body_get_mass(body1);
  double m_b = body_get_mass(body2);
  double u_a = vec_dot(collision_info.axis, body_get_velocity(body1));
  double u_b = vec_dot(collision_info.axis, body_get_velocity(body2));

  if (m_a == INFINITY) {
    impulse = m_b * (1 + C_R) * (u_b - u_a);
    vector_t abs_impulse = vec_multiply(impulse, collision_info.axis);
    body_add_impulse(body2, vec_negate(abs_impulse));
  } else if (m_b == INFINITY) {
    impulse = m_a * (1 + C_R) * (u_b - u_a);
    vector_t abs_impulse = vec_multiply(impulse, collision_info.axis);
    body_add_impulse(body1, abs_impulse);
  } else {
    impulse = (m_a * m_b) / (m_a + m_b) * (1 + C_R) * (u_b - u_a);
    vector_t abs_impulse = vec_multiply(impulse, collision_info.axis);
    body_add_impulse(body1, abs_impulse);
    body_add_impulse(body2, vec_negate(abs_impulse));
  }
  list_free(shape1);
  list_free(shape2);
}

void create_collision(scene_t *scene, body_t *body1, body_t *body2,
                      collision_handler_t handler, void *aux,
                      free_func_t freer) {
  collision_aux_t *collision_aux =
      collision_aux_init(handler, body1, body2, aux, freer);
  list_t *bodies = list_init(2, (free_func_t)NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, (force_creator_t)collision_forcer,
                                 collision_aux, bodies,
                                 (free_func_t)collision_aux_freer);
}

void create_destructive_collision(scene_t *scene, body_t *body1,
                                  body_t *body2) {
  create_collision(scene, body1, body2, destructive_collision, NULL, NULL);
}

void create_physics_collision(scene_t *scene, double elasticity, body_t *body1,
                              body_t *body2) {
  double *elasticity_pointer = malloc(sizeof(double));
  *elasticity_pointer = elasticity;
  create_collision(scene, body1, body2, elastic_collision, elasticity_pointer,
                   free);
}
