#include "folder.h"


//creer les dossiers intermediaires si necessaire : tmp, user, saturnd, tasks et pipes
void create_intermediaire_folders(){
    char *username = malloc(200 * sizeof(char));
    is_malloc_error(username);
    getlogin_r(username, 200);
    
    char *tmp1 = "/tmp";
    char *tmp2 = "/saturnd";
    char *tmp3 = "/";
    char *tmp4 = "/tasks";
    char *tmp5 = "/pipes";
    char *name = malloc( (strlen(tmp1) + strlen(tmp2) + strlen(tmp3) + strlen(username) + strlen(tmp4) + 1 )* sizeof(char));
    char *name2 = malloc( (strlen(tmp1) + strlen(tmp2) + strlen(tmp3) + strlen(username) + strlen(tmp5) + 1 )* sizeof(char));
    strcpy(name,tmp1);
    strcpy(name2,tmp1);
    struct stat st = {0};
    if (stat(name, &st) == -1) {
         is_mkdir_error(mkdir(name, 0700));
    }
    strcat(name,tmp3);
    strcat(name2,tmp3);
    strcat(name,username);
    strcat(name2,username);
    if (stat(name, &st) == -1) {
         is_mkdir_error(mkdir(name, 0700));
    }
    strcat(name, tmp2);
    strcat(name2, tmp2);
    if (stat(name, &st) == -1) {
         is_mkdir_error(mkdir(name, 0700));
    }
    strcat(name, tmp4);
    strcat(name2, tmp5);
    if (stat(name, &st) == -1) {
         is_mkdir_error(mkdir(name, 0700));
    }
    if (stat(name2, &st) == -1) {
         is_mkdir_error(mkdir(name2, 0700));
    }

    free(username);
    free(name);
    free(name2);
}

//creer les dossiers/fichiers au demmarage si necessaire
void create_files(){

    create_intermediaire_folders();

    //fichier last_taskid qui contient -1
    char *dir = get_directory_path();
    char *last_task = get_file_path(dir,"/last_taskid");
    int fp = open(last_task, O_RDWR);
    if (fp == -1){
        int fd = open(last_task, O_CREAT | O_RDWR, S_IRWXU);
        uint64_t id = -1;
        write(fd, &id ,sizeof(uint64_t));
        close(fd);
    }
    close(fp);
    free(last_task);
    free(dir);
}
