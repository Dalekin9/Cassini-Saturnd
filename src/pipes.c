#include "pipes.h"

int open_request_pipe() {
    int fd = open("run/pipes/saturnd-request-pipe", O_RDWR);
    if (fd == -1) {
        perror("Can't open the request pipe ");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int open_reply_pipe() {
    int fd = open("run/pipes/saturnd-reply-pipe", O_RDONLY);
    if (fd == -1) {
        perror("Can't open the reply pipe ");
        exit(EXIT_FAILURE);
    }
    return fd;
}

void close_pipe(int fd) {
    int ret = close(fd);
    if (ret == -1){
        perror("Can't close the pipe ");
        exit(EXIT_FAILURE);
    }
}