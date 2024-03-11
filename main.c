#include <ctype.h>
#include <stdio.h>
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
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
    /*
       Printing out each byte that we read()
       We will print each character's numeric ASCII value
       and the character it represents
    */
    
    // Tests whether a character is a control character (nonprintable characters)
    if (iscntrl(c)) 
      printf("%d\r\n", c);        // Ctrl-A will print it's ASCII value (1-26 for Cntrl combinations)
    else
      printf("%d ('%c')\r\n", c, c);
  }
  return 0;
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
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

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
