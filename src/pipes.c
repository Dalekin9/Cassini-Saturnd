#include "pipes.h"

/** Terminates the program if the pointer is NULL. */
void is_malloc_error2(void *p) {
    if (p == NULL) {
        perror("Malloc failure");
        exit(EXIT_FAILURE);
    }
}

/* Opens the request pipe.
- name = the path to the pipe.
Function fails if the pipe can't be opened. */
int open_request_pipe(char *name) {
    int fd = open(name, O_RDWR);
    if (fd == -1) {
        perror("Can't open the request pipe ");
        exit(EXIT_FAILURE);
    }
    return fd;
}

/* Opens the reply pipe.
- name = the path to the pipe.
Function fails if the pipe can't be opened. */
int open_reply_pipe(char *name) {
    int fd = open(name, O_RDWR);
    if (fd == -1) {
        perror("Can't open the reply pipe ");
        exit(EXIT_FAILURE);
    }
    return fd;
}

/* Closes the pipe. Function fails if it can't be closed. */
void close_pipe(int fd) {
    int ret = close(fd);
    if (ret == -1){
        perror("Can't close the pipe ");
        exit(EXIT_FAILURE);
    }
}

/* Returns the default path for the pipes directory : "/tmp/<USERNAME>/saturnd/pipes" */
char* write_default_pipes_directory() {
	// get the username (smaller than 200 chars)
	char *username = malloc(200 * sizeof(char));
	is_malloc_error2(username);
	getlogin_r(username, 200);

	char buf1[] = "/tmp/";
    char buf2[] = "/saturnd/pipes/";

	char *pipes_directory = malloc((strlen(username) + strlen(buf1) + strlen(buf2)) * sizeof(char));
    is_malloc_error2(pipes_directory);

	strcpy(pipes_directory, buf1);
	strcat(pipes_directory, username);
    strcat(pipes_directory, buf2);

    free(username);
    return pipes_directory;
}

/* Returns the name for the reply pipe : pipe_directory/saturnd-reply-pipe
Adds a '/' between the pipes_directory and the basename if necessary.

Assumes pipes_directory is NOT null.
*/
char *get_reply_pipe_name(char *pipes_directory) {
    char basename[] = "saturnd-reply-pipe";
    char slash[] = "/";
    char *reply_pipe;

    if (pipes_directory[strlen(pipes_directory)-1] == '/') {
       reply_pipe = malloc ((strlen(pipes_directory) + strlen(basename)) * sizeof(char) + 1);
       is_malloc_error2(reply_pipe);
       strcpy(reply_pipe, pipes_directory);
    } else { // need to add a "/" between dir name and basename
       reply_pipe = malloc ((strlen(pipes_directory) + strlen(basename)) * sizeof(char) + 2);
       is_malloc_error2(reply_pipe);
       strcpy(reply_pipe, pipes_directory);
       strcat(reply_pipe, slash);
    }
    strcat(reply_pipe, basename);
    return reply_pipe;
}

/* Returns the name for the request pipe : pipe_directory/saturnd-request-pipe
Adds a '/' between the pipes_directory and the basename if necessary.

Assumes pipes_directory is NOT null.
*/
char *get_request_pipe_name(char *pipes_directory) {
    char basename[] = "saturnd-request-pipe";
    char slash[] = "/";
    char *request_pipe;

    if (pipes_directory[strlen(pipes_directory)-1] == '/') {
       request_pipe = malloc((strlen(pipes_directory) + strlen(basename)) * sizeof(char) + 1);
       is_malloc_error2(request_pipe);
       strcpy(request_pipe, pipes_directory);
    } else { // need to add a "/" between dir name and basename
       request_pipe = malloc ((strlen(pipes_directory) + strlen(basename)) * sizeof(char) + 2);
       is_malloc_error2(request_pipe);
       strcpy(request_pipe, pipes_directory);
       strcat(request_pipe, slash);
    }
    strcat(request_pipe, basename);
    return request_pipe;
}