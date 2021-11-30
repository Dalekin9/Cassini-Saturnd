#include "cassini.h" // TODO : clean up the imports
#include "read-reply.h"
#include "print-reply.h"

void is_malloc_error(void *p) {
    if (p == NULL) {
        perror("Malloc error");
        exit(EXIT_FAILURE);
    }
}

void is_read_error(int ret) {
    if (ret == -1) {
        perror("Reading error");
        exit(EXIT_FAILURE);
    }
}

struct timing *read_timing(int fd) {
    uint64_t minutes; uint32_t hours; uint8_t daysofweek;

    struct timing *t = malloc(sizeof(struct timing));
    is_malloc_error(t);

    is_read_error(read(fd, &minutes, sizeof(uint64_t)));
    is_read_error(read(fd, &hours, sizeof(uint32_t)));
    is_read_error(read(fd, &daysofweek, sizeof(uint8_t)));

    t->minutes = be64toh(minutes);
    t->hours = be32toh(hours);
    t->daysofweek = daysofweek; // no need to invert bytes, there is only 1

    return t;
}

string *read_string(int fd) {
    string *str = malloc(sizeof(string));
    is_malloc_error(str);

    // read the length of the arg
    uint32_t strlength;
    is_read_error(read(fd, &strlength, sizeof(uint32_t)));
    str->length = be32toh(strlength);

    // malloc the BYTE array for the arg
    str->s = malloc(str->length * sizeof(BYTE));
    is_malloc_error(str->s);

    // read the arg
    is_read_error(read(fd, str->s, str->length));
}

string **read_args(int fd, uint32_t argc) {
    string **argv = malloc(argc * sizeof(string));
    is_malloc_error(argv);

    for(uint32_t j = 0; j < argc; j++){
        argv[j] = read_string(fd);
    }

    return argv;
}

task *parse_one_task(int fd) {
    // malloc the task
    task *t = malloc(sizeof(task));
    is_malloc_error(t);

    // read the id of the task
    uint64_t taskid;
    is_read_error(read(fd, &taskid, sizeof(uint64_t)));
    t->taskid = be64toh(taskid);

    // read the timing of the task
    t->t = read_timing(fd);

    // read the command and its args
    t->command = malloc(sizeof(commandline));
    is_malloc_error(t->command);

    // read the number of args
    uint32_t argc;
    is_read_error(read(fd, &argc, sizeof(uint32_t)));
    t->command->argc = be32toh(argc);

    if (argc != 0) { // read the args
        t->command->argv = read_args(fd, t->command->argc);
    }

    return t;
}

task **parse_tasks(int fd, uint16_t nbTasks) {
    task **tasks = malloc(nbTasks * sizeof(task));
    is_malloc_error(tasks);

    for(uint16_t i = 0; i < nbTasks; i++){
        tasks[i] = parse_one_task(fd);
    }
    return tasks;
}

void read_reply_l(int fd) {
    // read the number of tasks
    uint32_t nbTasks;
    read(fd, &nbTasks, sizeof(uint32_t));
    nbTasks = be32toh(nbTasks);

    // parse the list of tasks and print them
    if (nbTasks != 0) {
        task **tasks = parse_tasks(fd, nbTasks);
        print_reply_l(nbTasks, tasks);
    }  // If there are no tasks, there is nothing to print

}

void read_reply_c(int fd) {
    uint64_t taskid;
    read(fd, &taskid, sizeof(uint64_t));
    taskid = be64toh(taskid);
    print_reply_c(taskid);
}

void read_reply_error(int fd) {
    // get the error code
    uint16_t errcode;
    read(fd, &errcode, sizeof(uint16_t));
    errcode = htobe16(errcode);

    print_error(errcode);
}

void read_reply_x(int fd, uint16_t repcode) {
    uint32_t nbRuns;
    read(fd, &nbRuns, sizeof(uint32_t));
    nbRuns = htobe32(nbRuns);

    if (repcode == SERVER_REPLY_ERROR) {
        read_reply_error(fd);
    } else {
        run **runs = malloc(sizeof(nbRuns * sizeof(run)));
        is_malloc_error(runs);

        int64_t time;
        uint16_t exitcode;

        for (uint32_t i = 0; i < nbRuns; i++) {
            runs[i] = malloc(sizeof(run));
            is_malloc_error(runs);

            read(fd, &time, sizeof(time));
            time = htobe64(time);
            runs[i]->time = time;

            read(fd, &exitcode, sizeof(exitcode));
            exitcode = htobe16(exitcode);
            runs[i]->exitcode = exitcode;
        }

        print_times_and_exit_codes(nbRuns, runs);
    }
}

void read_reply_std(int fd, uint16_t repcode) {
    if (repcode == SERVER_REPLY_ERROR) {
        read_reply_error(fd); // printing call is inside read_reply_error
    } else {
        string *output = read_string(fd);
        print_output(output);
    }
}


void read_reply(int fd, uint16_t operation) {
    // read the repcode
    uint16_t repcode;
    read(fd, &repcode, sizeof(uint16_t));
    repcode = htobe16(repcode);

    switch (operation) {
        case CLIENT_REQUEST_LIST_TASKS:
            read_reply_l(fd); // print statement is inside the read_reply_l function
            break;
        case CLIENT_REQUEST_CREATE_TASK:
            read_reply_c(fd);
            break;
        case CLIENT_REQUEST_REMOVE_TASK:
            read_reply_error(fd);
            break;
        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
            read_reply_x(fd, repcode);
            break;
        case CLIENT_REQUEST_GET_STDERR:
        case CLIENT_REQUEST_GET_STDOUT:
            read_reply_std(fd, repcode);
        case CLIENT_REQUEST_TERMINATE: break; // nothing to do

    }
}