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

task **parse_tasks(int fd, uint32_t nbTasks) {
    task **tasks = malloc(nbTasks * sizeof(task));
    is_malloc_error(tasks);

    uint32_t minutes;
    uint16_t hours;
    uint8_t daysofweek;
    uint64_t taskid;
    uint32_t argc;
    uint32_t strlength;

    // read each task
    for(long i = 0; i < nbTasks; i++){

        // malloc the task
        tasks[i] = malloc(sizeof(task));
        is_malloc_error(tasks[i]);

        // read the id of the task
        is_read_error(read(fd, &taskid, sizeof(uint32_t)));
        tasks[i]->taskid = be64toh(taskid);

        // TODO : refactor into smaller function that reads the timing
        // read the timing of the task
        is_read_error(read(fd, &minutes, sizeof(uint32_t)));
        is_read_error(read(fd, &hours, sizeof(uint16_t)));
        is_read_error(read(fd, &daysofweek, sizeof(uint8_t)));
        tasks[i]->t = malloc(sizeof(struct timing));
        is_malloc_error(tasks[i]->t);
        tasks[i]->t->minutes = be32toh(minutes);
        tasks[i]->t->hours = be16toh(hours);
        tasks[i]->t->daysofweek = daysofweek; // no need to invert bytes, there is only 1

        // read the command and its args
        tasks[i]->command = malloc(sizeof(commandline));
        is_malloc_error(tasks[i]->command);

        // read the number of args
        is_read_error(read(fd, &argc, sizeof(uint32_t)));
        tasks[i]->command->argc = be32toh(argc);

        // TODO : refactor into a smaller function that read the argv array
        // malloc the args array
        uint32_t argc = tasks[i]->command->argc; // easier to write
        tasks[i]->command->argv = malloc(argc * sizeof(string));
        is_malloc_error(tasks[i]->command->argv);
        string** argv = tasks[i]->command->argv; // easier to write
        // read the args
        for(uint32_t j = 0; j < argc; j++){
            // read the length of the arg
            is_read_error(read(fd, &strlength, sizeof(uint32_t)));
            argv[j]->length = be32toh(strlength);

            // malloc the BYTE array for the arg
            argv[j]->s = malloc(argv[j]->length * sizeof(BYTE));
            is_malloc_error(argv[j]->s);

            // read the arg
            is_read_error(read(fd, argv[j]->s, argv[j]->length));
        }
    }
    return tasks;
}

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

    // parse the list of tasks and print them
    if (nbTasks != 0) {
        task **tasks = parse_tasks(fd, nbTasks);
        print_reply_l(nbTasks, tasks);
    }  // If there are no tasks, there is nothing to print

}