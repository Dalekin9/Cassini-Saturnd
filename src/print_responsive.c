#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/cassini.h"
#include "include/timing.h"


// -l
void print_response_to_stdout_l (uint16_t reptype, uint32_t nb_task, task *t[]){
    for (int i = 0; i <  nb_task; i++){
        fprintf(stdout, "%d: %lu %d %d " , t[i]->taskid, t[i]->time->minutes, t[i]->time->daysofweek, t[i]->time->daysofweek);
        for (int j =0 ; j < t[i]->command.argc; j ++){
            fprintf(stdout, "%s ", t[i]->command.argv[j]->s);
        }
        fprintf(stdout, "\n");
    }
}

// -x
void print_response_to_stdout_x_ok (uint16_t reptype, uint32_t nb_runs, run *t[]){
    for (int i = 0; i< nb_runs; i++){
        fprintf(stdout, "%ld %d\n", t[i]->time, t[i]->exitcode);
    }
}

// -e
void print_response_to_stdout_e (uint16_t reptype, uint16_t error) {
    if (error == SERVER_REPLY_ERROR_NOT_FOUND) {
        fprintf(stdout, "Il n'existe aucune tâche avec cet identifiant\n");
    } else {
        fprintf(stdout, "La tâche n'a pas encore été exécutée au moins une fois\n");
    }
}


/*
general method to print the response to stdout/stderr
- operation : operation request by the client
*/
void print_response( uint16_t operation, uint16_t reptype, uint16_t error, uint32_t nb_run_task, uint64_t taskid, task *t[], run *r[], string output) {
    switch (operation) {
        case CLIENT_REQUEST_LIST_TASKS : // -l
            print_response_to_stdout_l(reptype, nb_run_task, t);
            break;
        case CLIENT_REQUEST_CREATE_TASK : // -c
            fprintf(stdout, "%lu\n", taskid);
            break;
        case CLIENT_REQUEST_TERMINATE : // -q
            break;
        case CLIENT_REQUEST_REMOVE_TASK : // -r
            if (reptype == SERVER_REPLY_ERROR ){
                fprintf(stderr, "Il n'existe aucune tâche avec cet identifiant\n");
            }
            break;
        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES : // -x
            if (reptype == SERVER_REPLY_OK) {
                print_response_to_stdout_x_ok(reptype, nb_run_task, r);
            } else {
                fprintf(stderr, "Il n'existe aucune tâche avec cet identifiant");
            }
            break;
        case CLIENT_REQUEST_GET_STDOUT : // -o
            fprintf(stdout, "%s\n", output.s);
            break;
        case CLIENT_REQUEST_GET_STDERR : // -e
            print_response_to_stdout_e(reptype, error);
            break;
        default:
            fprintf(stderr, "Pas d'operations.\n");
            exit(1);
    }
}