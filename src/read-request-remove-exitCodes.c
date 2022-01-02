#include <read-request.h>

void is_read_error2(int val) {
    if (val == -1) {
        perror("Reading error");
        exit(EXIT_FAILURE);
    }
}

uint64_t read_taskID(int fd){
    // A supprimer ensuite
    uint16_t op;
    is_read_error2(read(fd, &op, sizeof(uint16_t)));
    uint64_t taskid;
    is_read_error2(read(fd, &taskid, sizeof(uint64_t)));
    taskid = be64toh(taskid);
    return taskid;
}



