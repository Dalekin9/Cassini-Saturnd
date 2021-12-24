#include <unistd.h> // read()

#include "print-reply.h"
#include "client-request.h"

void read_reply(int, uint16_t);

void is_malloc_error(void *p);
void is_read_error(int val);
struct timing *read_timing(int fd);
string *read_string(int fd);
string **read_args(int fd, uint32_t argc);