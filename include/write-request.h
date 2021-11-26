#include <stdint.h>

#include "cassini.h"
#include "timing.h"

void write_request(int pipefd, uint16_t operation, uint64_t taskID, struct timing *t, commandline *cmd);
