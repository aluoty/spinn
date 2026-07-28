#define _DEFAULT_SOURCE
#include "render.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char buf[SCREEN_H][SCREEN_W];
static float zbuf[SCREEN_H][SCREEN_W];
static int cbuf[SCREEN_H][SCREEN_W];

void render_init(void) {
  printf("\033[?25l");
  render_clear();
}

void render_cleanup(void) {
  printf("\033[0m");
  printf("\033[?25h");
  printf("\033[%d;1H\n", SCREEN_H + 1);
}

void render_clear(void) {
  memset(buf, ' ', sizeof(buf));
  memset(cbuf, 0, sizeof(cbuf));
  for (int y = 0; y < SCREEN_H; y++)
    for (int x = 0; x < SCREEN_W; x++)
      zbuf[y][x] = 1e9f;
}

static void put_pixel(int x, int y, float z, char ch, int color) {
  if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H)
    return;
  if (z < zbuf[y][x]) {
    zbuf[y][x] = z;
    buf[y][x] = ch;
    cbuf[y][x] = color;
  }
}

static char depth_char(float z) {
  if (z < -0.3f) return '@';
  if (z < 0.0f) return '#';
  if (z < 0.3f) return '+';
  if (z < 0.6f) return '=';
  return '-';
}

int depth_to_color(float z, int theme) {
  if (theme == 0)
    return 0; /* classic: no color */

  static const int neon[] = {95, 96, 93, 92, 97};
  static const int ocean[] = {96, 94, 36, 34, 90};
  static const int fire[] = {93, 91, 33, 31, 90};

  int idx;
  if (z < -0.3f)
    idx = 0;
  else if (z < 0.0f)
    idx = 1;
  else if (z < 0.3f)
    idx = 2;
  else if (z < 0.6f)
    idx = 3;
  else
    idx = 4;

  const int *pal = (theme == 1) ? neon : (theme == 2) ? ocean : fire;
  return pal[idx];
}

void render_line(int x0, int y0, float z0, int x1, int y1, float z1, char ch,
                 int color) {
  int dx = x1 - x0;
  int dy = y1 - y0;
  int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
  if (steps == 0) {
    char c = (ch) ? ch : depth_char((z0 + z1) / 2.0f);
    int cl = color ? color : depth_to_color((z0 + z1) / 2.0f, color);
    put_pixel(x0, y0, (z0 + z1) / 2.0f, c, cl);
    return;
  }

  for (int i = 0; i <= steps; i++) {
    float t = (float)i / (float)steps;
    int x = x0 + (int)(dx * t + 0.5f);
    int y = y0 + (int)(dy * t + 0.5f);
    float z = z0 + (z1 - z0) * t;
    char c = (ch) ? ch : depth_char(z);
    put_pixel(x, y, z, c, color);
  }
}

void render_text(int x, int y, const char *text) {
  for (int i = 0; text[i] && (x + i) < SCREEN_W; i++) {
    int cx = x + i;
    if (cx >= 0 && cx < SCREEN_W && y >= 0 && y < SCREEN_H) {
      buf[y][cx] = text[i];
      zbuf[y][cx] = -999.0f;
    }
  }
}

void render_text_colored(int x, int y, const char *text, int color) {
  for (int i = 0; text[i] && (x + i) < SCREEN_W; i++) {
    int cx = x + i;
    if (cx >= 0 && cx < SCREEN_W && y >= 0 && y < SCREEN_H) {
      buf[y][cx] = text[i];
      zbuf[y][cx] = -999.0f;
      cbuf[y][cx] = color;
    }
  }
}

void render_rect(int x0, int y0, int x1, int y1, char ch) {
  for (int x = x0; x <= x1; x++) {
    put_pixel(x, y0, -999.0f, ch, 0);
    put_pixel(x, y1, -999.0f, ch, 0);
  }
  for (int y = y0; y <= y1; y++) {
    put_pixel(x0, y, -999.0f, ch, 0);
    put_pixel(x1, y, -999.0f, ch, 0);
  }
}

void render_box(int x0, int y0, int x1, int y1, int color) {
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 >= SCREEN_W) x1 = SCREEN_W - 1;
  if (y1 >= SCREEN_H) y1 = SCREEN_H - 1;

  for (int x = x0 + 1; x < x1; x++) {
    buf[y0][x] = '-';
    buf[y1][x] = '-';
    zbuf[y0][x] = -999.0f;
    zbuf[y1][x] = -999.0f;
    cbuf[y0][x] = color;
    cbuf[y1][x] = color;
  }
  for (int y = y0 + 1; y < y1; y++) {
    buf[y][x0] = '|';
    buf[y][x1] = '|';
    zbuf[y][x0] = -999.0f;
    zbuf[y][x1] = -999.0f;
    cbuf[y][x0] = color;
    cbuf[y][x1] = color;
  }
  buf[y0][x0] = '+';
  buf[y0][x1] = '+';
  buf[y1][x0] = '+';
  buf[y1][x1] = '+';
  zbuf[y0][x0] = -999.0f;
  zbuf[y0][x1] = -999.0f;
  zbuf[y1][x0] = -999.0f;
  zbuf[y1][x1] = -999.0f;
  cbuf[y0][x0] = color;
  cbuf[y0][x1] = color;
  cbuf[y1][x0] = color;
  cbuf[y1][x1] = color;
}

void render_border(int x0, int y0, int x1, int y1, int color) {
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 >= SCREEN_W) x1 = SCREEN_W - 1;
  if (y1 >= SCREEN_H) y1 = SCREEN_H - 1;

  /* corners */
  buf[y0][x0] = '+'; buf[y0][x1] = '+';
  buf[y1][x0] = '+'; buf[y1][x1] = '+';
  for (int i = 0; i < 4; i++) {
    zbuf[y0][x0] = -999.0f; cbuf[y0][x0] = color;
    zbuf[y0][x1] = -999.0f; cbuf[y0][x1] = color;
    zbuf[y1][x0] = -999.0f; cbuf[y1][x0] = color;
    zbuf[y1][x1] = -999.0f; cbuf[y1][x1] = color;
  }

  /* top/bottom */
  for (int x = x0 + 1; x < x1; x++) {
    buf[y0][x] = '=';
    buf[y1][x] = '=';
    zbuf[y0][x] = -999.0f;
    zbuf[y1][x] = -999.0f;
    cbuf[y0][x] = color;
    cbuf[y1][x] = color;
  }

  /* sides */
  for (int y = y0 + 1; y < y1; y++) {
    buf[y][x0] = '|';
    buf[y][x1] = '|';
    zbuf[y][x0] = -999.0f;
    zbuf[y][x1] = -999.0f;
    cbuf[y][x0] = color;
    cbuf[y][x1] = color;
  }
}

void render_flush(void) {
  printf("\033[H");
  int last_color = 0;
  for (int y = 0; y < SCREEN_H; y++) {
    for (int x = 0; x < SCREEN_W; x++) {
      if (cbuf[y][x] != last_color) {
        if (cbuf[y][x] == 0)
          printf("\033[0m");
        else
          printf("\033[%dm", cbuf[y][x]);
        last_color = cbuf[y][x];
      }
      putchar(buf[y][x]);
    }
    if (y < SCREEN_H - 1)
      putchar('\n');
  }
  if (last_color != 0)
    printf("\033[0m");
  fflush(stdout);
}
