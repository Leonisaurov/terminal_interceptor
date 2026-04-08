#include "terminal.h"
#include "command.h"

#include <stdlib.h>
#include <unistd.h>

#include "debug.h"
#include "escape_code.h"

#include <time.h>

char timer_buffer[26];
char* get_time() {
    time_t t = time(NULL);
    if (t == (time_t)-1) return "ERROR";

    struct tm *lt = localtime(&t);
    strftime(timer_buffer, 26, "%H:%M", lt);
    return timer_buffer;
}

int pout(Cmd *self, const char* stdout, const int length) {
    for (int i = 0; i < length; i++) {
        EscapeCode code = extract_escape_code(stdout, &i);
        if (code.type != ESC_CODE_Unknown) {
            LOG("<ESC-CODE>");
        }
    }
    return 1;
}

const char *key = "time: ";
const unsigned short key_len = 6;
unsigned short key_match = 0;
int pin(Cmd *self, const char* stdin, const int length) {
    write(self->stdin, stdin, length);
    for (unsigned short i = 0; i < length; i++) {
        if (stdin[i] == '\b') key_match = 0;
        if (stdin[i] == key[key_match]) {
            key_match++;
            if (key_match == key_len) {
                SEND_Input(get_time());
                key_match = 0;
            }
            continue;
        }
        key_match = 0;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    // volatile int _dbg_wait = 1;
    // while (_dbg_wait){
    //     sleep(1);
    // }
    struct termios term = get_terminal(STDIN_FILENO);

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

    return 0;
}
