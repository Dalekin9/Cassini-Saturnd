#include "cassini.h" // TODO : clean up the include
#include "print-reply.h"

void print_reply_l(uint32_t nbTasks, task **tasks) {
    for (uint32_t i = 0; i < nbTasks; i++) {
        // get the timing as a string
        char buf_timing[TIMING_TEXT_MIN_BUFFERSIZE];
        timing_string_from_timing(buf_timing, tasks[i]->t);

        // print taskid and timing
        fprintf(stdout, "%lu: %s ", tasks[i]->taskid, buf_timing);

        // print command line args
        for (uint32_t j = 0 ; j < tasks[i]->command->argc; j++){
            fprintf(stdout, "%s ", tasks[i]->command->argv[j]->s);
        }
        fprintf(stdout, "\n");
        free(tasks[i]);
    }
    free(tasks);
}

void print_reply_c(uint32_t taskid) {
    fprintf(stdout, "%lu\n", taskid);
}

void print_error(uint16_t errcode) {
    if (errcode == SERVER_REPLY_ERROR_NOT_FOUND) {
        fprintf(stderr, "No task with this ID.\n");
    } else if (errcode == SERVER_REPLY_ERROR_NEVER_RUN) {
        fprintf(stderr, "This task hasn't been run yet\n");
    }
    exit(1);
}

void print_times_and_exit_codes(uint32_t nbRuns, run** runs) {
    for (uint32_t i = 0; i < nbRuns; i++) {
        struct tm *t = localtime(&runs[i]->time);
        int year = t->tm_year + 1900;
        int month = t->tm_mon + 1; // 0 = january
        int day = t->tm_mday;
        int h = t->tm_hour;
        int m = t->tm_min;
        int s = t->tm_sec;
        free(t);

        fprintf(stdout, "%02d-%02d-%02d %02d:%02d:%02d %lu\n", year, month, day, h, m, s, runs[i]->exitcode);
    }
}

void print_output(string *output) {
    fprintf(stdout, "%s\n", output->s);
    free(output);
}