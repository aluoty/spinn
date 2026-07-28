#include "math3d.h"
#include <math.h>

Vec3 rotate_x(Vec3 v, float a) {
  float c = cosf(a), s = sinf(a);
  return (Vec3){v.x, v.y * c - v.z * s, v.y * s + v.z * c};
}

Vec3 rotate_y(Vec3 v, float a) {
  float c = cosf(a), s = sinf(a);
  return (Vec3){v.x * c + v.z * s, v.y, -v.x * s + v.z * c};
}

Vec3 rotate_z(Vec3 v, float a) {
  float c = cosf(a), s = sinf(a);
  return (Vec3){v.x * c - v.y * s, v.x * s + v.y * c, v.z};
}

Vec2 project(Vec3 v, float fov, int screen_w, int screen_h) {
  float scale = fov / (fov + v.z);
  return (Vec2){
      .x = v.x * scale * 2.0f + (float)screen_w / 2.0f,
      .y = v.y * scale + (float)screen_h / 2.0f,
      .z = v.z,
  };
}
