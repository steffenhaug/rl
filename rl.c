#include <termios.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "rl.h"

// Backup of terminal state before entering raw mode.
struct termios old_term_state;

// Text buffer for the input line.
const size_t BUF_SIZE = 512;
char BUF[BUF_SIZE];

struct line input = { BUF, BUF };

void ansi_escape()
{
    // ANSI escape has been read.
    // If we discover an unsupported escape sequence,
    // the safe thing to do is exit, since there may
    // be a partial sequence left in the input which
    // would not be read correctly if we procceed.
    if (0x5B == getchar()) {
        // 0x5B indicates start of control sequence.
        switch (getchar()) {
          case 'A': /* Up    */ break;
          case 'B': /* Down  */ break;
          case 'C': /* Right */ break;
          case 'D': /* Left  */ break;
          default:
            exit(1);
        }
    } else {
        exit(1);
    }
}

char *readln(const char *prompt, void (*tab)(struct line*))
{
    // Read a sequence of characters from stdin
    // terminated by a newline character. Supports
    // line editing.

    // Print the prompt.
    printf("%s", prompt);

    // Read a line character by character.
    char c;
    while ((c = getchar())) {
        if (c == '\n') {
            // Newline; terminate the string and
            // reset the cursor back to start.
            *input.cursor = 0;
            input.cursor = input.buf;
            putchar('\n');
            return input.buf;
        } else if (c == '\r') {
            // Ignore carrige return.
            continue;
        } else if (c == 0x7F) {
            // Backspace
            if (input.cursor - input.buf) {
                printf("\b \b");
                input.cursor--;
            }
        } else if (c == '\t'){
            // Tab
            tab(&input);
        } else if (c == 4 || c == 3) {
            // Ctrl-D or Ctrl-C respectively.
            break;
        } else if (c == 0x1B) {
            // Start of ansi escape code.
            ansi_escape();
        } else {
            // The normal case: Any character.
            // Put it in the buffer and echo it.
            *input.cursor++ = c;
            putchar(c);
        }
    }

    // If we exit the loop any other way than seeing a
    // newline character, we have not read a line; so
    // we return NULL.
    return NULL;
}

void restore_old_term_state()
{
    // Re-apply the saved state.
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term_state);
}

void raw_mode()
{
    // Register restoring terminal settings on exit.
    atexit(restore_old_term_state);
    
    // Get the terminal state, and save a backup.
    struct termios ts;
    tcgetattr(STDIN_FILENO, &ts);
    old_term_state = ts;

    // Disable echoing, and enter raw-mode by leaving
    // canon mode (line mode) and disabling process
    // management hotkeys like Ctrl-C and Ctrl-S.
    ts.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    ts.c_iflag &= ~(IXON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &ts);
}
