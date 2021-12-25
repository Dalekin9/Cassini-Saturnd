#ifndef SATURND_READ_H
#define SATURND_READ_H

#include "server-reply.h"
#include "read-reply.h"
#include "saturnd-print.h"

#include <stdint.h>
#include <stdlib.h>
#include <errno.h> 

void is_malloc_error3(void *p);
void is_mkdir_error(int res);
char* get_directory_path();
char* get_file_path(char *directory, char *file);

void saturnd_read_reply_c (int fd);

#endif //SATURND_READ_H
