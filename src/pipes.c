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
            write_err(write(pipedes,"0101001001001101", 16)); //0x524d
            write_err(write(pipedes, &taskID,64));
            break;

        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
            write_err(write(pipedes,"0101010001011000", 16)); //0x5458
            write_err(write(pipedes, &taskID,64));
            break;

        case CLIENT_REQUEST_GET_STDOUT:
            write_err(write(pipedes,"0101001101001111", 16)); //0x534f
            write_err(write(pipedes, &taskID,64));
            break;

        case CLIENT_REQUEST_GET_STDERR:
            write_err(write(pipedes,"0101001101000101", 16)); //0x5345
            write_err(write(pipedes, &taskID,64));
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

/* Reads from the reply pipe when a response to -l ; returns a task list */
task **read_list_task(int pipedes, long taskNb){
    task **tasks = malloc(sizeof(**tasks));
    for(long i = 0; i < taskNb; i++){
        task a_task;
        long id;
        uint64_t minutes;
        uint32_t hours;
        uint8_t daysofweek;

        read_err(read(pipedes,&id, 64));
        a_task.taskid = id;

        read_err(read(pipedes,&minutes, 64));
        read_err(read(pipedes,&hours, 32));
        read_err(read(pipedes,&daysofweek, 8));
        struct timing time = {.minutes = minutes, .hours = hours, .daysofweek = daysofweek};
        a_task.t = &time;

        commandline cmd;
        uint32_t argc;
        read_err(read(pipedes, &argc, 32));
        cmd.argc = argc;

        string *args = malloc(sizeof(*args));
        for(long j = 0; j < argc; j++){
            uint32_t length;
            BYTE arg;
            read_err(read(pipedes, &length, 32));
            read_err(read(pipedes, &arg, length));
            string str = {.length = length, .s = &arg};
            args[j] = str;
        }
        cmd.argv = &args;
        a_task.command = cmd;
        tasks[i] = &a_task;
    }
    return tasks;
}

/* Reads and returns the ID of the task that has been created */
uint64_t read_create(int pipedes){
    uint64_t taskID;
    read_err(read(pipedes, &taskID, 64));
    return taskID;
}

/* Gathers the list of past runs from the reply pipe and returns it */
run **read_times_exitcode(int pipedes,long runNb){
    run **runs = malloc(sizeof(**runs));
    for (int i = 0; i < runNb; ++i) {
        int64_t time;
        uint16_t exitcode;
        read_err(read(pipedes, &time, 64));
        read_err(read(pipedes, &exitcode, 16));
        run a_run = {.time = time, .exitcode = exitcode};
        *runs[i] = a_run;
    }
    return runs;
}


/* Reads from the reply pipe and prints the result depending on the request */
void read_answer(int pipedes,uint16_t ope ) { 
    uint16_t rep;
    uint64_t id;
    long taskNb;
    long runNb;
    string a = {0, 0};
    read_err(read(pipedes, &rep, 16));
    switch (ope) {
        case CLIENT_REQUEST_LIST_TASKS:
            read_err(read(pipedes, &taskNb, 32));
            task **tasks = read_list_task(pipedes, taskNb);
            print_response(ope, rep, 0, taskNb, 0, tasks, 0, a);
            break;
        case CLIENT_REQUEST_CREATE_TASK:
            id = read_create(pipedes);
            print_response(ope, rep, 0, 0, id, 0, 0, a);
            break;
        case CLIENT_REQUEST_TERMINATE:
            print_response(ope, rep, 0, 0, 0, 0, 0, a);
            break;
        case CLIENT_REQUEST_REMOVE_TASK:
            if (rep == SERVER_REPLY_OK) {
                print_response(ope, rep, 0, 0, 0, 0, 0, a);
                break;
            } else {
                uint16_t err;
                read_err(read(pipedes, &err, 16)); //ERRCODE
                print_response(ope, rep, err, 0, 0, 0, 0, a);
                break;
            }
        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
            if (rep == SERVER_REPLY_OK) {
                read_err(read(pipedes, &runNb, 32));
                run **runs = read_times_exitcode(pipedes, runNb);
                print_response(ope, rep, 0, runNb, 0, 0, runs, a);
                break;
            } else {
                uint16_t err;
                read_err(read(pipedes, &err, 16)); //ERRCODE
                print_response(ope, rep, err, 0, 0, 0, 0, a);
                break;
            }
        case CLIENT_REQUEST_GET_STDOUT:
        case CLIENT_REQUEST_GET_STDERR:
            if (rep == SERVER_REPLY_OK) {
                uint32_t length;
                BYTE str;
                read_err(read(pipedes, &length, 32));
                read_err(read(pipedes, &str, length));
                print_response(ope, rep, 0, 0, 0, 0, 0, str);
                break;
            } else {
                uint16_t err;
                read_err(read(pipedes, &err, 16)); //ERRCODE
                print_response(ope, rep, err, 0, 0, 0, 0, a);
                break;
            }
        default:
            perror("?");
            exit(1);
    }
}





