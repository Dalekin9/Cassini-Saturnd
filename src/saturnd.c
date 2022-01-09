#include "saturnd.h"

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

void find_if_removed(s_task* task, char* dir_path) {
    char *removed_path = get_file_path(dir_path, "/removed");

    int ret = open(removed_path, O_RDONLY);
    if (ret == -1) {
        task->is_removed = false;
    } else {
        task->is_removed = true;
    }
    free(removed_path);
}

void read_all_arguments(s_task *task, char* dir_path) {
    char tmp[] = "/argv";
    char *path = get_file_path(dir_path, tmp);

    int fd = open(path, O_RDONLY);
    uint32_t argc;
    int res = read(fd, &argc, sizeof(uint32_t));
    if (res < 0) {
        perror("Read error"); exit(EXIT_FAILURE);
    }

    string **argv = get_argv(argc, fd);
    task->command = malloc(sizeof(commandline));
    is_malloc_error(task->command);

    task->command->argc = argc;
    task->command->argv = argv;
    free(path);
    close(fd);
}

// TODO ICI
uint64_t read_max_id(char *dir_path) {
    char *filename = "/last_taskid";
    char *tmp = get_directory_path();
    fprintf(stdout, "%s\n", tmp);
    char *path = malloc(sizeof(filename) + sizeof(tmp) + 2);
    strcpy(path, tmp); // les strcpy et strcat font de la merde
    strcat(path, filename);
    fprintf(stdout, "%s\n", path);
    free(tmp);

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("Error opening the last_taskid file");
        exit(EXIT_FAILURE);
    }
    uint64_t max_id;
    read(fd, &max_id, sizeof(uint64_t));
    close(fd);
    free(path);
    return max_id;
}

uint64_t read_all_tasks(s_task** all_tasks) {
    char *path = get_directory_tasks_path();
    uint64_t max_id = read_max_id(path);

    all_tasks = malloc((max_id + 1) * sizeof(s_task));
    for (uint64_t i = 0; i < max_id + 1; i++) {
        all_tasks[i] = malloc(sizeof(s_task));
        is_malloc_error(all_tasks[i]);

        // get the path of the directory (with the id of the task)
        char* dir_path = get_directory_id_path(i);

        // parse the infos
        find_if_removed(all_tasks[i], dir_path);
        read_all_arguments(all_tasks[i], dir_path);

        free(dir_path);
    }
    free(path);
    return max_id + 1;
}

int main(int argc, char * argv[]) {
     create_files(); // create the folders if needed
     create_pipes(); // create the pipes if needed

     s_task** tasks;
     uint64_t nb_tasks = read_all_tasks(tasks); // read all the tasks from disk

     char *pipes_directory = write_default_pipes_directory();
     char *request_pipe_name = get_pipe_name(pipes_directory, "saturnd-request-pipe");

     while (1) {

          run_tasks(tasks, nb_tasks);
          //sleep(5);

          int fd_req = open_pipe(request_pipe_name, O_RDONLY);

          uint16_t op;
          read(fd_req,&op,sizeof(uint16_t));
          op = be16toh(op);

          switch (op){
               case CLIENT_REQUEST_CREATE_TASK :
                    read_request_c(fd_req);
                    break;
               case CLIENT_REQUEST_REMOVE_TASK :
                    break;
               case CLIENT_REQUEST_GET_STDERR :
                    read_request_std(fd_req, false);
                    break;
               case CLIENT_REQUEST_GET_STDOUT :
                    read_request_std(fd_req, true);
                    break;
               case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES :
                    break;
               case CLIENT_REQUEST_LIST_TASKS :
                    break;
               case CLIENT_REQUEST_TERMINATE :
                    write_reply_terminate();
                    exit(0); // kill the deamon
                    break;
               default:
                    break;
          }

     }
}