#include "write-reply.h"

//write the answer in the reply pipe
void saturnd_print_reply_c (uint64_t id){
    char *pipes_directory = write_default_pipes_directory();
    char *reply_pipe_name = get_pipe_name(pipes_directory, "saturnd-reply-pipe");
    int fd = open(reply_pipe_name, O_WRONLY | O_NONBLOCK);

    int length = sizeof(uint16_t) + sizeof(uint64_t);
    BYTE buff[length];
    uint16_t op = (SERVER_REPLY_OK);
    memcpy(buff, &op, sizeof(op));
    uint64_t id2 = htobe64(id);
    memcpy(buff+2, &id2, sizeof(id2));

    // write the request to the pipe
    write(fd, buff, length);
    close(fd);
    free(pipes_directory);
    free(reply_pipe_name);
}