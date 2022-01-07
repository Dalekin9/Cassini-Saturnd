#ifndef SATURND_PRINT_H
#define SATURND_PRINT_H

#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "server-reply.h"
#include "pipes.h"
#include "create-task.h"

void write_reply_c (struct timing *t, uint32_t length, string **s);
void write_reply_std(uint64_t taskid, int is_stdout);
void write_reply_terminate();
void write_reply_t_ec(uint64_t taskid);
void write_reply_rm(uint64_t taskid);
#endif //SATURND_PRINT_H
