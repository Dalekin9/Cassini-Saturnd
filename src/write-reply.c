#include "write-reply.h"

//write the answer in the reply pipe
void saturnd_print_reply_c (uint64_t id, int fd){

    int length = sizeof(uint16_t) + sizeof(uint64_t);
    BYTE buff[length];
    uint16_t op = (SERVER_REPLY_OK);
    memcpy(buff, &op, sizeof(op));
    uint64_t id2 = htobe64(id);
    memcpy(buff+2, &id2, sizeof(id2));

    // write the request to the pipe
    write(fd, buff, length);
    close_pipe(fd);
}