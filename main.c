#include <stdio.h>
#include "rl.h"

void autocomplete(struct line *input) {
    // Append bar to whatever is currently typed.

    // Put b into the buffer
    *input->cursor++ = 'b';
    // Echo it back so the user can see what we did.
    putchar('b');

    *input->cursor++ = 'a';
    putchar('a');
    *input->cursor++ = 'r';
    putchar('r');
}

void repl() {
    // Simple read loop to demonstrate line editor.
    for (;;) {
        // Read a line.
        char* line = readln("$ ", autocomplete);
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
    raw_mode();

    repl();
    
    return 0;
}
