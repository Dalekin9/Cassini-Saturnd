#include "../include/saturnd.h"

void create_opt() {
    int fd = open("saturnd-request-pipe", O_RDONLY);
    saturnd_read_reply_c(fd);
    close(fd);
    fd = open("saturnd-request-pipe", O_RDONLY);
    close(fd);
}