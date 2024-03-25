#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/***** defines *****/
// How Ctrl works in general sets top 3 bits to 0 and the 5 rest of the bits
// to what it was before
#define CTRL_KEY(k) ((k) & 0x1f)

// Saving the initial termios state in struct 
struct termios orig_termios;

// Enables raw mode so that keyboard strokes being entered
// when the program is running is not visible
void enableRawMode();

// Disables raw mode and sets terminal's attributes to its original
// state when the program exits
void disableRawMode();

// Enabling error handling 
void die(const char* s);

// Function that waits for one keypress and returns it
char editorReadKey();

// Waits for a keypress then handles it
// Will end up mapping various <Ctrl> key combinations and other
// special keys to different editor functions
void editorProcessKeypress();

int main() {
  enableRawMode();

  while(1) {
      editorProcessKeypress();
    }

  return 0;
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = orig_termios;

  /*
   ECHO bitflag shows what a user is typing and is a bit flag 
   so we are ANDing it with the termios current c_lflag state 
   so that every bit retains its value

   ICANON bit flag processes input character by character instead of by
   line - this is important for eg in vim when we want the pressing of a 
   character to automatically do a command instead of waiting for an <Enter>

   ISIG bitflag turns off <Ctrl-C> and <Ctrl-Z> from sending SIGINT and SIGTSTP signals
   to the current process

   IXON bitflag prevents <Ctrl-S> (stops data from being transmitted to the terminal
   you press <Ctrl-Q>)

  IEXTEN bitflag prevents <Ctrl-V> and <Ctrl-O> from messing with the process

  ICRNL bitflag allows <Ctrl-M> to correctly output 13 instead of 10 (Ctrl-M has historically
  been set as the carriage return - when we press enter or return in terminal we go to the beginning
  of a new line)

  OPOST bitflag is similar to the ICRNL bitflag except for output since "\n" is translated to "\r\n"
  */

  raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
  raw.c_oflag &= !~(OPOST);
  raw.c_cflag &= ~(CS8);
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);


  /*
  * Sets a timeout so that read() returns if it doesn't get any input for a certain amount of time
  * VMIN sets the minimum number of bytes of input needed before read() can return
  *     VMIN = 0 meaans read() returns as soon as there is any input to be read 
  * VTIME sets the mximum amount of time to wait before read() returns it is in tenths of a second
  *     VTIME = 1 means we are setting time to 1/10 of a second or 100 milliseconeds
  * If read() times out it will return 0
  */
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void die(const char* s) {
  perror(s);
  exit(1);
}

char editorReadKey() {
  int nread;
  char c;

  while ( (nread = read(STDIN_FILENO, &c, 1)) != -1)
    if (nread == -1 && errno != EAGAIN) die("read");

  return c;
}


void editorProcessKeypress() {
  char c = editorReadKey();

  switch (c) {
    case CTRL_KEY('q'):
      exit(0);
      break;
  }
}
