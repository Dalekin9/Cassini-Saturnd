#ifndef SATURND_PRINT_H
#define SATURND_PRINT_H

#include <stdint.h> 

#include "server-reply.h"
#include "pipes.h"

void saturnd_print_reply_c (uint64_t id, int fd_rep);

#endif //SATURND_PRINT_H
