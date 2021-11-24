#include "pipes.h"

/* Stops the program when there's a reading error */
void is_read_error(long read_return){
    if(read_return < 0){
        perror("Reading error.\n");
        exit(EXIT_FAILURE);
    }
}

/* Stops the program when there's a writing error */
void is_write_error(long write_return){
    if(write_return < 0){
        perror("Writing error.\n");
        exit(EXIT_FAILURE);
    }
}

/* Stops the program if the file can't be opened.*/
void is_open_error(int returnValue) {
    if (returnValue == -1) {
        perror("Can't open the pipe\n");
        exit(EXIT_FAILURE);
    }
}

/* Stops the program if malloc fails */
void is_malloc_error(char *str) {
    if (str == NULL) {
        perror("Malloc error\n");
        exit(EXIT_FAILURE);
    }
}

/* Finds the default path for the pipes directory : "/tmp/<USERNAME>/saturnd/pipes"
and writes it to *pipes_directory
*/
void write_default_pipes_directory(char *pipes_directory) {
	 // get the username (smaller than 200 chars)
	 char *username = malloc(200 * sizeof(char));
     is_malloc_error(username);
	 getlogin_r(username, 200);

	 char buf1[] = "/tmp/";
	 char buf2[] = "/saturnd/pipes/";
	 pipes_directory = malloc((strlen(username) + strlen(buf1) + strlen(buf2))
							  *sizeof(char));
     is_malloc_error(pipes_directory);
	 strcpy(pipes_directory, buf1);
	 strcat(pipes_directory, username);
	 strcat(pipes_directory, buf2);

}

/* Writes the fully formed pipe names to the request_pipe and reply_pipe pointers.
- request_pipe = "/tmp/<USERNAME>/saturnd/pipes/saturnd-request-pipe"
- answer_pipe = "/tmp/<USERNAME>/saturnd/pipes/saturnd-reply-pipe"
*/
void find_pipes_names(char *pipes_directory, char *request_pipe, char *reply_pipe) {
    if (pipes_directory == NULL) {
        write_default_pipes_directory(pipes_directory);
    }

    char r[] = "saturnd-request-pipe";
    request_pipe = malloc ((strlen(pipes_directory) + strlen(r)) * sizeof(char));
    is_malloc_error(request_pipe);
    strcpy(request_pipe, pipes_directory);
    strcat(request_pipe, r);

    char a[] = "saturnd-reply-pipe";
    reply_pipe = malloc ((strlen(pipes_directory) + strlen(a)) * sizeof(char));
    is_malloc_error(reply_pipe);
    strcpy(reply_pipe, pipes_directory);
    strcat(reply_pipe, a);
}

/* Creates a pipe with READ and WRITE instructions
- pipe_name : the pathname to the pipe
Program terminates if the pipe can't be created.
 */
void create_pipe(char *pipe_name){
    if(mkfifo(pipe_name, O_RDWR) < 0){
        fprintf(stderr, "Can't create the pipe %s\n", pipe_name);
        exit(EXIT_FAILURE);
    }
}

/* Opens the reply pipe for reading. Program terminates on error. */
int openRD_reply_pipe(char *reply_name){
    int ret = open(reply_name, O_RDONLY);
    is_open_error(ret);
    return ret;
}

/* Opens the reply pipe for writing.
If the pipe can't be opened, the program tries to create it and then open it.
Program terminates if the last opening fails. */
int openWR_or_create_reply_pipe(char *reply_name){
    int ret = open(reply_name, O_WRONLY);
    if (ret == -1) {
       create_pipe(reply_name);
       ret = open(reply_name, O_WRONLY);
       is_open_error(ret);
    }
    return ret;
}

/* Opens the request pipe for reading.
If the pipe can't be opened, the program tries to create it and then open it.
Program terminates if the last opening fails. */
int openRD_or_create_requests_pipe(char *request_name){
    int ret = open(request_name, O_RDONLY);
    if (ret == -1) {
       create_pipe(request_name);
       ret = open(request_name, O_RDONLY);
       is_open_error(ret);
    }
    return ret;
}


/* Opens the request pipe for writing. Program terminates on error. */
int openWR_requests_pipe(char *request_name){
    int ret = open(request_name, O_WRONLY);
    is_open_error(ret);
    return ret;
}

/* Opens the pipes for cassini :
- the request pipe with WRITE permissions in fd[1]
- the reply pipe with READ permissions in fd[0]
If the files don't exist or can't be opened, the program terminates.

Arguments :
- fd : where to store the fds
- pipes_directory : the name of the directory where the pipes are store.
    If this is NULL, it will be filled with the default value.
*/
void open_pipes_cassini(int* fd, char *pipes_directory) {
    // get the pathnames of the pipes
    char *request_pipe_name;
    char *reply_pipe_name;
    find_pipes_names(pipes_directory, request_pipe_name, reply_pipe_name);

    // init the fd array
    int ret = malloc(fd, 2*sizeof(int));
    is_malloc_error(ret);

    // open the pipes
    fd[0] = openRD_reply_pipe(reply_pipe_name);
    fd[1] = openWR_requests_pipe(request_pipe_name);
}

/* Opens the pipes for cassini :
- the reply pipe with READ permissions in fd[0]
- the request pipe with WRITE permissions in fd[1]
If the files can't be opened, they are created and then opened.
Program terminates if the last opening fails.

Arguments :
- fd : where to store the fds
- pipes_directory : the name of the directory where the pipes are store.
    If this is NULL, it will be filled with the default value.
*/
void open_or_create_pipes_saturnd(int *fd, char *pipes_directory) {
    // get the path names of the pipes
    char *request_pipe_name;
    char *reply_pipe_name;
    find_pipes_names(pipes_directory, request_pipe_name, reply_pipe_name);

    // init the fd array
    int ret = malloc(fd, 2*sizeof(int));
    is_malloc_error(ret);

    // open or create the pipes
    fd[1] = openWR_or_create_reply_pipe(reply_pipe_name);
    fd[0] = openRD__or_create_requests_pipe(request_pipe_name);
}
