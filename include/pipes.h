#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


int open_request_pipe();

int open_reply_pipe();

void close_pipe(int);