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

void print_error_not_found() {
    fprintf(stderr, "No task with this ID.\n");
}

void print_times_and_exit_codes(uint32_t nbRuns, run** runs) {
    for (uint32_t i = 0; i < nbRuns; i++) {
        struct *tm = localtime(runs[i]->time);
        int year = tm->tm_year + 1900;
        int month = tm->tm_month;
        int day = tm->tm_mday;
        int h = tm->tm_hour;
        int m = tm->tm_min;
        int s = tm->tm_sec;

        fprintf(stdout, "%d-%d-%d %d:%d:%d %lu\n", year, month, day, h, m, s, runs[i]->exitcode)
    }
}