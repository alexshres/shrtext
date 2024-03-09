#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

// Saving the initial termios state in struct 
struct termios orig_termios;

// Enables raw mode so that keyboard strokes being entered
// when the program is running is not visible
void enableRawMode();

// Disables raw mode and sets terminal's attributes to its original
// state when the program exits
void disableRawMode();

int main() {
  enableRawMode();

  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
  return 0;
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);

  struct termios raw = orig_termios;

  // ECHO bitflag shows what a user is typing and is a bit flag 
  // so we are ANDing it with the termios current c_lflag state 
  // so that every bit retains its value
  raw.c_lflag &= ~(ECHO);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios)
}
