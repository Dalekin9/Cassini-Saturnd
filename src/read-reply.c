#include "read-reply.h"

/* Stops the program when there's a reading error */
void is_read_error(long read_return){
    if(read_return < 0){
        perror("Reading error.\n");
        exit(EXIT_FAILURE);
    }
}

void is_malloc_error2(void* p) {
    if (p == NULL) {
        perror("Malloc failure\n");
        exit(EXIT_FAILURE);
    }
}

/* Reads from the reply pipe when a response to -l.
   Returns a pointer to an array of tasks.*/
task **read_list_task(int pipe_fd, long taskNb){
    // malloc the array of tasks
    task **tasks = malloc(taskNb * sizeof(task));
    is_malloc_error2(tasks);

    uint32_t minutes;
    uint16_t hours;
    uint8_t daysofweek;
    uint64_t taskid;
    uint32_t argc;
    uint32_t strlength;

    // read each task
    for(long i = 0; i < taskNb; i++){

        // malloc the task
        tasks[i] = malloc(sizeof(task));
        is_malloc_error2(tasks[i]);

        // read the id of the task
        is_read_error(read(pipe_fd, &taskid, sizeof(uint32_t)));
        tasks[i]->taskid = be64toh(taskid);

        // read the timing of the task
        is_read_error(read(pipe_fd, &minutes, sizeof(uint32_t)));
        is_read_error(read(pipe_fd, &hours, sizeof(uint16_t)));
        is_read_error(read(pipe_fd, &daysofweek, sizeof(uint8_t)));
        tasks[i]->t = malloc(sizeof(struct timing));
        is_malloc_error2(tasks[i]->t);
        tasks[i]->t->minutes = be32toh(minutes);
        tasks[i]->t->hours = be16toh(hours);
        tasks[i]->t->daysofweek = daysofweek; // no need to invert bytes, there is only 1

        // read the command and its args
        tasks[i]->command = malloc(sizeof(commandline));
        is_malloc_error2(tasks[i]->command);

        // read the number of args
        is_read_error(read(pipe_fd, &argc, sizeof(uint32_t)));
        tasks[i]->command->argc = be32toh(argc);

        // malloc the args array
        uint32_t argc = tasks[i]->command->argc; // easier to write
        tasks[i]->command->argv = malloc(argc * sizeof(string));
        is_malloc_error2(tasks[i]->command->argv);
        string** argv = tasks[i]->command->argv; // easier to write

        // read the args
        for(uint32_t j = 0; j < argc; j++){
            // read the length of the arg
            is_read_error(read(pipe_fd, &strlength, sizeof(uint32_t)));
            argv[j]->length = be32toh(strlength);

            // malloc the BYTE array for the arg
            argv[j]->s = malloc(argv[j]->length * sizeof(BYTE));
            is_malloc_error2(argv[j]->s);

            // read the arg
            is_read_error(read(pipe_fd, argv[j]->s, argv[j]->length));
        }
    }
    return tasks;
}


/* Gathers the list of past runs from the reply pipe and returns it */
run **read_times_exitcode(int pipe_fd, long runNb){
    run **runs = malloc(sizeof(runNb * sizeof(run)));
    is_malloc_error2(runs);

    int64_t time;
    uint16_t exitcode;

    for (int i = 0; i < runNb; i++) {
        is_read_error(read(pipe_fd, &time, sizeof(time)));
        time = htobe64(time);

        is_read_error(read(pipe_fd, &exitcode, sizeof(exitcode)));
        exitcode = htobe16(exitcode);

        runs[i] = malloc(sizeof(run));
        is_malloc_error2(runs);
        runs[i]->time = time;
        runs[i]->exitcode = exitcode;
    }
    return runs;
}


/* Reads from the reply pipe and prints the result. */
void read_answer(int pipe_fd, uint16_t operation) {
    uint16_t reptype;
    uint64_t id;
    uint32_t taskNb;
    uint32_t runNb;

// Uncomment to test writing exactly the reply into the reply tube
//    BYTE buf[6] = {0x4f, 0x4b, 0x00, 0x00, 0x00, 0x00};
//    int out_fd = open("./run/pipes/saturnd-reply-pipe", O_WRONLY);
//    write(out_fd, buf, 6);

    // read the reply code
    is_read_error(read(pipe_fd, &reptype, sizeof(reptype)));
    reptype = htobe16(reptype);

    // depending on the operation, read the rest of the reply
    switch (operation) {
        case CLIENT_REQUEST_LIST_TASKS:
            // read the task number
            is_read_error(read(pipe_fd, &taskNb, sizeof(uint32_t)));
            taskNb = htobe32(taskNb);

            if (taskNb > 0) {
              // read the rest of the reply
              task **tasks = read_list_task(pipe_fd, taskNb);
              print_reply(operation, reptype, 0, taskNb, 0, tasks, 0, NULL);
            }
            break;
        case CLIENT_REQUEST_CREATE_TASK:
            is_read_error(read(pipe_fd, &id, sizeof(id)));
            id = htobe64(id);
            print_reply(operation, reptype, 0, 0, id, 0, 0, NULL);
            break;
        case CLIENT_REQUEST_TERMINATE:
            print_reply(operation, reptype, 0, 0, 0, 0, 0, NULL);
            break;
        case CLIENT_REQUEST_REMOVE_TASK:
            if (reptype == SERVER_REPLY_OK) {
                print_reply(operation, reptype, 0, 0, 0, 0, 0, NULL);
            } else {
                uint16_t err;
                is_read_error(read(pipe_fd, &err, sizeof(uint16_t)));
                err = htobe16(err);
                print_reply(operation, reptype, err, 0, 0, 0, 0, NULL);
            }
            break;
        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
            if (reptype == SERVER_REPLY_OK) {
                is_read_error(read(pipe_fd, &runNb, sizeof(runNb)));
                runNb = htobe32(runNb);
                run **runs = read_times_exitcode(pipe_fd, runNb);
                print_reply(operation, reptype, 0, runNb, 0, 0, runs, NULL);
            } else {
                uint16_t err;
                is_read_error(read(pipe_fd, &err, sizeof(err)));
                err = htobe16(err);
                print_reply(operation, reptype, err, 0, 0, 0, 0, NULL);
            }
            break;
        case CLIENT_REQUEST_GET_STDOUT:
        case CLIENT_REQUEST_GET_STDERR:
            if (reptype == SERVER_REPLY_OK) {
                // read the length of the output
                uint32_t length;
                is_read_error(read(pipe_fd, &length, sizeof(uint32_t)));
                length = htobe64(length);
                // read the output itself
                BYTE *str = malloc(length * sizeof(BYTE));
                is_read_error(read(pipe_fd, str, length));

                // make the string
                string *output_str = malloc(sizeof(string));
                is_malloc_error2(output_str);
                output_str->length = length;
                output_str->s = str;

                print_reply(operation, reptype, 0, 0, 0, 0, 0, output_str);
            } else {
                uint16_t err;
                is_read_error(read(pipe_fd, &err, sizeof(uint16_t)));
                err = htobe16(err);
                print_reply(operation, reptype, err, 0, 0, 0, 0, NULL);
            }
            break;
        default:
            perror("?");
            exit(1);
    }
    close(pipe_fd);
}