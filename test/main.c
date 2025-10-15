#include "terminal.h"
#include "command.h"

#include <stdlib.h>
#include <unistd.h>

#include "debug.h"

const char* outSequence = "wasap";
const int outSequenceLen = 5;
int outSequenceConsume = 0;
int pout(Cmd *self, const char* stdout, const int length) {
    for (int i = 0; i < length; i++) {
        if (stdout[i] == outSequence[outSequenceConsume]) {
            outSequenceConsume++;
            if (outSequenceConsume == outSequenceLen) {
                SEND_Output("What's up?");
                outSequenceConsume = 0;
            }
        } else {
            for (int i = 0; i < outSequenceConsume; i++) {
                SEND_char_Out(outSequence[i]);
            }
            outSequenceConsume = 0;
            SEND_char_Out(stdout[i]);
        }
    }
    return 0;
}

const char* inSequence = "Time: ";
const int inSequenceLen = 6;
int inSequenceConsume = 0;
int pin(Cmd *self, const char* stdin, const int length) {
    for (int i = 0; i < length; i++) {
        SEND_char_In(stdin[i]);
        if (stdin[i] == inSequence[inSequenceConsume]) {
            inSequenceConsume++;
            if (inSequenceConsume == inSequenceLen) {
                SEND_Input("17:08");
                inSequenceConsume = 0;
            }
        } else {
            inSequenceConsume = 0;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int flags;

    struct termios term = set_raw_terminal(STDIN_FILENO);
    set_nonblockig_input(STDIN_FILENO, &flags);
    unset_buffering(stdout);

    Cmd command = {0};
    command.term_snap = term;
    command.processStdout = pout;
    command.processStdin = pin;

    if (argc > 1) {
        LOG("Running user cmd: "); 
        for (int i = 1; i < argc; i++) {
            if (i != 1) {
                LOG(" ");
            }
            LOG(argv[i]);
        }
        NL;
        command.command = (const char**) argv + 1;
    } else {
        const char* cmd[] = {
            getenv("SHELL"),
            NULL,
        };
        LOG("Founded Shell: "); LOG(cmd[0]); NL;
        LOG("Running Shell"); NL;
        command.command = cmd;
    }
    encapsule_cmd(&command);

    set_buffering(stdout);
    set_normal_input(STDIN_FILENO, flags);
    set_normal_terminal(&term, STDIN_FILENO);
    return 0;
}
