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

string **read_args(int fd, uint32_t argc) {
    string **argv = malloc(argc * sizeof(string));
    is_malloc_error(argv);

    uint32_t strlength;
    for(uint32_t j = 0; j < argc; j++){
        argv[j] = malloc(sizeof(string));
        is_malloc_error(argv[j]);

        // read the length of the arg
        is_read_error(read(fd, &strlength, sizeof(uint32_t)));
        argv[j]->length = be32toh(strlength);

        // malloc the BYTE array for the arg
        argv[j]->s = malloc(argv[j]->length * sizeof(BYTE));
        is_malloc_error(argv[j]->s);

        // read the arg
        is_read_error(read(fd, argv[j]->s, argv[j]->length));
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
        perror("read one task");
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


void read_reply(int fd, uint16_t operation) {
    // read the repcode
    uint16_t repcode;
    read(fd, &repcode, sizeof(uint16_t));
    repcode = htobe16(repcode);


    uint64_t taskid;
    switch (operation) {
        case CLIENT_REQUEST_LIST_TASKS:
            read_reply_l(fd); // print statement is inside the read_reply_l function
            break;
        case CLIENT_REQUEST_CREATE_TASK:
            read(fd, &taskid, sizeof(uint64_t));
            taskid = be64toh(taskid);
            print_reply_c(taskid);
            break;
    }
}