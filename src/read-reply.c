#include "cassini.h" // TODO : clean up the imports
#include "read-reply.h"

void read_reply_l(int fd) {
    // read the repcode
    uint16_t repcode;
    read(fd, &repcode, sizeof(uint16_t));

    repcode = be16toh(repcode);
    if (repcode != SERVER_REPLY_OK) {
        perror("Wrong error code ");
        exit(EXIT_FAILURE);
    }

    // read the number of tasks
    uint32_t nbTasks;
    read(fd, &nbTasks, sizeof(uint32_t));

    nbTasks = be32toh(nbTasks);
    if (nbTasks != 0) {
        perror("Should be 0");
        exit(EXIT_FAILURE);
    }
}