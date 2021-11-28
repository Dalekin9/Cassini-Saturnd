#include "cassini.h" // TODO : clean up the include
#include "print-reply.h"

void print_reply_l(uint32_t nbTasks, task **tasks) {
    for (uint32_t i = 0; i < nbTasks; i++) {
        // get the timing as a string
        char buf_timing[TIMING_TEXT_MIN_BUFFERSIZE];
        timing_string_from_timing(buf_timing, tasks[i]->t);

        // print taskid and timing
        fprintf(stdout, "%lu: %s " , tasks[i]->taskid, buf_timing);
        // TODO : check the format of the taskid => uint16 (should be fine as a long but I'm not sure)

        // print command line args
        for (uint32_t j = 0 ; j < tasks[i]->command->argc; j++){
            fprintf(stdout, "%s ", tasks[i]->command->argv[j]->s);
        }
        fprintf(stdout, "\n");
    }
}

void print_reply_c(uint32_t taskid) {
    fprintf(stdout, "%lu\n", taskid);
}