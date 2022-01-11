#include "create-task.h"

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

/* creates a task and returns the id of the task */
uint64_t create_new_task(struct timing *t, uint32_t length, string **s) {
    char *dir = get_directory_path(); //general directory
    char *file_last_id = get_file_path(dir, "/last_taskid"); //for last_taskid
    //recuperer dernier id dispo
    uint64_t id = get_avalaible_id(file_last_id);

    //creer dossier nom id dans tasks
    char *directory_name = get_directory_id_path(id); //for id directory
    is_mkdir_error(mkdir(directory_name, 0700));

    // fichier "timing" qui contient les 3 champs de la structure
    char *file_timing = get_file_path(directory_name, "/timing");
    int fd = open(file_timing,O_CREAT,S_IRWXU);
    close(fd);
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
    size_t size = sizeof(uint32_t);
    for (int i = 0; i < length; i++) {
        size += sizeof(uint32_t);
        size += (s[i]->length) * sizeof(char);
    }
    char *buf = malloc(size);
    sze = 0;
    uint32_t le = htobe32(length);
    memcpy(buf + sze,&length,sizeof(uint32_t));
    sze += sizeof(uint32_t);
    for (int i = 0; i < length; i++) {
        uint32_t ta = (s[i]->length);
        memcpy(buf + sze,&ta,sizeof(uint32_t));
        sze += sizeof(uint32_t);
        memcpy(buf + sze,s[i]->s,sizeof(char)*(s[i]->length));
        sze += sizeof(char)*(s[i]->length);
    }
    write(fd,buf,size);
    close(fd);
    free(buf);

    // They should only be created after the first run => that way, no file = no run
    // It is not enough that the files are empty : some cmds don't print anything anywhere.
//    // deux fichiers "stdout" et "stderr" qui contiennent les resultats des derniers runs.
//    char *file_stdout = get_file_path(directory_name, "/stdout");
//    fd = open(file_stdout,O_CREAT,S_IRWXU);
//    close(fd);
//
//    char *file_stderr = get_file_path(directory_name, "/stderr");
//    fd = open(file_stderr,O_CREAT,S_IRWXU);
//    close(fd);

    // un fichier "runs" qui contient la date et la valeur de retour de chaque execution
    // On mettra toutes les executions dans ce meme fichier :
    char *file_runs = get_file_path(directory_name, "/runs");
    fd = open(file_runs,O_CREAT,S_IRWXU);
    close(fd); 

    //update le dernier id utilise
    update_last_id(file_last_id, id);

    //free les mallocs
    free(dir);
    free(file_last_id);
    free(directory_name);
    free(file_timing);
    free(file_argv);
//    free(file_stdout);
//    free(file_stderr);
    free(file_runs);
    free(t);
    for (int i = 0; i < length; i++){
        free(s[i]->s);
        free(s[i]);
    }
    free(s);

    return id;
}