#ifndef SATURND_PRINT_H
#define SATURND_PRINT_H

#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>

#include "server-reply.h"
#include "pipes.h"
#include "create-task.h"

void write_reply_c (struct timing *t, uint32_t length, string **s);
void write_reply_std(uint64_t taskid, int is_stdout);

#endif //SATURND_PRINT_H
