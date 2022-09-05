#ifndef __STAR_H__
#define __STAR_H__

#include "body.h"
#include "color.h"
#include "list.h"
#include "vector.h"

/**
 * @brief Initializes a star
 *
 * @param center center of star
 * @param num_vertices number of points in star
 * @param in_radius inner radius of star
 * @param out_radius outer radius of star
 * @param velocity initial velocity of star
 * @param angular_velocity angular velocity of star
 * @param density density of object
 * @param color color of star
 * @return body_t* star object
 */

body_t *star_init(vector_t center, int num_vertices, double in_radius,
                  double out_radius, vector_t velocity, vector_t acceleration,
                  double angular_velocity, double density, rgb_color_t color);

/**
 * @brief Negates star velocity on edges to "bounce"
 *
 * @param star star body_t
 * @param window window size for upper bound
 */

void star_bounce(body_t *star, vector_t window, double elasticity);

void side_top_bound(body_t *body, vector_t window, double elasticity);

void gravity_bounce(body_t *star, vector_t window, double elasticity);

#endif // #ifndef __STAR_H__
