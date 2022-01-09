#include "run_task.h"

//convert un tableau de string en un tableau de char (pour la commande exec)
char **get_char_from_string(string **argv, uint32_t length){
    char **tab = malloc((length + 1)* sizeof(char*));
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

/* Checks if the current time corresponds with the timing t */
bool is_correct_timing(struct timing* t){
    // get current time
    time_t rawtime;
    struct tm *local_t;
    time(&rawtime);
    local_t = localtime(&rawtime);
    int day = local_t->tm_wday;
    int h = local_t->tm_hour;
    int m = local_t->tm_min;

    // masks
    uint8_t days_m = t->daysofweek << day;
    uint32_t hours_m = t->hours << h;
    uint64_t minutes_m = t->minutes << m;

    // compare
    if (((t->daysofweek && days_m) == 1)
     && ((t->hours && hours_m) == 1)
     && ((t->minutes && minutes_m) == 1)) {
        return true;
    } else {
        return false;
    }
}

/* For the child process that will execute a task : duplicate stdout and stderr to the
   files at saturnd/tasks/<id>/stdout (resp stderr) */
void move_stdout_stderr(int id) {
    char *dir_path = get_directory_id_path(id);

    char* filename = "/stdout";
    char *out = get_file_path(dir_path, filename);

    filename = "/stderr";
    char *err = get_file_path(dir_path, filename);

    int fd1 = open(out, O_CREAT | O_RDWR, S_IRWXU);
    int fd2 = open(err, O_CREAT | O_RDWR, S_IRWXU);
    dup2(fd1,STDOUT_FILENO);
    dup2(fd2,STDERR_FILENO);

    free(dir_path);
    free(out);
    free(err);
}

void run_tasks(s_task **tasks, int nb_tasks){
    for (int i = 0; i < nb_tasks; i++) {
        if(!(tasks[i]->is_removed)) {
            if (is_correct_timing(tasks[i]->t)) {
                int status; // to store the return status of the child
                int64_t time_of_execution = time(NULL);
                int f = fork();
                if (f == -1) {
                    perror("Fork error");
                    exit(EXIT_FAILURE);
                } else if (f == 0) { // child that will execute the task
                    move_stdout_stderr(i);
                    char **tab = get_char_from_string(tasks[i]->command->argv, tasks[i]->command->argc);
                    char *com = tab[0];
                    execvp(com,tab);
                } else {
                    waitpid(f, &status, 0);
                    if (WIFEXITED(status)) {
                        // write exit code and time of run
                     } else {
                        perror("Le fils ne finit pas bien");
                        exit(EXIT_FAILURE);
                     }
                }
            }
        }
    }
}
