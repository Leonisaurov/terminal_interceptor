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
    int stdin;
};

#define TI_DONT_INTERCEPT   NULL
#define TI_DONT_LISTEN      (void*)1

#define SEND_Input(txt) write(self->stdin, txt, strlen(txt))
#define SEND_char_In(c) write(self->stdin, &c, 1)

#define SEND_Output(txt) write(STDOUT_FILENO, txt, strlen(txt))
#define SEND_char_Out(c) write(STDOUT_FILENO, &c, 1)

void encapsule_cmd(Cmd *command);

#endif
