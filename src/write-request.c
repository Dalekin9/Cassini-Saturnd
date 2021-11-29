#include "cassini.h"        // TODO : clean up the imports
#include "write-request.h"

void write_request_c(int fd, uint16_t operation, commandline *command, struct timing *t) {
    // compute the total length of the request
    int length = sizeof(operation)
                + sizeof(uint64_t) + sizeof(uint32_t) + sizeof(uint8_t)  // timing fields
                + sizeof(uint32_t); // argc
    for (uint32_t i = 0; i < command->argc; ++i) {
        // add the length of the string + 32 (storing the length)
        length += sizeof(uint32_t) + command->argv[i]->length;
    }

    // create the request
    int current = 0;
    BYTE buf[length];
    operation = htobe16(operation);
    memcpy(buf, &operation, sizeof(uint16_t));
    current += sizeof(uint16_t);

    // copy the timing
    t->minutes = htobe64(t->minutes);
    memcpy(buf+current, &t->minutes, sizeof(uint64_t));
    current += sizeof(uint64_t);
    t->hours = htobe32(t->hours);
    memcpy(buf+current, &t->hours, sizeof(uint32_t));
    current += sizeof(uint32_t);
    memcpy(buf+current, &t->daysofweek, sizeof(uint8_t)); // no need to convert endian for days : there is only 1 byte
    current += sizeof(uint8_t);

    uint32_t argc_tmp = htobe32(command->argc);
    memcpy(buf+current, &argc_tmp, sizeof(uint32_t));
    current += sizeof(uint32_t);

    // copy command->argv
    for (int i = 0; i < command->argc; i++) {
        string *str = command->argv[i];
        uint32_t length_tmp = htobe32(str->length);
        memcpy(buf+current, &length_tmp, sizeof(uint32_t));
        current += sizeof(uint32_t);
        memcpy(buf+current, str->s, str->length);
        current += str->length;
    }

    // write the request
    write(fd, buf, length);
}

void write_request_taskid(int fd, uint16_t operation, uint64_t taskid) {
    size_t length = sizeof(uint16_t) + sizeof(uint64_t);
    // copy the info into the BYTE buffer
    BYTE buf[length];
    operation = htobe16(operation);
    memcpy(buf, &operation, sizeof(uint16_t));
    taskid = htobe64(taskid);
    memcpy(buf+sizeof(uint16_t), &taskid, sizeof(uint64_t));
    // write the request to the pipe
    write(fd, buf, length);
}

void write_request_operation_code(int fd, uint16_t operation) {
    operation = htobe16(operation);
    write(fd, &operation, sizeof(operation));
}

void write_request(int fd, uint16_t operation, commandline *command, struct timing *t, uint64_t taskid) {
    switch (operation) {
        case CLIENT_REQUEST_CREATE_TASK:
            write_request_c(fd, operation, command, t);
            free(t);
            free(command->argv);
            free(command);
            break;
        // below are the requests with only the operation code
        case CLIENT_REQUEST_LIST_TASKS:
        case CLIENT_REQUEST_TERMINATE:
            write_request_operation_code(fd, operation);
            break;
        // below are all the requests with only the operation code and the taskid
        case CLIENT_REQUEST_REMOVE_TASK:
        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
        case CLIENT_REQUEST_GET_STDOUT:
        case CLIENT_REQUEST_GET_STDERR:
            write_request_taskid(fd, operation, taskid);
            break;
    }
}