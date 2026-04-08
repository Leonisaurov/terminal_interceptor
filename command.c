#include "command.h"

#include <assert.h>
#include <sys/ioctl.h>
#include <pty.h>

#include <poll.h>
#include <wait.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "config.h"
#include "terminal.h"

#include <stdio.h>

void encapsule_cmd(Cmd *cmd) {
    set_raw_terminal(cmd->term_snap, STDIN_FILENO);
    int flags;
    set_nonblockig_input(STDIN_FILENO, &flags);
    unset_buffering(stdout);

    struct winsize win;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &win);

    int master_fd, slave_fd;

    struct termios slave_t = cmd->term_snap;
    if (openpty(&master_fd, &slave_fd, NULL, &slave_t, &win) == -1) {
        perror("openpty");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
        return;
    }

    if (pid == 0) {
        assert(dup2(slave_fd, STDOUT_FILENO) == STDOUT_FILENO);
        assert(dup2(slave_fd, STDIN_FILENO) == STDIN_FILENO);
        assert(dup2(slave_fd, STDERR_FILENO) == STDERR_FILENO);

        if(setsid() == -1) {
            perror("setsid");
            exit(EXIT_FAILURE);
        }

        if (ioctl(slave_fd, TIOCSCTTY, slave_fd) == -1) {
            perror("ioctl TIOCSCTTY");
        }

        close(master_fd);
        close(slave_fd);

        execvp(*cmd->command, (char**)cmd->command);
        perror("execvp");
        exit(EXIT_FAILURE);
        return;
    }

    close(slave_fd);
    cmd->stdin = master_fd;
    LOG("PID: "); LOGN(pid); NL;

    char buffer[(STDIN_BFSIZE > STDOUT_BFSIZE?STDIN_BFSIZE:STDOUT_BFSIZE) + 1];
    int timeout = 10000;
    unsigned short fd_size = 0;
    struct pollfd fd[2];

    if (cmd->processStdout != TI_DONT_LISTEN) {
        fd[0].fd = master_fd;
        fd[0].events = POLLIN;
        fd_size = 1;
    }

    if (cmd->processStdin != TI_DONT_LISTEN) {
        fd[fd_size].fd = STDIN_FILENO;
        fd[fd_size].events = POLLIN;
        fd_size++;
    }

    int status;
    do {
        int ret = poll(fd, fd_size, timeout);
        if (ret == -1) break;
        if (ret == 0) continue;

        if (cmd->onRun != NULL)
            cmd->onRun(cmd);

        if (fd[0].revents & POLLIN) {
            int n = read(master_fd, buffer, STDOUT_BFSIZE);
            if (n < 1) continue; 
            buffer[n] = '\0';

            if(cmd->processStdout == TI_DONT_INTERCEPT) {
                write(STDOUT_FILENO, buffer, n);
            } else {
                int ret = cmd->processStdout(cmd, buffer, n);
                switch (ret) {
                    case 0: break;
                    case 1: write(STDOUT_FILENO, buffer, n); break;
                    // TODO: ERROR
                    default: break;
                }
            }
        }

        if (fd[1].revents & POLLIN) {
            int n = read(STDIN_FILENO, buffer, STDIN_BFSIZE);
            if (n < 1) continue;
            buffer[n] = '\0';

            if(cmd->processStdin == TI_DONT_INTERCEPT) {
                write(master_fd, buffer, n);
            } else {
                int ret = cmd->processStdin(cmd, buffer, n);
                switch (ret) {
                    case 0: break;
                    case 1: write(master_fd, buffer, n); break;
                    // TODO: ERROR
                    default: break;
                }
            }
        }
    } while((waitpid(pid, &status, WNOHANG)) == 0);

    close(master_fd);
    set_buffering(stdout);
    set_normal_input(STDIN_FILENO, flags);
    set_normal_terminal(&cmd->term_snap, STDIN_FILENO);
}
