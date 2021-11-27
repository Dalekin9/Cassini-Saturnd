#include "cassini.h"
#include "timing.h"

void print_reply(uint16_t operation, uint16_t reptype, uint16_t error, uint32_t nb_run_task, uint64_t taskid, task *t[], run *r[], string *output);