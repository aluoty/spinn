#ifndef SHAPES_H
#define SHAPES_H

#include "math3d.h"

typedef struct {
  Vec3 *vertices;
  int (*edges)[2];
  int num_vertices;
  int num_edges;
} Shape;

#define NUM_SHAPES 9

Shape shape_create_cube(void);
Shape shape_create_prism(void);
Shape shape_create_cylinder(void);
Shape shape_create_ball(void);
Shape shape_create_torus(void);
Shape shape_create_cone(void);
Shape shape_create_pyramid(void);
Shape shape_create_diamond(void);
Shape shape_create_star(void);

void shape_free(Shape s);

extern Shape (*shape_factories[])(void);
extern const char *shape_names[];

#endif
