#ifndef RL_H
#define RL_H
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

void insert(char c);
void delete();

// Put the terminal into raw mode so we can provide
// our own editing facilities.
void raw_mode();
void restore_old_term_state();
#endif
