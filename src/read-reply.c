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

    // read each task
    for(long i = 0; i < taskNb; i++){
        // malloc the task
        tasks[i] = malloc(sizeof(task));
        is_malloc_error2(tasks[i]);

        // read the id of the task
        is_read_error(read(pipe_fd, &(tasks[i]->taskid), sizeof(uint32_t)));

        // read the timing of the task
        is_read_error(read(pipe_fd, &minutes, sizeof(uint32_t)));
        is_read_error(read(pipe_fd, &hours, sizeof(uint16_t)));
        is_read_error(read(pipe_fd, &daysofweek, sizeof(uint8_t)));
        tasks[i]->t = malloc(sizeof(struct timing));
        is_malloc_error2(tasks[i]->t);
        tasks[i]->t->minutes = minutes;
        tasks[i]->t->hours = hours;
        tasks[i]->t->daysofweek = daysofweek;

        // read the command and its args
        tasks[i]->command = malloc(sizeof(commandline));
        is_malloc_error2(tasks[i]->command);

        // read the number of args
        is_read_error(read(pipe_fd, &(tasks[i]->command->argc), sizeof(uint32_t)));

        // malloc the args array
        uint32_t argc = tasks[i]->command->argc; // easier to write
        tasks[i]->command->argv = malloc(argc *sizeof(string));
        is_malloc_error2(tasks[i]->command->argv);
        string** argv = tasks[i]->command->argv; // easier to write

        // read the args
        for(uint32_t j = 0; j < argc; j++){
            // read the length of the arg
            is_read_error(read(pipe_fd, &(argv[j]->length), sizeof(uint32_t)));
            // malloc the BYTE array for the arg
            argv[j]->s = malloc(argv[j]->length * sizeof(BYTE));
            is_malloc_error2(argv[j]->s);
            // read the arg
            is_read_error(read(pipe_fd, argv[j]->s, argv[j]->length));
        }
    }
    return tasks;
}

/* Reads and returns the ID of the task that has been created */
uint64_t read_create(int pipe_fd){
    uint64_t taskID;
    is_read_error(read(pipe_fd, &taskID, 8));
    return taskID;
}

/* Gathers the list of past runs from the reply pipe and returns it */
run **read_times_exitcode(int pipe_fd, long runNb){
    run **runs = malloc(sizeof((64 + 16) * runNb));     // 64 for a run's time and 16 for it's exitcode
    if (runs == NULL) {
        perror("Malloc failure\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < runNb; ++i) {
        int64_t time;
        uint16_t exitcode;
        is_read_error(read(pipe_fd, &time, 8));
        is_read_error(read(pipe_fd, &exitcode, 2));
        run a_run = {.time = time, .exitcode = exitcode};
        *runs[i] = a_run;
    }
    return runs;
}


/* Reads from the reply pipe and prints the result. */
void read_answer(int pipe_fd, uint16_t operation) {
    uint16_t reptype;
    uint64_t id;
    long taskNb;
    long runNb;
    string a = {0, 0};

    // read the reply code
    is_read_error(read(pipe_fd, &reptype, sizeof(uint16_t)));

    // depending on the operation, read the rest of the reply
    switch (operation) {
        case CLIENT_REQUEST_LIST_TASKS:
            is_read_error(read(pipe_fd, &taskNb, sizeof(uint32_t))); // put the nb of tasks into taskNb
            fprintf(stdout, "READ THE ERROR CODE\n");
            task **tasks = read_list_task(pipe_fd, taskNb); // read the rest of the reply
            fprintf(stdout, "Read\n");
            print_reply(operation, reptype, 0, taskNb, 0, tasks, 0, NULL);
            break;
//        case CLIENT_REQUEST_CREATE_TASK:
//            id = read_create(pipe_fd);
//            print_reply(operation, reptype_code, 0, 0, id, 0, 0, &a);
//            break;
//        case CLIENT_REQUEST_TERMINATE:
//            print_reply(operation, reptype_code, 0, 0, 0, 0, 0, &a);
//            break;
//        case CLIENT_REQUEST_REMOVE_TASK:
//            if (reptype_code == SERVER_REPLY_OK) {
//                print_reply(operation, reptype_code, 0, 0, 0, 0, 0, &a);
//                break;
//            } else {
//                uint16_t err;
//                is_read_error(read(pipe_fd, &err, 2)); //ERRCODE
//                print_reply(operation, reptype_code, err, 0, 0, 0, 0, a);
//                break;
//            }
//        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
//            if (reptype_code == SERVER_REPLY_OK) {
//                is_read_error(read(pipe_fd, &runNb, 4));
//                run **runs = read_times_exitcode(pipe_fd, runNb);
//                print_reply(operation, reptype_code, 0, runNb, 0, 0, runs, a);
//                break;
//            } else {
//                uint16_t err;
//                is_read_error(read(pipe_fd, &err, 2)); //ERRCODE
//                print_reply(operation, reptype_code, err, 0, 0, 0, 0, a);
//                break;
//            }
//        case CLIENT_REQUEST_GET_STDOUT:
//        case CLIENT_REQUEST_GET_STDERR:
//            if (reptype_code == SERVER_REPLY_OK) {
//                uint32_t length;
//                BYTE str;
//                is_read_error(read(pipe_fd, &length, 4));
//                is_read_error(read(pipe_fd, &str, length));
//                print_reply(operation, reptype_code, 0, 0, 0, 0, 0, str);
//                break;
//            } else {
//                uint16_t err;
//                is_read_error(read(pipe_fd, &err, 2)); //ERRCODE
//                print_reply(operation, reptype_code, err, 0, 0, 0, 0, a);
//                break;
//            }
        default:
            perror("?");
            exit(1);
    }
    close(pipe_fd);
}