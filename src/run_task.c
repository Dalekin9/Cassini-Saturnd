#include "run_task.h"

//verifie si le timing est correct pour lancer ou non la tache
bool is_correct_timing(char *path){
    //lecture du timing
    char *tim;
    is_malloc_error(tim = malloc( (strlen(path) + strlen("/timing") + 1) * sizeof(char) ));
    strcpy(tim,path);
    strcat(tim,"/timing");

    int fd_t = open(tim,O_RDONLY);

    uint8_t days;
    uint32_t hours;
    uint64_t minutes;
    
    read(fd_t,&minutes,sizeof(uint64_t));
    read(fd_t,&hours,sizeof(uint32_t));
    read(fd_t,&days,sizeof(uint8_t));

    time_t rawtime;
    struct tm *t;
    time(&rawtime);
    t = localtime(&rawtime);
    int day = t->tm_wday;
    int h = t->tm_hour;
    int m = t->tm_min;

    //masques
    uint8_t days_m = days<<day;
    uint32_t hours_m = hours<<h;
    uint64_t minutes_m = minutes<<m;

    if ( ((days&&days_m) == 1) &&  ((hours&&hours_m) == 1) && ((minutes&&minutes_m) == 1) ) {
        return true;
    } else {
        return false;
    }

}

//recupere les arguments de la tache dans le fichier argv
string **get_argv(uint32_t argc, int fd_a){
    int nb = 0;
    string **argv = malloc(nb * sizeof(string));
    for (int i = 0; i < argc;i++) {

        uint32_t t;
        int res = read(fd_a, &t, sizeof(uint32_t));
        if (res <= 0){
            perror("Error de read n1");
            exit(EXIT_FAILURE);
        }

        string *s = malloc(sizeof(string));
        is_malloc_error(s);
        s->length = t;
        s->s = malloc(sizeof(char) * t);
        is_malloc_error(s->s);
        res = read(fd_a, s->s, (sizeof(char) * t));   
        if (res <= 0){
            perror("Error de read n2");
            exit(EXIT_FAILURE);
        }
        argv = realloc(argv,(nb+1)*sizeof(string));
        
        argv[nb] = malloc(sizeof(string));
        is_malloc_error(argv[nb]);
        argv[nb] = s;
        nb = nb+1;
    }

    return argv;
}

//convert un tableau de string en un tableau de char (pour la commande exec)
char **get_char_from_string(string **argv, uint32_t length){
    char **tab = malloc((length +1)* sizeof(char*));
    is_malloc_error(tab);
    // add all the strings from argv
    for (int i = 0; i < length; i ++){
        tab[i] = malloc(argv[i]->length +1); // +1 for the \0 at the end of the char*
                                             // (no included in the struct string)
        is_malloc_error(tab[i]);
        strcpy(tab[i],argv[i]->s);
        strcpy(tab[i]+argv[i]->length, "\0");
    }
    // add NULL at the last index (for execvp)
    tab[length] = malloc(sizeof(char*));
    is_malloc_error(tab[length]);
    tab[length] = NULL;
    return tab;
}

void run_tasks(){
    char *dir_path = get_directory_tasks_path();
    DIR *d = opendir(dir_path);
    struct dirent *entry;
    while ( (entry = readdir(d)) ){
        if (strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0) {
            char *path;
            is_malloc_error(path = malloc( (strlen(dir_path) + strlen(entry->d_name) + 2) * sizeof(char)));
            strcpy(path,dir_path);
            strcat(path,"/");
            strcat(path,entry->d_name);

            char *remo;
            is_malloc_error(remo = malloc( (strlen(path) + strlen("/removed") + 1) * sizeof(char)));
            strcpy(remo,path);
            strcat(remo,"/removed");  

            //ouvrir removed pour tester si la tache est supprimee
            int fd = open(remo,O_RDONLY);
            if (fd == -1) {

                if (is_correct_timing(path)){
                    // lire argv et stocker les arguments
                    char *arg;
                    is_malloc_error(arg = malloc( strlen(path) + ((strlen("/argv") + 1) * sizeof(char)) ));
                    strcpy(arg,path);
                    strcat(arg,"/argv");

                    int fd_a = open(arg,O_RDONLY);
                    uint32_t argc;
                    int res = read(fd_a, &argc, sizeof(uint32_t));
                    if (res <= 0) {
                        perror("Erreur de read");
                    } else {

                        string **argv = get_argv(argc,fd_a);   

                        char **tab = get_char_from_string(argv,argc);
                        char *com = tab[0];

                        int status;
                        int f = fork();
                        if (f == -1) {
                            perror("Erreur de fork");
                            exit(EXIT_FAILURE);
                        } else if (f == 0) {
                            char *out;
                            is_malloc_error(out = malloc( strlen(path) + ((strlen("/stdout") + 1) * sizeof(char)) ));
                            strcpy(out,path);
                            strcat(out,"/stdout");

                            char *err;
                            is_malloc_error(err = malloc( strlen(path) + ((strlen("/stderr") + 1) * sizeof(char)) ));
                            strcpy(err,path);
                            strcat(err,"/stderr");

                            int fd1 = open(out,O_CREAT | O_WRONLY, S_IRWXU);
                            int fd2 = open(err,O_CREAT | O_WRONLY, S_IRWXU);
                            dup2(fd1,STDOUT_FILENO);
                            dup2(fd2,STDERR_FILENO);

                            execvp(com,tab);
                            kill(getpid(),SIGTERM);
                        } else {

                            waitpid(f,&status,0);
                            if (WIFEXITED(status)) {
                                //ecrire dans le fichiers run la date 
                                for (int i = 0; i < argc; i++){
                                    free(argv[i]->s);
                                    free(argv[i]);
                                    //free(tab[i]);
                                }
                                
                                free(argv);
                                //free(tab);
                                free(arg);
                                close(fd_a);
                            } else {
                                perror("Le fils ne finit pas bien");
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                }
            }
        }
    }
    closedir(d);
}