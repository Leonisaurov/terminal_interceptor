#ifndef TERMINAL_INTERCEP_COMMAND
#define TERMINAL_INTERCEP_COMMAND

#include <termios.h>
#include <stdlib.h>

typedef struct Cmd Cmd;

struct Cmd {
    struct termios term_snap;
    const char **command;
    void* context;
    int (*onRun) (Cmd *self);
    int (*processStdout) (Cmd *self, const char* stdout, const int stdout_len);
    int (*processStdin) (Cmd *self, const char* stdin, const int stdin_len);
    int stdin_fd;
    FILE* stdin;
};

#define TI_DONT_INTERCEPT   NULL
#define TI_DONT_LISTEN      (void*)1

#define SEND(txt) write(self->stdin_fd, txt, strlen(txt))

void encapsule_cmd(Cmd *command);

#endif
