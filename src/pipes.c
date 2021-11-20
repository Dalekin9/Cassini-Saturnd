#include <unistd.h>
#include <fcntl.h>
#include "cassini.h"

//Creation des tubes de requête et de réponse
int create_pipes(){
    if(mkfifo("requests_pipe",O_RDWR) < 0){
        perror("Can't create RP");
        exit(1);
    }

    if(mkfifo("answers_pipe",O_RDWR) < 0){
        perror("Can't create AP");
        exit(1);
    };
}

//Ouverture en lecture pour le client
int openRD_answers_pipe(){
    return open("answers_pipe",O_RDONLY);
}

//Ouverture en écriture pour le client
int openWR_answers_pipe(){
    return open("answers_pipe",O_RDONLY);
}

//Ouverture en lecture pour le Daemon
int openRD_requests_pipe(){
    return open("answers_pipe",O_RDONLY);
}

//Ouverture en écriture pour le Daemon
int openWR_requests_pipe(){
    return open("answers_pipe",O_RDONLY);
}

//Ecriture du client dans le tube des req
int write_request(int fd){
    int reqSize = 0;
    while (0){ //TODO
        write(fd, STDIN_FILENO, 1);
        reqSize += 1;
    }
    return 0;
}

/*
 * Lecture du tube de réponse par le client
 * Doit renvoyer un tab de tab de String
 * Besoin d'operation, nombres de sous-tableaux, fd pour trad
 */
char* read_answer(int length, int fda,uint16_t ope ){ //TODO
    /* *char[] answbuf;
        switch(ope)
            case  :
                read(fda[0], answbuf,)
                */
    return "a";
}

