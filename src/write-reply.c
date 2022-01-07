#include "write-reply.h"

/* Writes a reply for operations that only returns a repcode
   with it's corresponding error if it is err*/
void write_reply_code(int fd, bool ok, uint16_t errcode){
    if(ok){
        uint16_t rep = SERVER_REPLY_OK;
        write(fd, &rep, sizeof(uint16_t));
    }else{
        size_t length = sizeof(uint16_t) * 2;
        BYTE buff[length];
        uint16_t rep = SERVER_REPLY_ERROR;
        memcpy(buff, &rep, sizeof(uint16_t));
        memcpy(buff + sizeof(uint16_t), &errcode, sizeof(uint16_t));
        write(fd, buff, length);

        close_pipe(fd);
    }
}

void write_reply_c (struct timing *t, uint32_t argc, string **argv){
    // create the task
    uint64_t taskid = create_new_task(t, argc, argv);

    // open the fd
    int fd = open_reply_pipe_saturnd();

    // prepare the reply
    int length = sizeof(uint16_t) + sizeof(uint64_t);
    BYTE buff[length];
    uint16_t op = htobe16(SERVER_REPLY_OK);
    memcpy(buff, &op, sizeof(op));
    taskid = be64toh(taskid);
    memcpy(buff+2, &taskid, sizeof(uint64_t));

    // write the reply to the pipe and close it
    write(fd, buff, length);
    close_pipe(fd);
}

uint32_t get_size_file(int fd) {
    struct stat st;
    fstat(fd, &st);
    uint32_t size = st.st_size;
    return htobe32(size - 1); // need to remove the last \0
}

void write_reply_std_error(uint16_t errcode, int fd) {
    BYTE buf[sizeof(uint16_t) * 2];
    uint16_t code = htobe16(SERVER_REPLY_ERROR);
    memcpy(buf, &code, sizeof(uint16_t));
    errcode = htobe16(errcode);
    memcpy(buf+sizeof(uint16_t), &errcode, sizeof(uint16_t));

    write(fd, buf, sizeof(uint16_t) *2);
}

void write_reply_std(uint64_t taskid, int is_stdout) {
    int fd = open_reply_pipe_saturnd();

    char *folder_path = get_directory_id_path(taskid);
    DIR *dirp = opendir(folder_path);

    if (dirp == NULL) { // no task with this id
        write_reply_std_error(SERVER_REPLY_ERROR_NOT_FOUND, fd);
    } else {
        // get the name of the file
        char *filepath;
        if (is_stdout) {
            filepath = get_file_path(folder_path, "/stdout");
        } else {
            filepath = get_file_path(folder_path, "/stderr");
        }
        free(folder_path);

        int filefd = open(filepath, O_RDONLY);
        if (filefd == -1) { // file doesn't exist == task was never run
            write_reply_std_error(SERVER_REPLY_ERROR_NEVER_RUN, fd);
        } else { // task was run, reply with the content of the file (+ the OK code)
            // make the first buffer with the repcode and the size of the string
            uint16_t code = htobe16(SERVER_REPLY_OK);
            uint32_t length = get_size_file(filefd);

            BYTE buf_numbers [sizeof(uint16_t) + sizeof(uint32_t)];
            memcpy(buf_numbers, &code, sizeof(uint16_t));
            memcpy(&buf_numbers[sizeof(uint16_t)], &length, sizeof(uint32_t));
            write(fd, &buf_numbers, sizeof(uint16_t) + sizeof(uint32_t));

            // write the contents to the pipe in chunks of 1024 bytes util the end of the file
            BYTE *buf = malloc(1024 * sizeof(BYTE));
            int nb_read = 1024;
            while (nb_read == 1024) {
                nb_read = read(filefd, buf, 1024);
                write(fd, buf, nb_read);
            }
            free(buf);
        }
        free(filepath);
    }
    close_pipe(fd);
}

void write_reply_terminate() {
    uint16_t code = htobe16(SERVER_REPLY_OK);
    int fd = open_reply_pipe_saturnd();
    write(fd, &code, sizeof(uint16_t));
}

void write_times_exitcodes(int fd,bool ok, run **runs, uint32_t nbRuns, uint16_t errcode){

    if(ok){
        size_t length = sizeof(uint16_t) 
        + sizeof(uint32_t) 
        + (nbRuns * (sizeof(int64_t) + sizeof(uint16_t)));

        BYTE buff[length];
        int currsize = 0;
        uint16_t rep = SERVER_REPLY_OK;
        memcpy(buff, &rep, sizeof(uint16_t));
        currsize += sizeof(uint16_t);

        memcpy(buff + currsize, &nbRuns, sizeof(uint32_t));
        currsize += sizeof(uint32_t);

        for (uint32_t i = 0; i < nbRuns; i++){
            int64_t runTime = runs[i]->time;
            memcpy(buff+currsize, &runTime, sizeof(int64_t));
            currsize += sizeof(int64_t);

            uint16_t exitCode = runs[i]->exitcode;
            memcpy(buff+currsize, &exitCode, sizeof(uint16_t));
            currsize += sizeof(uint16_t);
        }

        write(fd, buff, length);
        free(runs);
        close_pipe(fd);
        
    }else{
        write_reply_code(fd,ok,errcode);
        free(runs);
    }
}

void write_reply_t_ec(uint64_t taskid){

    int fd = open_reply_pipe_saturnd();

    char* folder_path = get_directory_id_path(taskid);
    char* runs_path = malloc((strlen(folder_path) + strlen("/runs") +1) * sizeof(char));
    is_malloc_error(runs_path);

    strcpy(runs_path, folder_path);
    strcat(runs_path,"/runs");

    free(folder_path);

    int fd_runs = open(runs_path, O_RDONLY);
    if(fd_runs == -1){
        free(runs_path);
        write_times_exitcodes(fd, false,NULL, 0, SERVER_REPLY_ERROR_NOT_FOUND);
    }

    uint32_t nb_runs = 0;
    bool end = false;

    size_t single = sizeof(int64_t) + sizeof(uint16_t);
    run **runs = malloc(single);
    is_malloc_error(runs);
    while(!end){
        int64_t *date;
        uint16_t *exitc;

        int res = read(fd_runs, &date, sizeof(int64_t));
        if(res == 0){
            end = true;
        }else if (res > 0) {
            nb_runs++;
            runs = realloc(runs,single * nb_runs);
            run *r = malloc(single);
            is_malloc_error(r);
            
            r->time = *date;
            if (read(fd_runs, &exitc, sizeof(uint16_t) == -1)){
                perror("Erreur de read");
                exit(EXIT_FAILURE);
            } 
            r->exitcode = *exitc;
            runs[nb_runs - 1] = r;
        } else {
            perror("Erreur de read");
            exit(EXIT_FAILURE);
        }
    }
    free(runs_path);
    write_times_exitcodes(fd, true, runs, nb_runs, 0);
}

void write_reply_rm(uint64_t taskid){

    int fd = open_reply_pipe_saturnd();
    char *folder_path = get_directory_id_path(taskid);

    DIR *d = opendir(folder_path);

    if(d){

        char* removedname = malloc((strlen(folder_path) + strlen("/removed") +1) * sizeof(char));
        is_malloc_error(removedname);
        strcpy(removedname, folder_path);
        strcat(removedname, "/removed");

        int removed = open(removedname, O_CREAT);
        close(removed);
        free(removedname);

        free(folder_path);

        close_pipe(fd);

    }else{
        write_reply_code(fd, false, SERVER_REPLY_ERROR_NOT_FOUND);
        free(folder_path);
    }
}