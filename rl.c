#include <termios.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "rl.h"

// Backup of terminal state so we can restore the
// old settings when we exit our program.
struct termios old_term_state;

// Text buffer for the input line.
#define BUF_SIZE 1024
char BUF[BUF_SIZE];

// A line to perform edit operations on the input.
struct line input = { BUF, BUF };

void delete(size_t n)
{
    // Move every character to the right of the
    // cursor one step to the left.
    char *del_cursor = input.cursor;
    do {
        *del_cursor = *(del_cursor + n);
    } while (*del_cursor++);
}

void insert(char c)
{
    // Make a new temporary cursor.
    char *ins_cursor = input.cursor;

    // Seek null terminal with the cursor.
    while (*ins_cursor++);

    // Go backwards, moving all characters to the
    // right of our actual cursor one step right.
    do {
        *ins_cursor = *(ins_cursor - 1);
    } while ((--ins_cursor) - input.cursor);

    // Put our character in.
    *(input.cursor++) = c;
}

void ansi_ctrl_seq()
{
    // ANSI escape (0x1B) has been read.
    // If we discover an unsupported escape sequence,
    // the safe thing to do is exit, since there may
    // be a partial sequence left in the input which
    // would not be read correctly if we procceed.

    // 0x5B ('[') indicates start of control sequence.
    if (getchar() == 0x5B) {
        // Assume no parameter bytes.
        switch (getchar()) {
          case 'A': /* Up   */ break;
          case 'B': /* Down */ break;
          case 'C':
            if (*input.cursor) {
                input.cursor++;
            }
            break;
          case 'D':
            if (input.cursor - input.buf) {
                input.cursor--;
            }
            break;
          default:
            exit(1);
        }
    } else {
        exit(1);
    }
}

void render_line()
{
    // Renders the input line using ANSI escape
    // codes to position  the terminals  cursor
    // in the same position as the line editors
    // cursor in the buffer.

    printf("\x1B[s"); // Save cursor position
    printf("%s", input.buf);
    printf("\x1B[J"); // Clear to end of line
    printf("\x1B[u"); // Restore the position

    // If the cursor is not in the start of the
    // buffer, move it forward as far as needed.
    if (input.cursor - input.buf) {
        printf("\x1B[%luC", input.cursor - input.buf);
    }
}


char *readln(const char *prompt, void (*tab)(struct line*))
{
    // We maintain the invariant that the string in 
    // the buffer is always null-terminated. Before
    // we read a character, the text is only a zero
    // byte.
    raw_mode();
    *input.buf = 0;

    // Loop to get characters and update buffer.
    for (;;) {
        // Render the line with prompt.
        printf("\xD%s", prompt);
        render_line();

        // Get input and update line.
        char c = getchar();

        if (c == '\n') {
            // reset the cursor back to start.
            input.cursor = input.buf;
            putchar('\n');
            restore_old_term_state();
            return input.buf;
        } else if (c == '\r') {
            // Ignore carrige return.
            continue;
        } else if (c == 0x7F) {
            // Backspace
            if (input.cursor - input.buf) {
                input.cursor--;
                delete(1);
            }
        } else if (c == '\t'){
            // Tab
            if (tab != NULL) {
                tab(&input);
            }
        } else if (c == 4 || c == 3) {
            // Ctrl-D or Ctrl-C respectively.
            break;
        } else if (c == 0x1B) {
            // Start of ansi escape code.
            ansi_ctrl_seq();
        } else if (c == EOF) {
            // We were reading from input redirected
            // file, and are now at the end.
            break;
        } else {
            // The normal case: Any character.
            // Put it in the buffer and echo it.
            insert(c);
        }
    }

    // If we exit the loop any other way than seeing a
    // newline character, we have not read a line, so
    // we return NULL.
    restore_old_term_state();
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
