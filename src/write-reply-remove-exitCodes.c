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
    }
}

void write_times_exitcodes(int fd, bool ok, run **runs, uint32_t nbRuns, uint16_t errcode){
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
        
    }else{
        write_reply_code(fd,ok,errcode);
        free(runs);
    }
}

void remove_task(int fd,uint64_t taskid){

    char *folder_path = get_task_path(taskid);
    printf("folder value : %s \n", folder_path);

    DIR *d = opendir(folder_path);

    if(d){
        char* timingname = malloc((strlen(folder_path) + strlen("/timing") +1) * sizeof(char));
        is_malloc_error3(timingname);
        strcpy(timingname, folder_path);
        strcat(timingname, "/timing");

        removeErr(remove(timingname));
        free(timingname);

        char* argvname = malloc((strlen(folder_path) + strlen("/argv") +1) * sizeof(char));
        is_malloc_error3(argvname);
        strcpy(argvname, folder_path);
        strcat(argvname, "/argv");

        removeErr(remove(argvname));
        free(argvname);

        char* stdoutname = malloc((strlen(folder_path) + strlen("/stdout") +1) * sizeof(char));
        is_malloc_error3(stdoutname);
        strcpy(stdoutname, folder_path);
        strcat(stdoutname, "/stdout");

        removeErr(remove(stdoutname));
        free(stdoutname);

        char* stderrname = malloc((strlen(folder_path) + strlen("/stderr")+1) * sizeof(char));
        is_malloc_error3(stderrname);
        strcpy(stderrname, folder_path);
        strcat(stderrname, "/stderr");

        removeErr(remove(stderrname));
        free(stderrname);

        char* runsname = malloc((strlen(folder_path) + strlen("/runs")+1) * sizeof(char));
        is_malloc_error3(runsname);
        strcpy(runsname, folder_path);
        strcat(runsname, "/runs");

        removeErr(remove(runsname));
        free(runsname);

        rmdir(folder_path);

        free(folder_path);

    }else{
        printf("cunt\n");
        write_reply_code(fd, false, SERVER_REPLY_ERROR_NOT_FOUND);
        free(folder_path);
    }
}

void reply_time_exitCodes(int fd, uint64_t taskid){
    char* folder_path = get_task_path(taskid);
    char* runs_path = malloc((strlen(folder_path) + strlen("/runs") +1) * sizeof(char));
    is_malloc_error3(runs_path);

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
    is_malloc_error3(runs);
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
            is_malloc_error3(r);
            
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