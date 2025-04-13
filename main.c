#include "terminal.h"
#include "command.h"

#include <unistd.h>

int main() {
    int flags;

    struct termios term = set_raw_terminal(STDIN_FILENO);
    set_nonblockig_input(STDIN_FILENO, &flags);
    unset_buffering(stdout);

    encapsule_cmd(term, "bash");

    set_buffering(stdout);
    set_normal_input(STDIN_FILENO, flags);
    set_normal_terminal(&term, STDIN_FILENO);
    return 0;
}
