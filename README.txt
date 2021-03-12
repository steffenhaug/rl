Dead simple line editor.


It supports the bare minimum you would expect:
- Backspace to delete text
- Left and right arrow keys to move cursor
- Inserting and deliting text in the middle of input


Other ANSI escape codes than the arrow keys are not
supported yet. For example, Delete, Home, End & Co
are not supported and will cause the reader to exit,
as will the function keys F1, F2 etc.

Modifying the ANSI escape code recognition to use a
state machine (Lex) might allow supporting this more
easily, but at the moment hard coding parsing every
useful escape code in a tree of if-else and switch
statements is just too tedious.
