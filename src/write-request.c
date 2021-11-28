#include "cassini.h"        // TODO : clean up the imports
#include "write-request.h"

void write_request_l(int fd, uint16_t operation) {
    operation = htobe16(operation);
    write(fd, &operation, sizeof(operation));
}