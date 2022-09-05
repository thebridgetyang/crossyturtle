#include "collision.h"
#include "forces.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

body_t *make_triangle_body() {
  list_t *shape = list_init(3, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){1, 0};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-0.5, +sqrt(3) / 2};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-0.5, -sqrt(3) / 2};
  list_add(shape, v);
  return body_init(shape, 1, (rgb_color_t){0, 0, 0});
}

void test_collisions() {
  const double DT = 0.1;
  const double V = 1.23;
  const double SEPARATION_AT_COLLISION = 1.5;
  const int TICKS_TO_COLLISION = 10;

  scene_t *scene = scene_init();
  body_t *body1 = make_triangle_body();
  vector_t initial_separation = {
      SEPARATION_AT_COLLISION + V * DT * (TICKS_TO_COLLISION - 0.5), 0};
  body_set_centroid(body1, vec_negate(initial_separation));
  body_set_velocity(body1, (vector_t){+V, 0});
  scene_add_body(scene, body1);

  body_t *body2 = make_triangle_body();
  scene_add_body(scene, body2);

  body_t *body3 = make_triangle_body();
  body_set_velocity(body3, (vector_t){-V, 0});
  body_set_centroid(body3, initial_separation);
  scene_add_body(scene, body3);

  create_destructive_collision(scene, body1, body2);
  create_destructive_collision(scene, body1, body3);
  create_destructive_collision(scene, body2, body3);
  for (int i = 0; i < TICKS_TO_COLLISION * 2; i++) {
    scene_tick(scene, DT);
    // Before collision, there should be 3 bodies; after, there should be 0
    if (i < TICKS_TO_COLLISION) {
      assert(scene_bodies(scene) == 3);
    } else {
      assert(scene_bodies(scene) == 0);
    }
  }
  scene_free(scene);
}

// Two shapes that have collided
void test_collision_1() {
  list_t *shape1 = list_init(3, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){1, 0};
  list_add(shape1, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-0.5, +sqrt(3) / 2};
  list_add(shape1, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-0.5, -sqrt(3) / 2};
  list_add(shape1, v);

  list_t *shape2 = list_init(3, free);
  vector_t *w = malloc(sizeof(*w));
  *w = (vector_t){0, 0};
  list_add(shape2, w);
  w = malloc(sizeof(*w));
  *w = (vector_t){0, 5};
  list_add(shape2, w);
  w = malloc(sizeof(*w));
  *w = (vector_t){5, 0};
  list_add(shape2, w);

  assert(find_collision(shape1, shape2).collided == true);

  list_free(shape1);
  list_free(shape2);
}

void test_collision_2() {
  list_t *shape3 = list_init(4, free);
  vector_t *a = malloc(sizeof(*a));
  *a = (vector_t){0, 0};
  list_add(shape3, a);
  a = malloc(sizeof(*a));
  *a = (vector_t){0, 1};
  list_add(shape3, a);
  a = malloc(sizeof(*a));
  *a = (vector_t){1, 1};
  list_add(shape3, a);
  a = malloc(sizeof(*a));
  *a = (vector_t){1, 0};
  list_add(shape3, a);

  list_t *shape4 = list_init(3, free);
  vector_t *b = malloc(sizeof(*b));
  *b = (vector_t){2, 0};
  list_add(shape4, b);
  b = malloc(sizeof(*b));
  *b = (vector_t){0, 10};
  list_add(shape4, b);
  b = malloc(sizeof(*b));
  *b = (vector_t){5, 0};
  list_add(shape4, b);

  assert(find_collision(shape3, shape4).collided == false);

  list_free(shape3);
  list_free(shape4);
}

int main(int argc, char *argv[]) {
  // Run all tests if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }
  DO_TEST(test_collisions)
  DO_TEST(test_collision_1)
  DO_TEST(test_collision_2)

  puts("collision_test PASS");
}