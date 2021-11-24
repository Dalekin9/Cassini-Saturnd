#include "cassini.h"
#include "timing.h"

/*
This file contains all the methods to format and write the
daemon's reponses to stdout (and the errors to stderr).

Only the print_response() method will be used from the outside.
Most of its arguments will be NULL, and it decides with a
switch which sub-methods need to be called.
*/


/* Prints the response to the -l option */
void print_l (uint16_t reptype, uint32_t nb_task, task *t[]){
    for (int i = 0; i <  nb_task; i++){
        fprintf(stdout, "%d: %lu %d %d " , t[i]->taskid, t[i]->t->minutes,
                t[i]->t->daysofweek, t[i]->t->daysofweek);
        for (int j =0 ; j < t[i]->command.argc; j ++){
            fprintf(stdout, "%s ", t[i]->command.argv[j]->s);
        }
        fprintf(stdout, "\n");
    }
}

/* Prints the reponse to the -x option, if the daemon answered OK */
void print_x_ok (uint16_t reptype, uint32_t nb_runs, run *t[]){
    for (int i = 0; i< nb_runs; i++){
        fprintf(stdout, "%ld %d\n", t[i]->time, t[i]->exitcode);
    }
}

/* Prints the response to the -e option */
void print_e (uint16_t reptype, uint16_t error) {
    if (error == SERVER_REPLY_ERROR_NOT_FOUND) {
        fprintf(stdout, "There is no task with this id.\n");
    } else {
        fprintf(stdout, "This task has not been run yet.\n");
    }
}


/* Formats and prints the daemon's response to stdout/stderr.
See protocol.md for further description of the response.
- operation : operation request by the client
- reptype : the type of response
- error : the error code. Can be null
- nb_run_taks : the number of tasks (resp. running process) in the t (resp. r) array.
    Can be null.
- taskid : the id of the task that was modified (can be null)
- t : an array of the tasks that were created (can be null)
- r : an array of the running process (can be null)
- output : the string that the process wrote to stdout (can be null)
*/
void print_response( uint16_t operation, uint16_t reptype, uint16_t error, uint32_t nb_run_task,
                     uint64_t taskid, task *t[], run *r[], string output) {
    switch (operation) {
        case CLIENT_REQUEST_LIST_TASKS : // -l
            print_l(reptype, nb_run_task, t);
            break;
        case CLIENT_REQUEST_CREATE_TASK : // -c
            fprintf(stdout, "%lu\n", taskid);
            break;
        case CLIENT_REQUEST_TERMINATE : // -q
            break;
        case CLIENT_REQUEST_REMOVE_TASK : // -r
            if (reptype == SERVER_REPLY_ERROR ){
                fprintf(stderr, "There is no task with this id.\n");
            }
            break;
        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES : // -x
            if (reptype == SERVER_REPLY_OK) {
                print_x_ok(reptype, nb_run_task, r);
            } else {
                fprintf(stderr, "There is no task with this id.\n");
            }
            break;
        case CLIENT_REQUEST_GET_STDOUT : // -o
            fprintf(stdout, "%s\n", output.s);
            break;
        case CLIENT_REQUEST_GET_STDERR : // -e
            print_e(reptype, error);
            break;
        default:
            fprintf(stderr, "No response.\n");
            exit(1);
    }
}