#include "terminal.h"
#include "command.h"

#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int flags;

    struct termios term = set_raw_terminal(STDIN_FILENO);
    set_nonblockig_input(STDIN_FILENO, &flags);
    unset_buffering(stdout);

    char* cmd;
    if (argc == 2)
        cmd = argv[1];
    else
        cmd = getenv("SHELL");
    encapsule_cmd(term, cmd);

    set_buffering(stdout);
    set_normal_input(STDIN_FILENO, flags);
    set_normal_terminal(&term, STDIN_FILENO);
    return 0;
}
