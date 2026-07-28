#include "shapes.h"
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Shape (*shape_factories[])(void) = {
    shape_create_cube,    shape_create_prism,   shape_create_cylinder,
    shape_create_ball,    shape_create_torus,   shape_create_cone,
    shape_create_pyramid, shape_create_diamond, shape_create_star,
};

const char *shape_names[] = {"Cube",    "Prism",   "Cylinder", "Ball",
                             "Torus",   "Cone",    "Pyramid",  "Diamond",
                             "Star"};

Shape shape_create_cube(void) {
  Shape s;
  s.num_vertices = 8;
  s.num_edges = 12;
  s.vertices = malloc(sizeof(Vec3) * 8);
  s.edges = malloc(sizeof(int[2]) * 12);

  float h = 5.0f;
  s.vertices[0] = (Vec3){-h, -h, -h};
  s.vertices[1] = (Vec3){h, -h, -h};
  s.vertices[2] = (Vec3){h, h, -h};
  s.vertices[3] = (Vec3){-h, h, -h};
  s.vertices[4] = (Vec3){-h, -h, h};
  s.vertices[5] = (Vec3){h, -h, h};
  s.vertices[6] = (Vec3){h, h, h};
  s.vertices[7] = (Vec3){-h, h, h};

  s.edges[0][0] = 0; s.edges[0][1] = 1;
  s.edges[1][0] = 1; s.edges[1][1] = 2;
  s.edges[2][0] = 2; s.edges[2][1] = 3;
  s.edges[3][0] = 3; s.edges[3][1] = 0;
  s.edges[4][0] = 4; s.edges[4][1] = 5;
  s.edges[5][0] = 5; s.edges[5][1] = 6;
  s.edges[6][0] = 6; s.edges[6][1] = 7;
  s.edges[7][0] = 7; s.edges[7][1] = 4;
  s.edges[8][0] = 0; s.edges[8][1] = 4;
  s.edges[9][0] = 1; s.edges[9][1] = 5;
  s.edges[10][0] = 2; s.edges[10][1] = 6;
  s.edges[11][0] = 3; s.edges[11][1] = 7;

  return s;
}

Shape shape_create_prism(void) {
  Shape s;
  int n = 3;
  s.num_vertices = 2 * n;
  s.num_edges = 3 * n;
  s.vertices = malloc(sizeof(Vec3) * s.num_vertices);
  s.edges = malloc(sizeof(int[2]) * s.num_edges);

  float r = 5.0f, h = 6.0f;
  for (int i = 0; i < n; i++) {
    float angle = (float)i * 2.0f * (float)M_PI / (float)n -
                  (float)M_PI / 2.0f;
    s.vertices[i] = (Vec3){r * cosf(angle), r * sinf(angle), -h};
    s.vertices[i + n] = (Vec3){r * cosf(angle), r * sinf(angle), h};
  }

  for (int i = 0; i < n; i++) {
    s.edges[i][0] = i;
    s.edges[i][1] = (i + 1) % n;
  }
  for (int i = 0; i < n; i++) {
    s.edges[n + i][0] = n + i;
    s.edges[n + i][1] = n + (i + 1) % n;
  }
  for (int i = 0; i < n; i++) {
    s.edges[2 * n + i][0] = i;
    s.edges[2 * n + i][1] = n + i;
  }

  return s;
}

Shape shape_create_cylinder(void) {
  Shape s;
  int n = 16;
  s.num_vertices = 2 * n;
  s.num_edges = 3 * n;
  s.vertices = malloc(sizeof(Vec3) * s.num_vertices);
  s.edges = malloc(sizeof(int[2]) * s.num_edges);

  float r = 4.0f, h = 6.0f;
  for (int i = 0; i < n; i++) {
    float angle = (float)i * 2.0f * (float)M_PI / (float)n;
    s.vertices[i] = (Vec3){r * cosf(angle), r * sinf(angle), -h};
    s.vertices[i + n] = (Vec3){r * cosf(angle), r * sinf(angle), h};
  }

  for (int i = 0; i < n; i++) {
    s.edges[i][0] = i;
    s.edges[i][1] = (i + 1) % n;
  }
  for (int i = 0; i < n; i++) {
    s.edges[n + i][0] = n + i;
    s.edges[n + i][1] = n + (i + 1) % n;
  }
  for (int i = 0; i < n; i++) {
    s.edges[2 * n + i][0] = i;
    s.edges[2 * n + i][1] = n + i;
  }

  return s;
}

Shape shape_create_ball(void) {
  Shape s;
  int lat = 10, lon = 20;
  int nv = lat * lon;
  int ne = lat * lon + (lat - 1) * lon;
  s.num_vertices = nv;
  s.num_edges = ne;
  s.vertices = malloc(sizeof(Vec3) * nv);
  s.edges = malloc(sizeof(int[2]) * ne);

  float r = 6.0f;
  for (int i = 0; i < lat; i++) {
    float theta = (float)(i + 1) * (float)M_PI / (float)(lat + 1);
    for (int j = 0; j < lon; j++) {
      float phi = (float)j * 2.0f * (float)M_PI / (float)lon;
      int idx = i * lon + j;
      s.vertices[idx] = (Vec3){r * sinf(theta) * cosf(phi),
                                r * sinf(theta) * sinf(phi),
                                r * cosf(theta)};
    }
  }

  int e = 0;
  for (int i = 0; i < lat; i++) {
    for (int j = 0; j < lon; j++) {
      s.edges[e][0] = i * lon + j;
      s.edges[e][1] = i * lon + (j + 1) % lon;
      e++;
    }
  }
  for (int j = 0; j < lon; j++) {
    for (int i = 0; i < lat - 1; i++) {
      s.edges[e][0] = i * lon + j;
      s.edges[e][1] = (i + 1) * lon + j;
      e++;
    }
  }

  return s;
}

Shape shape_create_torus(void) {
  Shape s;
  int R = 12, r = 6; /* major/minor segments */
  int nv = R * r;
  int ne = 2 * R * r;
  s.num_vertices = nv;
  s.num_edges = ne;
  s.vertices = malloc(sizeof(Vec3) * nv);
  s.edges = malloc(sizeof(int[2]) * ne);

  float major_r = 4.0f, minor_r = 2.0f;
  for (int i = 0; i < R; i++) {
    float theta = (float)i * 2.0f * (float)M_PI / (float)R;
    for (int j = 0; j < r; j++) {
      float phi = (float)j * 2.0f * (float)M_PI / (float)r;
      int idx = i * r + j;
      float cr = major_r + minor_r * cosf(phi);
      s.vertices[idx] = (Vec3){cr * cosf(theta), cr * sinf(theta),
                                minor_r * sinf(phi)};
    }
  }

  int e = 0;
  for (int i = 0; i < R; i++) {
    for (int j = 0; j < r; j++) {
      s.edges[e][0] = i * r + j;
      s.edges[e][1] = i * r + (j + 1) % r;
      e++;
      s.edges[e][0] = i * r + j;
      s.edges[e][1] = ((i + 1) % R) * r + j;
      e++;
    }
  }

  return s;
}

Shape shape_create_cone(void) {
  Shape s;
  int n = 16;
  s.num_vertices = n + 1;
  s.num_edges = 2 * n;
  s.vertices = malloc(sizeof(Vec3) * (n + 1));
  s.edges = malloc(sizeof(int[2]) * 2 * n);

  float r = 5.0f, h = 8.0f;
  for (int i = 0; i < n; i++) {
    float angle = (float)i * 2.0f * (float)M_PI / (float)n;
    s.vertices[i] = (Vec3){r * cosf(angle), r * sinf(angle), -h / 2.0f};
  }
  s.vertices[n] = (Vec3){0.0f, 0.0f, h / 2.0f};

  for (int i = 0; i < n; i++) {
    s.edges[i][0] = i;
    s.edges[i][1] = (i + 1) % n;
  }
  for (int i = 0; i < n; i++) {
    s.edges[n + i][0] = i;
    s.edges[n + i][1] = n;
  }

  return s;
}

Shape shape_create_pyramid(void) {
  Shape s;
  s.num_vertices = 5;
  s.num_edges = 8;
  s.vertices = malloc(sizeof(Vec3) * 5);
  s.edges = malloc(sizeof(int[2]) * 8);

  float b = 5.0f, h = 8.0f;
  s.vertices[0] = (Vec3){-b, -b, -b};
  s.vertices[1] = (Vec3){b, -b, -b};
  s.vertices[2] = (Vec3){b, -b, b};
  s.vertices[3] = (Vec3){-b, -b, b};
  s.vertices[4] = (Vec3){0.0f, h, 0.0f};

  s.edges[0][0] = 0; s.edges[0][1] = 1;
  s.edges[1][0] = 1; s.edges[1][1] = 2;
  s.edges[2][0] = 2; s.edges[2][1] = 3;
  s.edges[3][0] = 3; s.edges[3][1] = 0;
  s.edges[4][0] = 0; s.edges[4][1] = 4;
  s.edges[5][0] = 1; s.edges[5][1] = 4;
  s.edges[6][0] = 2; s.edges[6][1] = 4;
  s.edges[7][0] = 3; s.edges[7][1] = 4;

  return s;
}

Shape shape_create_diamond(void) {
  Shape s;
  s.num_vertices = 6;
  s.num_edges = 12;
  s.vertices = malloc(sizeof(Vec3) * 6);
  s.edges = malloc(sizeof(int[2]) * 12);

  float b = 4.0f, h = 6.0f;
  s.vertices[0] = (Vec3){-b, 0.0f, -b};
  s.vertices[1] = (Vec3){b, 0.0f, -b};
  s.vertices[2] = (Vec3){b, 0.0f, b};
  s.vertices[3] = (Vec3){-b, 0.0f, b};
  s.vertices[4] = (Vec3){0.0f, h, 0.0f};
  s.vertices[5] = (Vec3){0.0f, -h, 0.0f};

  /* top pyramid edges */
  s.edges[0][0] = 0; s.edges[0][1] = 1;
  s.edges[1][0] = 1; s.edges[1][1] = 2;
  s.edges[2][0] = 2; s.edges[2][1] = 3;
  s.edges[3][0] = 3; s.edges[3][1] = 0;
  s.edges[4][0] = 0; s.edges[4][1] = 4;
  s.edges[5][0] = 1; s.edges[5][1] = 4;
  s.edges[6][0] = 2; s.edges[6][1] = 4;
  s.edges[7][0] = 3; s.edges[7][1] = 4;
  /* bottom pyramid edges */
  s.edges[8][0] = 0; s.edges[8][1] = 5;
  s.edges[9][0] = 1; s.edges[9][1] = 5;
  s.edges[10][0] = 2; s.edges[10][1] = 5;
  s.edges[11][0] = 3; s.edges[11][1] = 5;

  return s;
}

Shape shape_create_star(void) {
  Shape s;
  s.num_vertices = 14;
  s.num_edges = 24;
  s.vertices = malloc(sizeof(Vec3) * 14);
  s.edges = malloc(sizeof(int[2]) * 24);

  float tip = 7.0f, mid = 3.0f;
  /* 6 tips along +/-XYZ axes */
  s.vertices[0] = (Vec3){tip, 0.0f, 0.0f};
  s.vertices[1] = (Vec3){-tip, 0.0f, 0.0f};
  s.vertices[2] = (Vec3){0.0f, tip, 0.0f};
  s.vertices[3] = (Vec3){0.0f, -tip, 0.0f};
  s.vertices[4] = (Vec3){0.0f, 0.0f, tip};
  s.vertices[5] = (Vec3){0.0f, 0.0f, -tip};
  /* inner octahedron vertices */
  s.vertices[6] = (Vec3){mid, mid, mid};
  s.vertices[7] = (Vec3){-mid, mid, mid};
  s.vertices[8] = (Vec3){mid, -mid, mid};
  s.vertices[9] = (Vec3){-mid, -mid, mid};
  s.vertices[10] = (Vec3){mid, mid, -mid};
  s.vertices[11] = (Vec3){-mid, mid, -mid};
  s.vertices[12] = (Vec3){mid, -mid, -mid};
  s.vertices[13] = (Vec3){-mid, -mid, -mid};

  /* connect each tip to its 4 nearest inner vertices */
  int e = 0;
  /* +X tip */
  s.edges[e][0] = 0; s.edges[e++][1] = 6;
  s.edges[e][0] = 0; s.edges[e++][1] = 8;
  s.edges[e][0] = 0; s.edges[e++][1] = 10;
  s.edges[e][0] = 0; s.edges[e++][1] = 12;
  /* -X tip */
  s.edges[e][0] = 1; s.edges[e++][1] = 7;
  s.edges[e][0] = 1; s.edges[e++][1] = 9;
  s.edges[e][0] = 1; s.edges[e++][1] = 11;
  s.edges[e][0] = 1; s.edges[e++][1] = 13;
  /* +Y tip */
  s.edges[e][0] = 2; s.edges[e++][1] = 6;
  s.edges[e][0] = 2; s.edges[e++][1] = 7;
  s.edges[e][0] = 2; s.edges[e++][1] = 10;
  s.edges[e][0] = 2; s.edges[e++][1] = 11;
  /* -Y tip */
  s.edges[e][0] = 3; s.edges[e++][1] = 8;
  s.edges[e][0] = 3; s.edges[e++][1] = 9;
  s.edges[e][0] = 3; s.edges[e++][1] = 12;
  s.edges[e][0] = 3; s.edges[e++][1] = 13;
  /* +Z tip */
  s.edges[e][0] = 4; s.edges[e++][1] = 6;
  s.edges[e][0] = 4; s.edges[e++][1] = 7;
  s.edges[e][0] = 4; s.edges[e++][1] = 8;
  s.edges[e][0] = 4; s.edges[e++][1] = 9;
  /* -Z tip */
  s.edges[e][0] = 5; s.edges[e++][1] = 10;
  s.edges[e][0] = 5; s.edges[e++][1] = 11;
  s.edges[e][0] = 5; s.edges[e++][1] = 12;
  s.edges[e][0] = 5; s.edges[e++][1] = 13;

  return s;
}

void shape_free(Shape s) {
  free(s.vertices);
  free(s.edges);
}
