// A line to edit.
struct line {
    char *const buf;
    char *cursor;
};

// Prompts the user, lets them edit a line, and
// returns pointer to the line when a newline
// character is read from stdin.
char *readln(
    const char *prompt, 
    void (*tab)(struct line*)
);

// Put the terminal into raw mode so we can provide
// our own editing facilities.
void raw_mode();
