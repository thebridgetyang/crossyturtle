#ifndef __SHAPES_H__
#define __SHAPES_H__

#include "body.h"
#include "color.h"
#include "list.h"
#include "vector.h"

/**
 * @brief Makes list of star vertices
 *
 * @param height star body_t
 * @param width window size for upper bound
 * @param center Center of rectangle
 * @return list_t of vertices of rectangle counterclockwise starting from upper
 * right hand corner
 */
list_t *rectangle_init(double height, double width, vector_t center);

/**
 * @brief Makes list of circle vertices
 *
 * @param radius radius of circle
 * @param smoothness Number of points on circle
 * @param center Center of circle
 * @return list_t of vertices of circle counterclockwise starting from 0 degrees
 */
list_t *circle_init(double radius, double smoothness, vector_t center);

#endif // #ifndef __SHAPES_H__
