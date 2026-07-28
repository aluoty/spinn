#ifndef RENDER_H
#define RENDER_H

#define SCREEN_W 80
#define SCREEN_H 40

void render_init(void);
void render_cleanup(void);
void render_clear(void);
void render_line(int x0, int y0, float z0, int x1, int y1, float z1, char ch,
                 int color);
void render_text(int x, int y, const char *text);
void render_text_colored(int x, int y, const char *text, int color);
void render_rect(int x0, int y0, int x1, int y1, char ch);
void render_box(int x0, int y0, int x1, int y1, int color);
void render_border(int x0, int y0, int x1, int y1, int color);
void render_flush(void);

int depth_to_color(float z, int theme);

#endif
