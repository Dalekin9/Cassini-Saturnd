#include "read-request.h"

void saturnd_read_reply_c (int fd, int fd_rep){
    //lire le timing
    struct timing *t = read_timing(fd);

    //lire les arguments
    uint32_t argc;
    is_read_error(read(fd, &argc, sizeof(uint32_t)));
    uint32_t val = be32toh(argc);
    string **st = read_args(fd, val);
    close_pipe(fd);
    create_folder_task(t,val,st, fd_rep);
}