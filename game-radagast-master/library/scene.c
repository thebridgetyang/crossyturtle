#include "scene.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

const size_t INIT_BODY_COUNT = 10;
const size_t INIT_FORCE_COUNT = 10;

typedef struct scene {
  list_t *bodies;
  list_t *forces;
  list_t *auxes;
  list_t *forced_bodies;
} scene_t;

scene_t *scene_init(void) {
  scene_t *new_scene = malloc(1 * sizeof(scene_t));
  assert(new_scene != NULL);
  new_scene->bodies = list_init(INIT_BODY_COUNT, (free_func_t)body_free);
  new_scene->forces = list_init(INIT_FORCE_COUNT, (free_func_t)NULL);
  new_scene->auxes = list_init(INIT_FORCE_COUNT, (free_func_t)list_free);
  new_scene->forced_bodies =
      list_init(INIT_FORCE_COUNT, (free_func_t)list_free);
  return new_scene;
}

void scene_free(scene_t *scene) {
  list_free(scene->bodies);
  list_free(scene->forces);
  list_free(scene->auxes);
  list_free(scene->forced_bodies);
  free(scene);
}

size_t scene_bodies(scene_t *scene) { return list_size(scene->bodies); }

body_t *scene_get_body(scene_t *scene, size_t index) {
  return (body_t *)list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
  list_add(scene->bodies, body);
}

body_t *scene_set_body(scene_t *scene, body_t *body, size_t index) {
  return (body_t *)list_set(scene->bodies, body, index);
}

void scene_remove_body(scene_t *scene, size_t index) {
  body_remove(scene_get_body(scene, index));
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer) {
  list_add(scene->forces, forcer);
  list_t *aux_with_free = list_init(1, freer);
  list_add(aux_with_free, aux);
  list_add(scene->auxes, aux_with_free);
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies,
                                    free_func_t freer) {
  scene_add_force_creator(scene, forcer, aux, freer);
  list_add(scene->forced_bodies, bodies);
}

void scene_tick(scene_t *scene, double dt) {
  // apply forces
  for (size_t i = 0; i < list_size(scene->forces); i++) {
    ((force_creator_t)list_get(scene->forces, i))(
        list_get(list_get(scene->auxes, i), 0));
  }

  // free removed bodies
  list_t *body_removed = list_init(INIT_BODY_COUNT, NULL);
  list_t *force_removed = list_init(INIT_FORCE_COUNT, NULL);
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    if (body_is_removed(scene_get_body(scene, i))) {
      list_add(body_removed, (void *)i);
    }
  }
  for (size_t i = 0; i < list_size(scene->forced_bodies); i++) {
    for (size_t j = 0; j < list_size(list_get(scene->forced_bodies, i)); j++) {
      if (list_size(force_removed) != 0 &&
          (size_t)list_get(force_removed, list_size(force_removed) - 1) == i)
        break;
      for (size_t k = 0; k < list_size(body_removed); k++) {
        if (list_get(list_get(scene->forced_bodies, i), j) ==
            scene_get_body(scene, (size_t)list_get(body_removed, k))) {
          list_add(force_removed, (void *)i);
          break;
        }
      }
    }
  }
  for (size_t i = 0; i < list_size(body_removed); i++) {
    body_free(
        list_remove(scene->bodies, (size_t)list_get(body_removed, i) - i));
  }
  for (size_t i = 0; i < list_size(force_removed); i++) {
    list_remove(scene->forces, (size_t)list_get(force_removed, i) - i);
    list_free(
        list_remove(scene->auxes, (size_t)list_get(force_removed, i) - i));
    list_free(list_remove(scene->forced_bodies,
                          (size_t)list_get(force_removed, i) - i));
  }

  list_free(body_removed);
  list_free(force_removed);

  // body tick
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_tick(list_get(scene->bodies, i), dt);
  }
}