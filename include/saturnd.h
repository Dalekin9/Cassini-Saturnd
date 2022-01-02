#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "write-reply.h"
#include "server-reply.h"
#include "print-reply.h"
#include "read-request.h"

void remove_task(int fd,uint64_t taskid);

uint64_t read_taskID(int fd);

void reply_time_exitCodes(int fd, uint64_t taskid);