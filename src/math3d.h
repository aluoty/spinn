#ifndef MATH3D_H
#define MATH3D_H

typedef struct {
  float x, y, z;
} Vec3;

typedef struct {
  float x, y;
  float z; /* depth for z-buffer */
} Vec2;

Vec3 rotate_x(Vec3 v, float angle);
Vec3 rotate_y(Vec3 v, float angle);
Vec3 rotate_z(Vec3 v, float angle);

Vec2 project(Vec3 v, float fov, int screen_w, int screen_h);

#endif
