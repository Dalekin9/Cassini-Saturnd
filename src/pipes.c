#include "pipes.h"
#include "cassini.h"
#include "timing-text-io.h"
#include "timing.h"

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

/* Creates the pipes (for the request and the reply), assuming they
 * don't already exist.
 * Ends the program on error (if the path isn't valid or accessible).
 */
void create_pipes(char *request_name, char *reply_name){
    if(mkfifo(request_name, O_RDWR) < 0){
        perror("Can't create RP");
        exit(EXIT_FAILURE);
    }

    if(mkfifo(reply_name, O_RDWR) < 0){
        perror("Can't create AP");
        exit(EXIT_FAILURE);
    }
}

/* Opens the reply pipe for reading. Program terminates on error. */
int openRD_reply_pipe(char *reply_name){
    int ret = open(reply_name, O_RDONLY);
    is_open_error(ret);
    return ret;
}

/* Opens the reply pipe for writing. Program terminates on error. */
int openWR_reply_pipe(char *reply_name){
    int ret = open(reply_name, O_WRONLY);
    is_open_error(ret);
    return ret;
}

/* Opens the request pipe for reading. Program terminates on error. */
int openRD_requests_pipe(char *request_name){
    int ret = open(request_name, O_RDONLY);
    is_open_error(ret);
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

void open_or_create_pipes_saturnd(int *fd, char *pipes_directory) {
    // get the pathnames of the pipes
    char *request_pipe_name;
    char *reply_pipe_name;
    find_pipes_names(pipes_directory, request_pipe_name, reply_pipe_name);

    // init the fd array
    int ret = malloc(fd, 2*sizeof(int));
    is_malloc_error(ret);

}

