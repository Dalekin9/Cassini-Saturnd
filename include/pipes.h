#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h> // open()
#include <string.h> // strcat()

int open_request_pipe(char*);

int open_reply_pipe(char*);

void close_pipe(int);

char **find_pipes_names(char*);
