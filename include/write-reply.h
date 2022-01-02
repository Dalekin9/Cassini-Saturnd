#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>

#include "server-reply.h"
#include "client-request.h"

void write_times_exitcodes(int fd, bool ok, run **runs, uint32_t nbRuns, uint16_t errcode);

void write_reply_code(int fd, bool ok, uint16_t errcode);