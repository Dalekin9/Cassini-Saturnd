#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h> // open()
#include <string.h> // strcat()

int open_request_pipe(char*);
int open_reply_pipe(char*);
void close_pipe(int);


char* write_default_pipes_directory();
char *get_reply_pipe_name(char*);
char *get_request_pipe_name(char*);

