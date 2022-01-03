#include "read-request.h"

void read_request_c (int fd){
    // read the timing
    struct timing *t = read_timing(fd);

    // read the args of the command line
    uint32_t argc;
    is_read_error(read(fd, &argc, sizeof(uint32_t)));
    argc = be32toh(argc);
    string **argv = read_args(fd, argc);
    close_pipe(fd);
    write_reply_c(t, argc, argv);
}

/* Reads the request for stdout (if is_stdout = 1) or for stderr (if is_stdout = 0).
 * Then launches the methods to write the reply to thereply pipe.
*/
void read_request_std(int fd, int is_stdout) {
    // read the id of the task
    uint64_t taskid;
    read(fd, &taskid, sizeof(uint64_t));
    taskid = htobe64(taskid);
    // write the reply
    write_reply_std(taskid, is_stdout);
}