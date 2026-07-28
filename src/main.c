#define _DEFAULT_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <sys/select.h>

#include "input.h"
#include "math3d.h"
#include "menu.h"
#include "render.h"
#include "shapes.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MODE_AUTO   0
#define MODE_MANUAL 1
#define MODE_WOBBLE 2
#define MODE_RANDOM 3
#define NUM_MODES   4

#define THEME_CLASSIC 0
#define THEME_NEON    1
#define THEME_OCEAN   2
#define THEME_FIRE    3
#define NUM_THEMES    4

static volatile int running = 1;

static void handle_signal(int sig) {
  (void)sig;
  running = 0;
}

static float clampf(float v, float lo, float hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

int main(void) {
  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);
  srand((unsigned)time(NULL));
  input_init();

  int shape_idx = menu_welcome();
  if (shape_idx < 0) {
    input_cleanup();
    printf("Goodbye!\n");
    return 0;
  }

  render_init();

  Shape shape = shape_factories[shape_idx]();

  float angle_x = 0.0f, angle_y = 0.0f, angle_z = 0.0f;
  float speed = 1.0f;
  int paused = 0;
  int mode = MODE_AUTO;
  float manual_speed = 0.05f;
  int theme = THEME_CLASSIC;
  int show_border = 1;
  int shapes_viewed = 1;

  /* wobble */
  float wobble_t = 0.0f;

  /* random spin */
  float rand_vx = 0.03f, rand_vy = 0.05f;
  float rand_tx = 0.03f, rand_ty = 0.05f;

  /* FPS tracking */
  struct timeval frame_start, frame_end;
  float fps_display = 0.0f;
  int fps_frames = 0;
  long fps_accum_us = 0;

  time_t start_time = time(NULL);

  gettimeofday(&frame_start, NULL);

  while (running) {
    gettimeofday(&frame_start, NULL);

    render_clear();

    /* draw border */
    if (show_border)
      render_border(0, 0, SCREEN_W - 1, SCREEN_H - 2, 90);

    /* draw shape */
    for (int i = 0; i < shape.num_edges; i++) {
      int a = shape.edges[i][0];
      int b = shape.edges[i][1];

      Vec3 v0 = rotate_x(shape.vertices[a], angle_x);
      v0 = rotate_y(v0, angle_y);
      v0 = rotate_z(v0, angle_z);
      Vec3 v1 = rotate_x(shape.vertices[b], angle_x);
      v1 = rotate_y(v1, angle_y);
      v1 = rotate_z(v1, angle_z);

      Vec2 p0 = project(v0, 40.0f, SCREEN_W, SCREEN_H);
      Vec2 p1 = project(v1, 40.0f, SCREEN_W, SCREEN_H);

      float avg_z = (p0.z + p1.z) / 2.0f;
      int color = depth_to_color(avg_z, theme);

      render_line((int)p0.x, (int)p0.y, p0.z, (int)p1.x, (int)p1.y, p1.z,
                  '\0', color);
    }

    /* HUD */
    const char *mode_names[] = {"Auto", "Manual", "Wobble", "Random"};
    const char *pause_str = paused ? " [PAUSED]" : "";
    char hud[SCREEN_W + 1];
    snprintf(hud, sizeof(hud),
             "spinn %s %s %s %.1fx | %dv/%de | %dfps | C:Theme B:Border M:Mode Q:Quit",
             mode_names[mode], pause_str, shape_names[shape_idx], speed,
             shape.num_vertices, shape.num_edges, (int)fps_display);
    render_text_colored(0, SCREEN_H - 1, hud, 90);

    render_flush();

    /* auto rotation */
    if (!paused) {
      switch (mode) {
      case MODE_AUTO:
        angle_x += 0.03f * speed;
        angle_y += 0.05f * speed;
        break;
      case MODE_WOBBLE:
        wobble_t += 0.05f * speed;
        angle_x += 0.02f * speed;
        angle_y += 0.03f * speed;
        angle_z = 0.4f * sinf(wobble_t * 2.0f) + 0.2f * cosf(wobble_t * 1.3f);
        break;
      case MODE_RANDOM:
        if (fps_frames % 15 == 0) {
          rand_tx = ((float)(rand() % 1000) / 1000.0f - 0.5f) * 0.12f;
          rand_ty = ((float)(rand() % 1000) / 1000.0f - 0.5f) * 0.12f;
        }
        rand_vx += (rand_tx - rand_vx) * 0.03f;
        rand_vy += (rand_ty - rand_vy) * 0.03f;
        angle_x += rand_vx * speed;
        angle_y += rand_vy * speed;
        break;
      }
    }

    /* input */
    int key = input_read_key();
    if (key != KEY_NONE) {
      switch (key) {
      case KEY_Q:
      case KEY_ESC:
        running = 0;
        break;

      case KEY_M:
        mode = (mode + 1) % NUM_MODES;
        break;

      case 'c':
        theme = (theme + 1) % NUM_THEMES;
        break;

      case 'b':
        show_border = !show_border;
        break;

      case KEY_SPACE:
        paused = !paused;
        break;

      case KEY_PLUS:
      case '=':
        speed = clampf(speed + 0.25f, 0.25f, 4.0f);
        manual_speed = clampf(manual_speed + 0.01f, 0.01f, 0.15f);
        break;

      case KEY_MINUS:
        speed = clampf(speed - 0.25f, 0.25f, 4.0f);
        manual_speed = clampf(manual_speed - 0.01f, 0.01f, 0.15f);
        break;

      case KEY_R:
        if (mode == MODE_AUTO) {
          angle_x = -angle_x;
          angle_y = -angle_y;
        }
        break;

      /* manual rotation */
      case KEY_W:
      case KEY_UP:
        if (mode == MODE_MANUAL && !paused)
          angle_x -= manual_speed * 5.0f;
        break;
      case KEY_S:
      case KEY_DOWN:
        if (mode == MODE_MANUAL && !paused)
          angle_x += manual_speed * 5.0f;
        break;
      case KEY_A:
      case KEY_LEFT:
        if (mode == MODE_MANUAL && !paused)
          angle_y -= manual_speed * 5.0f;
        break;
      case KEY_D:
      case KEY_RIGHT:
        if (mode == MODE_MANUAL && !paused)
          angle_y += manual_speed * 5.0f;
        break;

      /* shape switching */
      case KEY_TAB: {
        shape_free(shape);
        shape_idx = (shape_idx + 1) % NUM_SHAPES;
        shape = shape_factories[shape_idx]();
        shapes_viewed++;
        break;
      }
      default:
        if (key >= '1' && key <= '9') {
          int idx = key - '1';
          if (idx < NUM_SHAPES && idx != shape_idx) {
            shape_free(shape);
            shape_idx = idx;
            shape = shape_factories[shape_idx]();
            shapes_viewed++;
          }
        }
        break;
      }
    }

    /* FPS calculation */
    gettimeofday(&frame_end, NULL);
    long elapsed_us = (frame_end.tv_sec - frame_start.tv_sec) * 1000000L +
                      (frame_end.tv_usec - frame_start.tv_usec);
    fps_accum_us += elapsed_us;
    fps_frames++;

    if (fps_accum_us >= 500000L) {
      fps_display = (float)fps_frames / ((float)fps_accum_us / 1000000.0f);
      fps_frames = 0;
      fps_accum_us = 0;
    }

    /* cap frame rate */
    usleep(33000);
  }

  int elapsed = (int)(time(NULL) - start_time);
  render_cleanup();
  input_cleanup();

  menu_goodbye(shapes_viewed, elapsed, speed, mode, theme,
               shape.num_vertices, shape.num_edges);
  shape_free(shape);

  return 0;
}
