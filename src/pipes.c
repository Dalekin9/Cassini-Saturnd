#include "pipes.h"
#include "cassini.h"
#include "timing-text-io.h"

/* Stops the program when there's a reading error */
void is_read_error(long readreturn){
    if(readreturn < 0){
        perror("Reading error.\n");
        exit(EXIT_FAILURE);
    }
}

/* Stops the program when there's a writing error */
void is_write_error(long writereturn){
    if(writereturn < 0){
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
    strcopy(request_pipe, pipes_directory);
    strcat(request_pipe, r);

    char a[] = "saturnd-reply-pipe";
    reply_pipe = malloc ((strlen(pipes_directory) + strlen(a)) * sizeof(char));
    is_malloc_error(reply_pipe);
    strcopy(reply_pipe, pipes_directory);
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
int openRD_answers_pipe(char *reply_name){
    int ret = open(reply_name, O_RDONLY);
    is_open_error(ret);
    return ret;
}

/* Opens the reply pipe for writing. Program terminates on error. */
int openWR_answers_pipe(char *reply_name){
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

/*
 * Writes a request in the request pipe.
 * See protocol.md to see which arguments correspond to which operation type.
 * Arguments :
    - pipefd : the fd of the pipe to write to
 */
void write_request(int pipefd, uint16_t operation, uint64_t taskID, timing *timing, commandline *cmd){
    long ret;
    switch (operation) {
        case CLIENT_REQUEST_LIST_TASKS:
            ret = write(pipefd , CLIENT_REQUEST_LIST_TASKS, 16);
            break;

        case CLIENT_REQUEST_CREATE_TASK:
            ret = write_create_request(pipefd, operation, timing, cmd);
            break;

        case CLIENT_REQUEST_TERMINATE:
            ret = write(pipefd, CLIENT_REQUEST_TERMINATE, 16));
            break;

        case CLIENT_REQUEST_REMOVE_TASK:
            BYTE buf[16+64];
            strcpy(buf, CLIENT_REQUEST_REMOVE_TASK);
            strcpy(*buf+64, taskID);
            ret = write(pipefd, buf, 16+64);
            break;

        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
            BYTE buf[16+64];
            strcpy(buf, CLIENT_REQUEST_GET_TIMES_AND_EXITCODES);
            strcpy(*buf+64, taskID);
            ret = write(pipefd, buf, 16+64);
            break;

        case CLIENT_REQUEST_GET_STDOUT:
            BYTE buf[16+64];
            strcpy(buf, CLIENT_REQUEST_GET_STDOUT);
            strcpy(*buf+64, taskID);
            ret = write(pipefd, buf, 16+64);
            break;

        case CLIENT_REQUEST_GET_STDERR:
            BYTE buf[16+64];
            strcpy(buf, CLIENT_REQUEST_GET_STDERR);
            strcpy(*buf+64, taskID);
            ret = write(pipefd, buf, 16+64);
            break;
    }
    is_write_error(ret);
}

int write_create_request(int pipefd, int operation, timing* t, commandline* command) {
    // compute the total length of the request
    int length = 16 + (64+32+8) + 32; // 16=operation; (...)=t; 32=argc
    for (int i = 0; i < cmd->argc; ++i) { // for each element of command,
                                     // add the length of the string + 32 (storing the length)
      length += 32 + cmd.argv[i]->length;
    }

    // create the request
    int current = 0;
    BYTE buf[length];
    strcpy(buf, operation, 16);
    current += 16;
    // copy the timing
    strcpy(*buf+current, t->minutes, 64);
    current += 64;
    strcpy(*buf+current, t->hours, 32);
    current += 32;
    strcpy(*buf+current, t->daysofweek, 8);
    current += 8;
    strcpy(*buf+current, command->argc, 32);
    // copy command->argv
    for (int i = 0; i < cmd->argc, i++) {
        string *str = command->argv[i];
        strcpy(*buf+current, str->length);
        current += 32;
        strcpy(*buf+current, str->s);
        current += str->length;
    }

    return write(pipefd, buf, length);
}






