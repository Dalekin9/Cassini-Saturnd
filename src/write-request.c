/*
This file contains the methods to format a request into a BYTE array
and to write them into the requests tube.
*/


/*
Formats the request to create a new task and writes it into the requests pipe.
*/
int write_create_request(int pipefd, uint16_t operation, struct timing *t, commandline* command) {
    // compute the total length of the request
    int length = 2 + (8+4+1) + 4; // 16=operation; (...)=t; 32=argc
    for (int i = 0; i < command->argc; ++i) { // for each element of command,
                                     // add the length of the string + 32 (storing the length)
      length += 4 + command->argv[i]->length;
    }

    // create the request
    int current = 0;
    BYTE buf[length];
    memcpy(buf, &operation, 2);
    current += 2;
    // copy the timing
    memcpy(buf+current, &t->minutes, 8);
    current += 8;
    memcpy(buf+current, &t->hours, 4);
    current += 4;
    memcpy(buf+current, &t->daysofweek, 1);
    current += 1;
    memcpy(buf+current, &command->argc, 4);
    // copy command->argv
    for (int i = 0; i < command->argc; i++) {
        string *str = command->argv[i];
        memcpy(buf+current, &str->length, 4);
        current += 4;
        memcpy(buf+current, str->s, str->length);
        current += str->length;
    }

    return write(pipefd, buf, length);
}

/*
 * Writes a request in the request pipe.
 * See protocol.md to see which arguments correspond to which operation type.
 * pipefd is the fd of the pipe to write to
 */
void write_request(int pipefd, uint16_t operation, uint64_t taskID, struct timing *t, commandline *cmd){
    long ret;
    switch (operation) {
        case CLIENT_REQUEST_LIST_TASKS:
            ret = write(pipefd , &operation, 2);
            break;

        case CLIENT_REQUEST_CREATE_TASK:
            ret = write_create_request(pipefd, operation, t, cmd);
            break;

        case CLIENT_REQUEST_TERMINATE:
            ret = write(pipefd, &operation, 2);
            break;

        case CLIENT_REQUEST_REMOVE_TASK:
            BYTE buf[2+8];
            memcpy(buf, &operation, 2);
            memcpy(buf+8, &taskID, 8);
            ret = write(pipefd, buf, 2+8);
            break;

        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
            BYTE buf2[2+8];
            memcpy(buf2, &operation, 2);
            memcpy(buf2+8, &taskID, 8);
            ret = write(pipefd, buf2, 2+8);
            break;

        case CLIENT_REQUEST_GET_STDOUT:
            BYTE buf3[2+8];
            memcpy(buf3, &operation, 2);
            memcpy(buf3+8, &taskID, 8);
            ret = write(pipefd, buf3, 2+8);
            break;

        case CLIENT_REQUEST_GET_STDERR:
            BYTE buf4[2+8];
            memcpy(buf4, &operation, 2);
            memcpy(buf4+2, &taskID, 8);
            ret = write(pipefd, buf4, 2+8);
            break;
    }
    is_write_error(ret);
}