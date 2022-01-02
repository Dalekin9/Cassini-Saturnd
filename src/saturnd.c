#include "saturnd.h"

void removeErr(int removeret){
    if (removeret == 0){
        return;
    }else{
        perror("Erreur de remove");
        exit(EXIT_FAILURE);
    }
    
}

void is_malloc_error3(void *p) {
    if (p == NULL) {
        perror("Malloc failure");
        exit(EXIT_FAILURE);
    }
}

char *get_task_path(uint64_t taskid){
    char *username = malloc(200 * sizeof(char));
	is_malloc_error3(username);
	getlogin_r(username, 200);

    char *buff = malloc(sizeof(uint64_t) * sizeof(char));
    sprintf(buff,"%lu",taskid);

    char *folder_path = malloc((strlen(username) + strlen("/tmp/") + strlen("/saturnd/tasks/") + strlen(buff) + 1) * sizeof(char));
    is_malloc_error3(folder_path);

    strcpy(folder_path, "/tmp/");
    strcat(folder_path,username);
    strcat(folder_path,"/saturnd/tasks/");
    strcat(folder_path, buff);

    free(username);
    free(buff);
    return folder_path;
}
