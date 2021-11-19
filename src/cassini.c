#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cassini.h"


typedef struct {
  uint32_t taskid;
  struct timing *t;
  commandline command;
} task;

typedef struct {
  int64_t time;
  uint16_t exitcode;
} run;



const char usage_info[] = "\
   usage: cassini [OPTIONS] -l -> list all tasks\n\
      or: cassini [OPTIONS]    -> same\n\
      or: cassini [OPTIONS] -q -> terminate the daemon\n\
      or: cassini [OPTIONS] -c [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] COMMAND_NAME [ARG_1] ... [ARG_N]\n\
          -> add a new task and print its TASKID\n\
             format & semantics of the \"timing\" fields defined here:\n\
             https://pubs.opengroup.org/onlinepubs/9699919799/utilities/crontab.html\n\
             default value for each field is \"*\"\n\
      or: cassini [OPTIONS] -r TASKID -> remove a task\n\
      or: cassini [OPTIONS] -x TASKID -> get info (time + exit code) on all the past runs of a task\n\
      or: cassini [OPTIONS] -o TASKID -> get the standard output of the last run of a task\n\
      or: cassini [OPTIONS] -e TASKID -> get the standard error\n\
      or: cassini -h -> display this message\n\
\n\
   options:\n\
     -p PIPES_DIR -> look for the pipes in PIPES_DIR (default: /tmp/<USERNAME>/saturnd/pipes)\n\
";


// -l
void print_response_to_stdout_l (uint16_t reptype, uint32_t nb_task, task *t[]){
    for (int i = 0; i <  nb_task; i++){
        fprintf(stdout, "%d: %lu %d %d " , t[i]->taskid, t[i]->t->minutes, t[i]->t->daysofweek, t[i]->t->daysofweek);
        for (int j =0 ; j < t[i]->command.argc; j ++){
            fprintf(stdout, "%s ", t[i]->command.argv[j]->s);
        }
        fprintf(stdout, "\n");
    }
}

// -c
void print_response_to_stdout_c (uint16_t reptype, uint64_t taskid) {
    fprintf(stdout, "%lu\n", taskid);
}

// -q
void print_response_to_stdout_q (uint16_t reptype, uint64_t taskid) {
    //s'il faut afficher que c'est la fin du demon alors :
    fprintf(stdout, "End of Demon\n");
    //si c'est pas le cas 
    //return;
}

// -r
void print_response_to_stdout_r (uint16_t reptype) {
    if (reptype == SERVER_REPLY_OK ){
        return;
    } else {
        //on suppose directement que on aura forcement NF pour l'erreur (?)
        fprintf(stderr, "Il n'existe aucune tâche avec cet identifiant\n");
    }
}

// -x
void print_response_to_stdout_x_ok (uint16_t reptype, uint32_t nb_runs, run *t[]){
    for (int i = 0; i< nb_runs; i++){
        fprintf(stdout, "%ld %d\n", t[i]->time, t[i]->exitcode);
    }
}

void print_response_to_stdout_x_err (uint16_t reptype, uint16_t errcode){
    fprintf(stderr, "Il n'existe aucune tâche avec cet identifiant");
}

// -o
void print_response_to_stdout_o (uint16_t reptype, string output){
    fprintf(stdout, "%s\n", output.s);
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
            print_response_to_stdout_c(reptype, taskid);
            break;
        case CLIENT_REQUEST_TERMINATE : // -q
            print_response_to_stdout_q(reptype, taskid);
            break;
        case CLIENT_REQUEST_REMOVE_TASK : // -r
            print_response_to_stdout_r(reptype);
            break;
        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES : // -x
            if (reptype == SERVER_REPLY_OK) {
                print_response_to_stdout_x_ok(reptype, nb_run_task, r);
            } else {
                print_response_to_stdout_x_err(reptype, error);
            }
            break;
        case CLIENT_REQUEST_GET_STDOUT : // -o
            print_response_to_stdout_o(reptype, output);
            break;
        case CLIENT_REQUEST_GET_STDERR : // -e
            print_response_to_stdout_e(reptype, error);
            break;
        default:
            fprintf(stderr, "Pas d'operations.\n");
            exit(1);
    }
}


int main(int argc, char * argv[]) {
  errno = 0;
  
  char * minutes_str = "*";
  char * hours_str = "*";
  char * daysofweek_str = "*";
  char * pipes_directory = NULL;
  
  uint16_t operation = CLIENT_REQUEST_LIST_TASKS;
  uint64_t taskid;
  
  int opt;
  char * strtoull_endp;
  while ((opt = getopt(argc, argv, "hlcqm:H:d:p:r:x:o:e:")) != -1) {
    switch (opt) {
    case 'm':
      minutes_str = optarg;
      break;
    case 'H':
      hours_str = optarg;
      break;
    case 'd':
      daysofweek_str = optarg;
      break;
    case 'p':
      pipes_directory = strdup(optarg);
      if (pipes_directory == NULL) goto error;
      break;
    case 'l':
      operation = CLIENT_REQUEST_LIST_TASKS;
      break;
    case 'c':
      operation = CLIENT_REQUEST_CREATE_TASK;
      break;
    case 'q':
      operation = CLIENT_REQUEST_TERMINATE;
      break;
    case 'r':
      operation = CLIENT_REQUEST_REMOVE_TASK;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'x':
      operation = CLIENT_REQUEST_GET_TIMES_AND_EXITCODES;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'o':
      operation = CLIENT_REQUEST_GET_STDOUT;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'e':
      operation = CLIENT_REQUEST_GET_STDERR;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'h':
      printf("%s", usage_info);
      return 0;
    case '?':
      fprintf(stderr, "%s", usage_info);
      goto error;
    }
  }

  // --------
  // | TODO |
  // --------

  //print in stdout/stderr infos
  //an exemple to use the function
  string a = {0,""};
  print_response(operation, 0, 0, 0, 0, NULL ,NULL , a);


  return EXIT_SUCCESS;

 error:
  if (errno != 0) perror("main");
  free(pipes_directory);
  pipes_directory = NULL;
  return EXIT_FAILURE;
}

