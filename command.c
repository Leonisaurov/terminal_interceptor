#include "command.h"

#include <sys/ioctl.h>
#include <pty.h>

#include <poll.h>
#include <wait.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>

#include "overlay.h"

void encapsule_cmd(struct termios term, char** cmd) {
    struct winsize win;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &win);

    int master_fd, slave_fd;

    struct termios slave_t = term;
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

        execvp(*cmd, cmd);
        perror("execvp");
        exit(EXIT_FAILURE);
        return;
    }

    close(slave_fd);

    char buffer[256];
    int timeout = 1000;
    struct pollfd fd[2];

    fd[0].fd = master_fd;
    fd[0].events = POLLIN;

    fd[1].fd = STDIN_FILENO;
    fd[1].events = POLLIN;

    int status;

    bool esc_code       = false;
    bool mark_mode      = false;
    bool escape_mode    = false;

    open_overlay(win);
    int index_mark  = -1;
    int slice_arg   = -1;
    int x_arg       = -1;
    int y_arg       = -1;

    int ret;
    do {
        if (!esc_code && !mark_mode && !escape_mode) {
            print_overlay();
        }

        ret = poll(fd, 2, timeout);
        if (ret == -1) break;
        if (ret == 0) continue;

        if (fd[0].revents & POLLIN) {
            int n = read(master_fd, buffer, 255);
            if (n < 1) continue; 
            buffer[n] = '\0';

            char *c = buffer;
            while (*c != '\0') {
                switch (*c) {
                    case 27:
                        esc_code = true;
                        c++;
                        continue;
                    case '[': {
                                  if (!esc_code) break;
                                  escape_mode = true;
                                  esc_code = false;
                                  char c = 27;
                                  write(STDOUT_FILENO, &c, 1);
                                  break;
                              }
                    case '{':
                        if (!esc_code) break;
                        mark_mode = true;
                        esc_code = false;
                        slice_arg = 0;
                        c++;
                        continue;
                    case '}':
                        if (!esc_code) break;
                        mark_mode = false;
                        esc_code = false;
                        index_mark = -1;
                        break;
                    default:
                        if (index_mark == -1 && mark_mode && ((*c >= '0' && *c <= '9') || *c == ';')) {
                            if (x_arg == -1) {
                                if (*c == ';') {
                                    x_arg = 0;
                                    c++;
                                    continue;
                                }
                                slice_arg *= 10;
                                slice_arg += *c - '0';
                                c++;
                                continue;
                            } else if(y_arg == -1) {
                                if (*c == ';') {
                                    y_arg = 0;
                                    c++;
                                    continue;
                                }
                                x_arg *= 10;
                                x_arg += *c - '0';
                                c++;
                                continue;
                            } else {
                                if (*c == ';') {
                                    index_mark = add_mark(x_arg, y_arg, slice_arg);
                                    slice_arg   = -1;
                                    x_arg       = -1;
                                    y_arg       = -1;
                                    c++;
                                    continue;
                                }
                                y_arg *= 10;
                                y_arg += *c - '0';
                                c++;
                                continue;
                           }
                        }
                        if (index_mark != -1) {
                            mark_stream_char(index_mark, *c);
                            break;
                        }
                        if (escape_mode && *c >= 0x40 && *c <= 0x7E)
                            escape_mode = false;
                        if (esc_code) {
                            char c = 27;
                            write(STDOUT_FILENO, &c, 1);
                            esc_code = false;
                        }
                        break;
                }
                if (mark_mode) {
                } else
                    write(STDOUT_FILENO, c, 1);
                c++;
            }
        }

        if (fd[1].revents & POLLIN) {
            int n = read(STDIN_FILENO, buffer, 255);
            if (n < 1) continue;
            write(master_fd, buffer, n);
        }
    } while((waitpid(pid, &status, WNOHANG)) == 0);

    close_overlay();
    close(master_fd);
}
