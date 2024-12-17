#include <sys/termios.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

struct termios orig_termios;

void die(const char* s) {
    perror(s);
    exit(1);
}

void disableRawMode(void) {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        die("tcsetattr");
    }
}

void enableRawMode(void) {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    // settings for raw mode
    raw.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    raw.c_cc[VMIN] = 0; // minimum number of bytes for read() to return
    raw.c_cc[VTIME] = 1; // timeout for read(), 1/10 of a seconds

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) { // why use TCSAFLUSH but not others?
        die("tcsetattr");
    }
}

int main(void) {
    enableRawMode();

    while (1) {
        char c = '\0';
        // STDIN_FILENO = standard input
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }

        if (c == 'q') break;
    }
    return 0;
}
