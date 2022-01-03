#include "read-request.h"

void is_mkdir_error(int res){
    if (res == -1) {
        switch (res) {
            case EACCES :
                printf("the parent directory does not allow write");
                exit(EXIT_FAILURE);
            case EEXIST:
                printf("pathname already exists");
                exit(EXIT_FAILURE);
            case ENAMETOOLONG:
                printf("pathname is too long");
                exit(EXIT_FAILURE);
            default:
                perror("mkdir");
                exit(EXIT_FAILURE);
        }
    }
}

/*
 * met a jour le dernier id disponible avec le dernier utilisé
 */
void update_last_id (char *file, uint64_t id){
    //ecrire l'entier dans le fichier
    int r = remove(file);
    if (r == -1) {
        printf("echec de la suppression du fichier : %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    int fd = open(file, O_CREAT | O_RDWR, S_IRWXU);
    write(fd,&id,sizeof(uint64_t));
    close(fd);
}


/*
 * retourne le dernier id disponible utilsable
 */
uint64_t get_avalaible_id (char *file){
    //lire le fichier "last_taskid" dans  "/tmp/username/saturnd"
    uint64_t id;
    int fd = open(file, O_RDONLY);
    read(fd,&id,sizeof(uint64_t));
    close(fd);
    //le retourner avec + 1
    id = id + 1;
    return id;
}

/* Returns the default path for the pipes directory : "/tmp/<USERNAME>/saturnd/tasks/id" */
char* get_directory_id_path(char *d, uint64_t id) {
    char *a = "/tasks/";

    char *ids = malloc(sizeof(uint64_t)*sizeof(char));
    is_malloc_error(ids);
    sprintf(ids,"%lu",id);

    char *id_directory = malloc((strlen(d) + strlen(a) + strlen(ids) + 1) * sizeof(char));
    is_malloc_error(id_directory);

    strcpy(id_directory, d);
    strcat(id_directory, a);
    strcat(id_directory, ids);

    free(ids);
    return id_directory;
}

/* Returns the default path for the pipes directory : "/tmp/<USERNAME>/saturnd/tasks" */
char* get_directory_path() {
    // get the username (smaller than 200 chars)
    char *username = malloc(200 * sizeof(char));
    is_malloc_error(username);
    getlogin_r(username, 200);

    char buf1[] = "/tmp/";
    char buf2[] = "/saturnd";

    char *id_directory = malloc((strlen(username) + strlen(buf1) + strlen(buf2) + 1) * sizeof(char) + sizeof(uint64_t));
    is_malloc_error(id_directory);

    strcpy(id_directory, buf1);
    strcat(id_directory, username);
    strcat(id_directory, buf2);

    free(username);
    return id_directory;
}

/* Returns the default path for the file : "/tmp/<USERNAME>/tasks/id/..." = directory/file*/
char* get_file_path(char *directory, char *file) {

    char *name_file = malloc((strlen(directory) + strlen(file) +1) * sizeof(char));
    is_malloc_error(name_file);

    strcpy(name_file, directory);
    strcat(name_file, file);

    return name_file;
}


void create_folder_task(struct timing *t, uint32_t length, string **s, int fd_rep) {
    char *dir = get_directory_path(); //general directory
    char *file_last_id = get_file_path(dir, "/last_taskid"); //for last_taskid
    //recuperer dernier id dispo
    uint64_t id = get_avalaible_id(file_last_id);

    //creer dossier nom id dans tasks
    char *directory_name = get_directory_id_path(dir, id); //for id directory
    is_mkdir_error(mkdir(directory_name, 0700));

    // fichier "timing" qui contient les 3 champs de la structure
    char *file_timing = get_file_path(directory_name, "/timing");
    int fd = open(file_timing,O_CREAT,S_IRWXU);
    fd = open(file_timing, O_WRONLY);
    size_t len = sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint64_t);
    BYTE buff[len];
    // copy the timing
    size_t sze = 0;
    uint64_t minutes = htobe64(t->minutes);
    memcpy(buff, &minutes, sizeof(uint64_t));
    uint32_t hours = htobe32(t->hours);
    sze += sizeof(uint64_t);
    memcpy(buff+sze, &hours, sizeof(uint32_t));
    uint8_t days = t->daysofweek;
    sze += sizeof(uint32_t);
    memcpy(buff+sze, &t->daysofweek, sizeof(uint8_t)); // no need to convert endian for days : there is only 1 byte
    write(fd, buff, sizeof(buff));
    close(fd);

    // fichier "argv" qui contient la commandline (sous forme d'une seule chaine de char)
    char *file_argv = get_file_path(directory_name, "/argv");   
    fd = open(file_argv,O_CREAT,S_IRWXU);
    fd = open(file_argv, O_WRONLY);
    int size = 0;
    for (int i = 0; i < length; i++) {
        size += ((s[i]->length) * sizeof(char));
    }
    size += sizeof(char);
    char *buf = malloc(size);
    for (int i = 0; i < length; i++) {
        if (i == 0) {
            strcpy(buf, s[i]->s);
        } else {
            strcat(buf, s[i]->s);
        }
    }
    write(fd,buf,size);
    close(fd);
    free(buf);
    // deux fichiers "stdout" et "stderr" qui contiennent les resultats des derniers runs.
    char *file_stdout = get_file_path(directory_name, "/stdout");
    fd = open(file_stdout,O_CREAT,S_IRWXU);
    close(fd); 

    char *file_stderr = get_file_path(directory_name, "/stderr");
    fd = open(file_stderr,O_CREAT,S_IRWXU);
    close(fd); 
    // un fichier "runs" qui contient la date et la valeur de retour de chaque execution On mettra toutes les executions dans ce meme fichier :
    char *file_runs = get_file_path(directory_name, "/runs");
    fd = open(file_runs,O_CREAT,S_IRWXU);
    close(fd); 

    //update le dernier id utiliser
    update_last_id(file_last_id, id);
    //free les mallocs
    free(dir);
    free(file_last_id);
    free(directory_name);
    free(file_timing);
    free(file_argv);
    free(file_stdout);
    free(file_stderr);
    free(file_runs);
    free(t);
    for (int i = 0; i < length; i++){
        free(s[i]->s);
        free(s[i]);
    }
    free(s);

    //print reponse dans tube de reponse avc id
    saturnd_print_reply_c(id, fd_rep);
}


void saturnd_read_reply_c (int fd, int fd_rep){
    //lire le timing
    struct timing *t = read_timing(fd);

    //lire les arguments
    uint32_t argc;
    is_read_error(read(fd, &argc, sizeof(uint32_t)));
    uint32_t val = be32toh(argc);
    string **st = read_args(fd, val);
    close_pipe(fd);
    create_folder_task(t,val,st, fd_rep);
}