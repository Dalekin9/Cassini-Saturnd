#ifndef SATURND_READ_H
#define SATURND_READ_H

#include <stdint.h>

#include "server-reply.h"
#include "write-reply.h"
#include "common-read.h"
#include "create-task.h"

void saturnd_read_reply_c (int fd, int fd_rep);

#endif //SATURND_READ_H
