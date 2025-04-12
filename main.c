#include <asm-generic/ioctls.h>
#include <pty.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <termios.h>
#include <fcntl.h>

#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <poll.h>

struct termios raw_t, original_t;

void encapsule_cmd(const char* cmd) {
    struct winsize win;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &win);

    int master_fd, slave_fd;

    struct termios slave_t = original_t;
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

        execlp(cmd, (char*) NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
        return;
    }

    close(slave_fd);

    char buffer[256];
    int timeout = 10000;
    struct pollfd fd[2];

    fd[0].fd = master_fd;
    fd[0].events = POLLIN;

    fd[1].fd = STDIN_FILENO;
    fd[1].events = POLLIN;

    int status;
    do {
        int ret = poll(fd, 2, timeout);
        if (ret == -1) break;
        if (ret == 0) continue;

        if (fd[0].revents & POLLIN) {
            int n = read(master_fd, buffer, 255);
            if (n < 1) continue; 
            buffer[n] = '\0';

            write(STDOUT_FILENO, buffer, n);
        }

        if (fd[1].revents & POLLIN) {
            int n = read(STDIN_FILENO, buffer, 255);
            if (n < 1) continue;
            write(master_fd, buffer, n);
        }
    } while((waitpid(pid, &status, WNOHANG)) == 0);

    close(master_fd);
}

int main() {
    tcgetattr(STDIN_FILENO, &original_t);

    raw_t = original_t;
    raw_t.c_lflag &= ~(ICANON | ECHO | ISIG);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw_t);

    int original_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, original_flags | O_NONBLOCK);

    setvbuf(stdout, NULL, _IONBF, 0);

    encapsule_cmd("bash");

    setvbuf(stdout, NULL, _IOLBF, BUFSIZ);

    fcntl(STDIN_FILENO, F_SETFL, original_flags);
    tcsetattr(STDIN_FILENO, TCSANOW, &original_t);
    return 0;
}
