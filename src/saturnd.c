#include "../include/read-reply.h"
#include "../include/server-reply.h"
#include "../include/pipes.h"

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


void create_forlder(struct timing *t, commandline c) {
    //recuperer dernier id dispo
    //creer dossier nom id
    //dedans :
    // fichier "timing" qui contient les 3 champs de la structure
    // fichier "argv" qui contient la commandline (sous forme d'une seule chaine de char)
    // deux fichiers "stdout" et "stderr" qui contiennent les resultats des derniers runs.
    // un fichier "runs" qui contient la date et la valeur de retour de chaque execution On mettra toutes les executions dans ce meme fichier :
    //update le dernier id utiliser
    //free les mallocs de t et c
    //print reponse dans tube de reponse avc id
}

void create_opt() {
    int fd = open("saturnd-request-pipe", O_RDONLY);

    close(fd);
    fd = open("saturnd-request-pipe", O_RDONLY);
    close(fd);
}