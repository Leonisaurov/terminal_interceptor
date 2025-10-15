#include "command.h"

#include <sys/ioctl.h>
#include <pty.h>

#include <poll.h>
#include <wait.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

void encapsule_cmd(Cmd *cmd) {
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
        dup2(slave_fd, STDOUT_FILENO);
        dup2(slave_fd, STDIN_FILENO);
        dup2(slave_fd, STDERR_FILENO);

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

    char buffer[(STDIN_BFSIZE > STDOUT_BFSIZE?STDIN_BFSIZE:STDOUT_BFSIZE) + 1];
    int timeout = 10000;
    struct pollfd fd[2] = {0};

    if (cmd->processStdout != TI_DONT_LISTEN) {
        fd[0].fd = master_fd;
        fd[0].events = POLLIN;
    }

    if (cmd->processStdin != TI_DONT_LISTEN) {
        fd[1].fd = STDIN_FILENO;
        fd[1].events = POLLIN;
    }

    int status;
    do {
        int ret = poll(fd, 2, timeout);
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
}
