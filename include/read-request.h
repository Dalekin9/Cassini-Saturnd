#ifndef SATURND_READ_H
#define SATURND_READ_H

#include <stdint.h>
#include <dirent.h>

#include "server-reply.h"
#include "write-reply.h"
#include "common-read.h"
#include "create-task.h"
#include "common-folder.h"

void read_request_c (int fd);
void read_request_std(int fd, int is_stdout);



#endif //SATURND_READ_H
