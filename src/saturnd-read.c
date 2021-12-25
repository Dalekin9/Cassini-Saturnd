#include "saturnd-read.h"


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
uint64_t update_last_id (uint64_t id){
    //supprime contenu du fichier
    //ecrire l'entier dedans
}


/*
 * retourne le dernier id disponible utilsable
 */
uint64_t get_avalaible_id (){
    //lire le fichier "last_taskid" dans  "/tmp//saturnd"
    //le retourner avec + 1
}

/* Returns the default path for the pipes directory : "/tmp/<USERNAME>/tasks/id" */
char* get_name_id_directory(uint64_t id) {
    // get the username (smaller than 200 chars)
    char *username = malloc(200 * sizeof(char));
    is_malloc_error2(username);
    getlogin_r(username, 200);

    char buf1[] = "/tmp/";
    char buf2[] = "/saturnd/tasks";

    char *id_directory = malloc((strlen(username) + strlen(buf1) + strlen(buf2)) * sizeof(char) + sizeof(uint64_t));
    is_malloc_error2(id_directory);

    strcpy(id_directory, buf1);
    strcat(id_directory, username);
    strcat(id_directory, buf2);
    strcat(id_directory, id);

    free(username);
    return id_directory;
}



void create_forlder(struct timing *t, uint32_t length, string **s) {
    //recuperer dernier id dispo
    uint64_t id = get_avalaible_id();
    //creer dossier nom id dans tasks
    char *directory_name = get_name_id_directory(id);
    is_mkdir_error(mkdir(directory_name, 0700));
    //dedans :
    // fichier "timing" qui contient les 3 champs de la structure
    // fichier "argv" qui contient la commandline (sous forme d'une seule chaine de char)
    // deux fichiers "stdout" et "stderr" qui contiennent les resultats des derniers runs.
    // un fichier "runs" qui contient la date et la valeur de retour de chaque execution On mettra toutes les executions dans ce meme fichier :
    //update le dernier id utiliser
    update_last_id(id);
    //free les mallocs de t et c
    free(t);
    free(s);

    //print reponse dans tube de reponse avc id
}


void saturnd_read_reply_c (int fd){

    //lire le timing
    struct timing *t = read_timing(fd);
    //lire commandline
    uint32_t argc;
    is_read_error(read(fd, &argc, sizeof(uint32_t)));
    argc = be32toh(argc);
    string **s = read_args(fd, argc);
    // read the command and its args
    create_forlder(t,argc,s);

}