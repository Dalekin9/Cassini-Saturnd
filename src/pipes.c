#include "pipes.h"

void is_malloc_error2(void *p) {
    if (p == NULL) {
        perror("Malloc failure");
        exit(EXIT_FAILURE);
    }
}

// TODO : rewrite the pipes opening with the right permissions.
// Right now, all are O_RDWR so that it doesn't hang in tests

int open_request_pipe(char *name) {
    int fd = open(name, O_RDWR);
    if (fd == -1) {
        perror("Can't open the request pipe ");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int open_reply_pipe(char *name) {
    int fd = open(name, O_RDWR);
    if (fd == -1) {
        perror("Can't open the reply pipe ");
        exit(EXIT_FAILURE);
    }
    return fd;
}

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

/* Returns the fully formed pipe names :
- request_pipe = "/tmp/<USERNAME>/saturnd/pipes/saturnd-request-pipe"
- answer_pipe = "/tmp/<USERNAME>/saturnd/pipes/saturnd-reply-pipe"
*/
char** find_pipes_names(char *pipes_directory) {
    if (pipes_directory == NULL) {
        write_default_pipes_directory(pipes_directory);
    }

    char r[] = "saturnd-request-pipe";
    char *request_pipe = malloc ((strlen(pipes_directory) + strlen(r)) * sizeof(char));
    is_malloc_error2(request_pipe);
    strcpy(request_pipe, pipes_directory);
    strcat(request_pipe, r);

    char a[] = "saturnd-reply-pipe";
    char *reply_pipe = malloc ((strlen(pipes_directory) + strlen(a)) * sizeof(char));
    is_malloc_error2(reply_pipe);
    strcpy(reply_pipe, pipes_directory);
    strcat(reply_pipe, a);

    char **names = malloc((strlen(request_pipe) + strlen(reply_pipe)) * sizeof(char));
    is_malloc_error2(names);
    names[0] = request_pipe;
    names[1] = reply_pipe;

    return names;
}