#include "menu.h"
#include "input.h"
#include "render.h"
#include "shapes.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/select.h>

static void clear_screen(void) {
  printf("\033[2J\033[H");
  fflush(stdout);
}

/* Print a 55-char line: | + 53 padded chars + | */
static void cl(const char *color, const char *text) {
  if (color)
    printf("|%s%-53s\033[0m|\n", color, text);
  else
    printf("|%-53s|\n", text);
}

/* printf-format a 55-char line (ANSI-aware padding) */
static void clf(const char *fmt, ...) {
  char raw[200];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(raw, sizeof(raw), fmt, ap);
  va_end(ap);
  /* strip ANSI codes to count visible chars, then pad */
  int vis = 0;
  for (int i = 0; raw[i]; i++) {
    if (raw[i] == '\033') { while (raw[i] && raw[i] != 'm') i++; continue; }
    vis++;
  }
  printf("|%s%*s|\n", raw, 53 - vis > 0 ? 53 - vis : 0, "");
}

/* Print a raw 55-char line (no wrapping) */
#define PL(s) printf("%s", s "\n")

#define C_B   "\033[1m"
#define C_D   "\033[90m"
#define C_R   "\033[91m"
#define C_G   "\033[92m"
#define C_Y   "\033[93m"
#define C_BL  "\033[94m"
#define C_MG  "\033[95m"
#define C_CY  "\033[96m"

int menu_welcome(void) {
  clear_screen();

  for (;;) {
    printf("\033[H");

    /* Header */
    PL("+=====================================================+");
    PL("|                                                     |");
    cl(C_CY, "             _____       _              ");
    cl(C_CY, "            / ___/____  (_)___  ____    ");
    cl(C_CY, "            \\__ \\/ __ \\/ / __ \\/ __ \\  ");
    cl(C_CY, "            ___/ / /_/ / / / / / / / /  ");
    cl(C_CY, "           /____/ .___/_/_/ /_/_/ /_/   ");
    cl(C_CY, "               /_/                      ");
    PL("|                                                     |");
    PL("+=====================================================+");

    /* Shape selection */
    PL("|                                                     |");
    clf("  " C_B "Select a shape:" "\033[0m                                    ");
    PL("|                                                     |");
    clf("    " C_G "[1]" "\033[0m Cube      " C_G "[2]" "\033[0m Prism     " C_G "[3]" "\033[0m Cylinder         ");
    clf("    " C_G "[4]" "\033[0m Ball      " C_G "[5]" "\033[0m Torus     " C_G "[6]" "\033[0m Cone             ");
    clf("    " C_G "[7]" "\033[0m Pyramid   " C_G "[8]" "\033[0m Diamond   " C_G "[9]" "\033[0m Star             ");
    PL("|                                                     |");

    /* Themes */
    clf("  " C_B "Themes:" "\033[0m                                            ");
    clf("    " C_MG "C" "\033[0m Classic  " C_CY "N" "\033[0m Neon      " C_BL "O" "\033[0m Ocean              ");
    clf("    " C_R "F" "\033[0m Fire     " C_D "(select before spinning)" "\033[0m              ");
    PL("|                                                     |");

    /* Controls */
    clf("  " C_B "Controls:" "\033[0m                                          ");
    clf("    " C_Y "M" "\033[0m .... Cycle mode (Auto/Manual/Wobble/Random)  ");
    clf("    " C_Y "WASD" "\033[0m/" C_Y "Arrows" "\033[0m .. Manual rotation (Manual mode)     ");
    clf("    " C_Y "+/-" "\033[0m .......... Speed up/down                     ");
    clf("    " C_Y "SPACE" "\033[0m ........ Pause/Resume                      ");
    clf("    " C_Y "TAB/1-9" "\033[0m ...... Switch shape                      ");
    clf("    " C_Y "R" "\033[0m ............ Reverse rotation                  ");
    clf("    " C_Y "B" "\033[0m ............ Toggle border                     ");
    clf("    " C_Y "Q" "\033[0m ............ Quit                              ");
    PL("|                                                     |");
    PL("+=====================================================+");

    clf("  Press " C_G "1-%d" "\033[0m to start, or " C_R "Q" "\033[0m to quit", NUM_SHAPES);
    fflush(stdout);

    for (;;) {
      int key = input_read_key();
      if (key == KEY_NONE) {
        struct timeval tv = {0, 10000};
        select(0, NULL, NULL, NULL, &tv);
        continue;
      }
      if (key == KEY_Q || key == KEY_ESC)
        return -1;
      if (key >= '1' && key <= '9') {
        int idx = key - '1';
        if (idx < NUM_SHAPES)
          return idx;
      }
    }
  }
}

void menu_goodbye(int shapes_viewed, int seconds, float speed, int mode,
                  int theme, int verts, int edges) {
  clear_screen();

  int min = seconds / 60;
  int sec = seconds % 60;
  const char *mode_name[] = {"Auto", "Manual", "Wobble", "Random"};
  const char *theme_name[] = {"Classic", "Neon", "Ocean", "Fire"};
  char buf[200];

  PL("+=====================================================+");
  PL("|                                                     |");
  cl(C_CY, "             _____       _              ");
  cl(C_CY, "            / ___/____  (_)___  ____    ");
  cl(C_CY, "            \\__ \\/ __ \\/ / __ \\/ __ \\  ");
  cl(C_CY, "            ___/ / /_/ / / / / / / / /  ");
  cl(C_CY, "           /____/ .___/_/_/ /_/_/ /_/   ");
  cl(C_CY, "               /_/                      ");
  PL("|                                                     |");
  cl(C_CY, "              " C_Y "G O O D B Y E !" C_CY "                        ");
  PL("|                                                     |");
  PL("+=====================================================+");
  PL("|                                                     |");
  snprintf(buf, sizeof(buf), "     Thanks for spinning with spinn.%s", "");
  cl(C_D, buf);
  PL("|                                                     |");
  /* inner stats box */
  cl(C_CY, "   +-------------------------------------------+     ");
  snprintf(buf, sizeof(buf), "   |  Shapes viewed:  %-25d|     ", shapes_viewed);
  cl(C_CY, buf);
  snprintf(buf, sizeof(buf), "   |  Time spent:     %2dm %02ds                  |     ", min, sec);
  cl(C_CY, buf);
  snprintf(buf, sizeof(buf), "   |  Mode:           %-25s|     ", mode_name[mode]);
  cl(C_CY, buf);
  snprintf(buf, sizeof(buf), "   |  Speed:          %-5.1fx                   |     ", speed);
  cl(C_CY, buf);
  snprintf(buf, sizeof(buf), "   |  Theme:          %-25s|     ", theme_name[theme]);
  cl(C_CY, buf);
  snprintf(buf, sizeof(buf), "   |  Vertices:       %-25d|     ", verts);
  cl(C_CY, buf);
  snprintf(buf, sizeof(buf), "   |  Edges:          %-25d|     ", edges);
  cl(C_CY, buf);
  cl(C_CY, "   +-------------------------------------------+     ");
  PL("|                                                     |");
  cl(NULL, "              See you next time!                     ");
  PL("|                                                     |");
  PL("+=====================================================+");
  fflush(stdout);
}
