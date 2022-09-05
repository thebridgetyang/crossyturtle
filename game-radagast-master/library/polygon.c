#include "polygon.h"
#include "list.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

double const AREA_CONST = 0.5;
double const CENTROID_CONST = 6;
double polygon_area(list_t *polygon) {
  double area = 0;
  size_t len = list_size(polygon) - 1;
  for (size_t i = 0; i < len + 1; i++) {
    area += ((vector_t *)list_get(polygon, i % (len + 1)))->x *
                ((vector_t *)list_get(polygon, (i + 1) % (len + 1)))->y -
            ((vector_t *)list_get(polygon, (i + 1) % (len + 1)))->x *
                ((vector_t *)list_get(polygon, i % (len + 1)))->y;
  }
  return area * AREA_CONST;
}

vector_t polygon_centroid(list_t *polygon) {
  vector_t centroid;
  size_t len = list_size(polygon) - 1;
  double x = 0;
  double y = 0;
  double area = polygon_area(polygon);
  for (size_t i = 0; i < len + 1; i++) {
    x += (((vector_t *)list_get(polygon, i % (len + 1)))->x +
          ((vector_t *)list_get(polygon, (i + 1) % (len + 1)))->x) *
         (((vector_t *)list_get(polygon, i % (len + 1)))->x *
              ((vector_t *)list_get(polygon, (i + 1) % (len + 1)))->y -
          ((vector_t *)list_get(polygon, (i + 1) % (len + 1)))->x *
              ((vector_t *)list_get(polygon, i % (len + 1)))->y);

    y += (((vector_t *)list_get(polygon, i % (len + 1)))->y +
          ((vector_t *)list_get(polygon, (i + 1) % (len + 1)))->y) *
         (((vector_t *)list_get(polygon, i % (len + 1)))->x *
              ((vector_t *)list_get(polygon, (i + 1) % (len + 1)))->y -
          ((vector_t *)list_get(polygon, (i + 1) % (len + 1)))->x *
              ((vector_t *)list_get(polygon, i % (len + 1)))->y);
  }
  centroid.x = x / (area * CENTROID_CONST);
  centroid.y = y / (area * CENTROID_CONST);
  return centroid;
}

void polygon_translate(list_t *polygon, vector_t translation) {
  size_t len = list_size(polygon);
  for (size_t i = 0; i < len; i++) {
    vector_t *temp = (vector_t *)list_get(polygon, i);
    *temp = vec_add(translation, *temp);
  }
};

void polygon_rotate(list_t *polygon, double angle, vector_t point) {
  size_t len = list_size(polygon);
  for (size_t i = 0; i < len; i++) {
    vector_t *temp = (vector_t *)list_get(polygon, i);
    *temp = vec_subtract(*temp, point);
    *temp = vec_rotate(*temp, angle);
    *temp = vec_add(*temp, point);
  }
};