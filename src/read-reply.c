#include "read-reply.h"

/* Stops the program when there's a reading error */
void is_read_error(long read_return){
    if(read_return < 0){
        perror("Reading error.\n");
        exit(EXIT_FAILURE);
    }
}

unsigned long commandline_size(uint32_t argc){
    return argc * (4 + sizeof(BYTE*));
}

/* Reads from the reply pipe when a response to -l ; returns a task list */
task **read_list_task(int pipedes, long taskNb){
    task **tasks = malloc(taskNb * (8 + (8 + 4 + 1) )) ;
    if(tasks == NULL){
        perror("Malloc failure\n");
        exit(EXIT_FAILURE);
    }
    for(long i = 0; i < taskNb; i++){
        task a_task;
        long id;
        uint64_t minutes;
        uint32_t hours;
        uint8_t daysofweek;

        is_read_error(read(pipedes,&id, 8));
        a_task.taskid = id;

        is_read_error(read(pipedes,&minutes, 8));
        is_read_error(read(pipedes,&hours, 4));
        is_read_error(read(pipedes,&daysofweek, 1));
        struct timing time = {.minutes = minutes, .hours = hours, .daysofweek = daysofweek};
        a_task.t = &time;

        commandline cmd;
        uint32_t argc;
        is_read_error(read(pipedes, &argc, 4));
        cmd.argc = argc;


        string *args = malloc(commandline_size(argc));
	if(args == NULL){
        	perror("Malloc failure\n");
        	exit(EXIT_FAILURE);
    	}
        for(long j = 0; j < argc; j++){
            uint32_t length;
            BYTE *byte;
            is_read_error(read(pipedes, &length, 4));
            is_read_error( read(pipedes, &byte, length));

            string str = {.length = length, .s = byte};
            args[j] = str;
        }
        cmd.argv = &args;
        a_task.command = cmd;
        tasks[i] = &a_task;
    }
    return tasks;
}

/* Reads and returns the ID of the task that has been created */
uint64_t read_create(int pipedes){
    uint64_t taskID;
    is_read_error(read(pipedes, &taskID, 8));
    return taskID;
}

/* Gathers the list of past runs from the reply pipe and returns it */
run **read_times_exitcode(int pipedes, long runNb){
    run **runs = malloc(sizeof((64 + 16) * runNb));     // 64 for a run's time and 16 for it's exitcode
    if (runs == NULL) {
        perror("Malloc failure\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < runNb; ++i) {
        int64_t time;
        uint16_t exitcode;
        is_read_error(read(pipedes, &time, 8));
        is_read_error(read(pipedes, &exitcode, 2));
        run a_run = {.time = time, .exitcode = exitcode};
        *runs[i] = a_run;
    }
    return runs;
}


/* Reads from the reply pipe and prints the result depending on the request */
void read_answer(int pipedes,uint16_t ope ) {
    uint16_t rep;
    uint64_t id;
    long taskNb;
    long runNb;
    string a = {0, 0};
    is_read_error(read(pipedes, &rep, 2));
    switch (ope) {
        case CLIENT_REQUEST_LIST_TASKS:
            is_read_error(read(pipedes, &taskNb, 4));
            task **tasks = read_list_task(pipedes, taskNb);
            print_response(ope, rep, 0, taskNb, 0, tasks, 0, a);
            break;
        case CLIENT_REQUEST_CREATE_TASK:
            id = read_create(pipedes);
            print_response(ope, rep, 0, 0, id, 0, 0, a);
            break;
        case CLIENT_REQUEST_TERMINATE:
            print_response(ope, rep, 0, 0, 0, 0, 0, a);
            break;
        case CLIENT_REQUEST_REMOVE_TASK:
            if (rep == SERVER_REPLY_OK) {
                print_response(ope, rep, 0, 0, 0, 0, 0, a);
                break;
            } else {
                uint16_t err;
                is_read_error(read(pipedes, &err, 2)); //ERRCODE
                print_response(ope, rep, err, 0, 0, 0, 0, a);
                break;
            }
        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
            if (rep == SERVER_REPLY_OK) {
                is_read_error(read(pipedes, &runNb, 4));
                run **runs = read_times_exitcode(pipedes, runNb);
                print_response(ope, rep, 0, runNb, 0, 0, runs, a);
                break;
            } else {
                uint16_t err;
                is_read_error(read(pipedes, &err, 2)); //ERRCODE
                print_response(ope, rep, err, 0, 0, 0, 0, a);
                break;
            }
        case CLIENT_REQUEST_GET_STDOUT:
        case CLIENT_REQUEST_GET_STDERR:
            if (rep == SERVER_REPLY_OK) {
                uint32_t length;
                BYTE str;
                is_read_error(read(pipedes, &length, 4));
                is_read_error(read(pipedes, &str, length));
                print_response(ope, rep, 0, 0, 0, 0, 0, str);
                break;
            } else {
                uint16_t err;
                is_read_error(read(pipedes, &err, 2)); //ERRCODE
                print_response(ope, rep, err, 0, 0, 0, 0, a);
                break;
            }
        default:
            perror("?");
            exit(1);
    }
}