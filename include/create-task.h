#ifndef CREATE_TASK_H
#define CREATE_TASK_H

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "common-read.h"
#include "common-folder.h"
#include "write-reply.h"
#include "server-reply.h"

void create_folder_task(struct timing *t, uint32_t length, string **s, int fd_rep);

#endif // CREATE_TASK_H