#include "shapes.h"
#include "body.h"
#include "polygon.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

list_t *rectangle_init(double height, double width, vector_t center) {
  list_t *user = list_init(4, free);
  vector_t *v1 = malloc(1 * sizeof(vector_t));
  v1->x = center.x + width / 2;
  v1->y = center.y + height / 2;
  list_add(user, v1);

  vector_t *v2 = malloc(1 * sizeof(vector_t));
  v2->x = center.x - width / 2;
  v2->y = center.y + height / 2;
  list_add(user, v2);

  vector_t *v3 = malloc(1 * sizeof(vector_t));
  v3->x = center.x - width / 2;
  v3->y = center.y - height / 2;
  list_add(user, v3);

  vector_t *v4 = malloc(1 * sizeof(vector_t));
  v4->x = center.x + width / 2;
  v4->y = center.y - height / 2;
  list_add(user, v4);

  return user;
}

list_t *circle_init(double radius, double smoothness, vector_t center) {
  list_t *circle = list_init(smoothness, free);
  for (double i = 0; i < 2 * M_PI; i += (2 * M_PI) / smoothness) {
    vector_t *v2 = malloc(1 * sizeof(vector_t));
    v2->x = radius * cos(i) + center.x;
    v2->y = radius * sin(i) + center.y;
    list_add(circle, v2);
  }
  return circle;
}