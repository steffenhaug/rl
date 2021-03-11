#include <termios.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

const size_t BUF_SIZE = 512;
char BUF[BUF_SIZE];
char *CURSOR = BUF;

char* readln(const char* prompt) {
    // Read a sequence of characters from stdin
    // terminated by a newline character. Supports
    // line editing.

    // Print the prompt.
    printf("%s", prompt);
    fflush(stdout);

    // Read a line character by character.
    char c;
    while ((c = getchar())) {
        if (c == '\n') {
            // Newline; terminate the string and
            // reset the cursor back to start.
            *CURSOR = 0;
            CURSOR = BUF;
            putchar('\n');
            return BUF;
        } else if (c == '\r') {
            // Ignore carrige return.
            continue;
        } else if (c == 0x7f) {
            // Backspace
            if (CURSOR - BUF) {
                printf("\b \b");
                CURSOR--;
            }
        } else if (c == '\t'){
            // Tab (call autocomplete function :D)
        } else if (c == 4 || c == 3) {
            // Ctrl-D or Ctrl-C respectively.
            break;
        } else if (c == 0x1B) {
            // Start of ansi escape code.
            printf("ANSI escape codes in the input is not supported yet.");
        } else {
            // The normal case: Any character.
            // Put it in the buffer and echo it.
            *CURSOR++ = c;
            putchar(c);
        }
    }

    // If we exit the loop any other way than seeing a
    // newline character, we have not read a line; so
    // we return NULL.
    return NULL;
}

void repl() {
    // Simple read loop to demonstrate line editor.
    for (;;) {
        // Read a line.
        char* line = readln("$ ");
        if (line) {
            // If we got a line back, echo it.
            printf("(echo)%s\n", line);
        } else {
            // Otherwise exit.
            printf("\nGoodbye!\n");
            return;
        }
    }
}

int main() {
    struct termios tp, save;

    // Get the termios state, and save a backup.
    tcgetattr(STDIN_FILENO, &tp);
    save = tp;

    // Disable echoing, and enable non-canonical mode so
    // we can provide our own editing facilities.
    tp.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    tp.c_iflag &= ~(IXON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp);

    repl();
    
    // Re-apply the saved state.
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &save);

    return 0;
}
