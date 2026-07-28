#define _DEFAULT_SOURCE
#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

static struct termios orig_termios;

void input_init(void) {
  struct termios raw;
  tcgetattr(STDIN_FILENO, &orig_termios);
  raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON | ISIG);
  raw.c_iflag &= ~(IXON | ICRNL);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void input_cleanup(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static int has_input(void) {
  fd_set fds;
  struct timeval tv = {0, 0};
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

int input_read_key(void) {
  if (!has_input())
    return KEY_NONE;

  unsigned char c;
  if (read(STDIN_FILENO, &c, 1) != 1)
    return KEY_NONE;

  if (c == '\033') {
    /* escape sequence: \033 [ <key> */
    if (!has_input())
      return KEY_ESC;

    unsigned char seq[2];
    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return KEY_ESC;
    if (seq[0] != '[') {
      return KEY_ESC;
    }
    if (!has_input())
      return KEY_NONE;
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return KEY_NONE;

    switch (seq[1]) {
    case 'A': return KEY_UP;
    case 'B': return KEY_DOWN;
    case 'C': return KEY_RIGHT;
    case 'D': return KEY_LEFT;
    default:  return KEY_NONE;
    }
  }

  return (int)c;
}
