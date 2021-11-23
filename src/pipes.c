#include <unistd.h>
#include <fcntl.h>
#include "cassini.h"
#include "timing-text-io.h"

typedef unsigned char BYTE;



typedef struct {
    uint32_t length;
    BYTE *s;
} string;

typedef struct {
    uint32_t argc;
    string **argv;
} commandline;

typedef struct {
    uint32_t taskid;
    struct timing *t;
    commandline command;
} task;

typedef struct {
    int64_t time;
    uint16_t exitcode;
} run;

// Stops the program when there's a reading error
void read_err(int pipedes, const void *buffer, size_t size){
    if(read(pipedes, &buffer, size) < 0){
        perror("Erreur de lecture");
        exit(1);
    }
}

// Stops the program when there's a writing error
void write_err(int pipedes, const void *buffer, size_t size){
    if(read(pipedes, &buffer, size) < 0){
        perror("Erreur d'écriture'");
        exit(1);
    }
}

/*
 * Creates a reply and a request pipe if there's no error
 * (Error if the path isn't valid or accessible)
 */
void create_pipes(){
    if(mkfifo("/tmp/<USER_NAME>/saturnd/pipes/saturnd-request-pipe",O_RDWR) < 0){
        perror("Can't create RP");
        exit(1);
    }

    if(mkfifo("/tmp/<USER_NAME>/saturnd/pipes/saturnd-reply-pipe",O_RDWR) < 0){
        perror("Can't create AP");
        exit(1);
    };
}

//Opens the reply pipe for reading
int openRD_answers_pipe(){
    return open("answers_pipe",O_RDONLY);
}

//Opens the reply pipe for writing
int openWR_answers_pipe(){
    return open("answers_pipe",O_RDONLY);
}

//Opens the request pipe for reading
int openRD_requests_pipe(){
    return open("answers_pipe",O_RDONLY);
}

//Opens the request pipe for writing
int openWR_requests_pipe(){
    return open("answers_pipe",O_RDONLY);
}

/*
 * Writes a request in the request pipe
 * What is written depends on ope's value
 */
void write_request(int pipedes, uint16_t ope, uint64_t taskID, struct timing timing, commandline cmd){
    long args;

    switch (ope) {
        case CLIENT_REQUEST_LIST_TASKS:
            write_err(pipedes,"0100110001010011", 16); // 0x4c53
            break;

        case CLIENT_REQUEST_CREATE_TASK:
            args = (long) &cmd.argc;
            write_err(pipedes,"0100001101010010", 16); //0x4352

            write_err(pipedes, &timing.minutes, 64);
            write_err(pipedes, &timing.hours, 32);
            write_err(pipedes, &timing.daysofweek, 8);

            write_err(pipedes, &cmd.argc, 32);
            for (int i = 0; i < args; ++i) {
                write_err(pipedes, &cmd.argv[i]->length, 32);
                write_err(pipedes, &cmd.argv[i]->s, (cmd.argv[i]->length)*2*8);
            }
            break;

        case CLIENT_REQUEST_TERMINATE:
            write_err(pipedes,"0100101101001001", 16); //0x4b49
            break;

        case CLIENT_REQUEST_REMOVE_TASK:
            write_err(pipedes,"0101001001001101", 16); //0x524d
            write_err(pipedes, &taskID,64);
            break;

        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
            write_err(pipedes,"0101010001011000", 16); //0x5458
            write_err(pipedes, &taskID,64);
            break;

        case CLIENT_REQUEST_GET_STDOUT:
            write_err(pipedes,"0101001101001111", 16); //0x534f
            write_err(pipedes, &taskID,64);
            break;

        case CLIENT_REQUEST_GET_STDERR:
            write_err(pipedes,"0101001101000101", 16); //0x5345
            write_err(pipedes, &taskID,64);
            break;
    }
}

/* Reads from the reply pipe when a response to -l ; returns a task list */
task **read_list_task(int pipedes, long taskNb){
    task **tasks;
    for(long i = 0; i < taskNb; i++){
        task a_task;
        long id;
        uint64_t minutes;
        uint32_t hours;
        uint8_t daysofweek;

        read_err(pipedes,&id, 64);
        a_task.taskid = id;

        read_err(pipedes,&minutes, 64);
        read_err(pipedes,&hours, 32);
        read_err(pipedes,&daysofweek, 8);
        struct timing time = {.minutes = minutes, .hours = hours, .daysofweek = daysofweek};
        a_task.t = &time;

        commandline cmd;
        uint32_t argc;
        read_err(pipedes, &argc, 32);
        cmd.argc = argc;

        string *args;
        for(long j = 0; j < argc; j++){
            uint32_t length;
            BYTE arg;
            read_err(pipedes, &length, 32);
            read_err(pipedes, &arg, length);
            string str = {.length = length, .s = &arg};
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
    read_err(pipedes, &taskID, 64);
    return taskID;
}

/* Gathers the list of past runs from the reply pipe and returns it */
run **read_times_exitcode(int pipedes,long runNb){
    run **runs;
    for (int i = 0; i < runNb; ++i) {
        int64_t time;
        uint16_t exitcode;
        read_err(pipedes, &time, 64);
        read_err(pipedes, &exitcode, 16);
        run a_run = {.time = time, .exitcode = exitcode};
        runs[i] = &a_run;
    }
}


/* Reads from the reply pipe and prints the result depending on the request */
void read_answer(int pipedes,uint16_t ope ) { 
    uint16_t rep;
    uint64_t id;
    long taskNb;
    long runNb;
    string a = {0, 0};
    read_err(pipedes, &rep, 16);
    switch (ope) {
        case CLIENT_REQUEST_LIST_TASKS:
            read_err(pipedes, &taskNb, 32);
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
                read_err(pipedes, &err, 16); //ERRCODE
                print_response(ope, rep, err, 0, 0, 0, 0, a);
                break;
            }
        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
            if (rep == SERVER_REPLY_OK) {
                read_err(pipedes, &runNb, 32);
                run **runs = read_times_exitcode(pipedes, runNb);
                print_response(ope, rep, 0, runNb, 0, 0, runs, a);
                break;
            } else {
                uint16_t err;
                read_err(pipedes, &err, 16); //ERRCODE
                print_response(ope, rep, err, 0, 0, 0, 0, a);
                break;
            }
        case CLIENT_REQUEST_GET_STDOUT:
        case CLIENT_REQUEST_GET_STDERR:
            if (rep == SERVER_REPLY_OK) {
                uint32_t length;
                BYTE str;
                read_err(pipedes, &length, 32);
                read_err(pipedes, &str, length);
                print_response(ope, rep, 0, 0, 0, 0, 0, str);
                break;
            } else {
                uint16_t err;
                read_err(pipedes, &err, 16); //ERRCODE
                print_response(ope, rep, err, 0, 0, 0, 0, a);
                break;
            }
    }
}





