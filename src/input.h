#ifndef INPUT_H
#define INPUT_H

#define KEY_NONE    -1
#define KEY_W       'w'
#define KEY_A       'a'
#define KEY_S       's'
#define KEY_D       'd'
#define KEY_SPACE   ' '
#define KEY_TAB     '\t'
#define KEY_ENTER   '\n'
#define KEY_ESC     27
#define KEY_PLUS    '+'
#define KEY_MINUS   '-'
#define KEY_M       'm'
#define KEY_R       'r'
#define KEY_Q       'q'
#define KEY_UP      1000
#define KEY_DOWN    1001
#define KEY_LEFT    1002
#define KEY_RIGHT   1003

void input_init(void);
void input_cleanup(void);
int  input_read_key(void);

#endif
