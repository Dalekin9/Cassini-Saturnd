#ifndef SATURND_READ_H
#define SATURND_READ_H

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "server-reply.h"
#include "read-reply.h"
#include "write-reply.h"
#include "common-read.h"

void is_mkdir_error(int res);
char* get_directory_path();
char* get_file_path(char *directory, char *file);

void saturnd_read_reply_c (int fd);

#endif //SATURND_READ_H
