#include "saturnd.h"
#include <math.h>
#include <signal.h>
#include <sys/wait.h>

void run_tasks(){
     char *dir_path = get_directory_tasks_path();
     DIR *d = opendir(dir_path);
     struct dirent *entry;
     while ( (entry = readdir(d)) ){
          printf("entry name : %s\n",entry->d_name);
          if (strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0) {
               printf("dans un entry\n");
               char *path;
               is_malloc_error(path = malloc( (strlen(dir_path) + strlen(entry->d_name) + 2) * sizeof(char) ));
               strcpy(path,dir_path);
               strcat(path,"/");
               strcat(path,entry->d_name);

               printf("le path : %s\n",path);

               char *remo;
               is_malloc_error(remo = malloc( (strlen(path) + strlen("/removed") + 1) * sizeof(char) ));   
               strcpy(remo,path);
               strcat(remo,"/removed");  

               printf("le removed : %s\n",remo);  

               //ouvrir removed pour tester si la tache est supprimee
               int fd = open(remo,O_RDONLY);
               if (fd == -1) {

                    printf("open de remo = -1 \n");

                    //lecture du timing
                    char *tim;
                    is_malloc_error(tim = malloc( (strlen(path) + strlen("/timing") + 1) * sizeof(char) ));
                    strcpy(tim,path);
                    strcat(tim,"/timing");

                    printf("le timing : %s\n",tim);

                    int fd_t = open(tim,O_RDONLY);

                    uint8_t days;
                    uint8_t days_m;
                    uint32_t hours;
                    uint32_t hours_m;
                    uint64_t minutes;
                    uint64_t minutes_m;
                    
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

                    printf("moment masque \n");
                    //masques
                    days_m = days<<day;
                    hours_m = hours<<h;
                    minutes_m = minutes<<m;

                    printf("fin masque \n");

                    int dd = days&&days_m;
                    int hh = hours&&hours_m;
                    int mm = minutes&&minutes_m;

                    printf("val dd %d \n",dd);
                    printf("val hh %d \n",hh);
                    printf("val mm %d \n",mm);

                    //regarder si days ok
                    if ( (days&&days_m) == 1) { //si oui alors

                         printf("day ok\n");
                         //regarder si hours ok
                         if ((hours&&hours_m) == 1) { //si oui alors

                              printf("hours ok\n");
                              //regarder si minutes ok
                              if ((minutes&&minutes_m) == 1) { //si oui alors

                                   printf("minutes ok\n");

                                   // lire argv et stocker les arguments
                                   char *arg;
                                   is_malloc_error(arg = malloc( strlen(path) + ((strlen("/argv") + 1) * sizeof(char)) ));
                                   strcpy(arg,path);
                                   strcat(arg,"/argv");

                                   printf("le argv : %s\n",arg);
                                   
                                   char **tab;
                                   is_malloc_error(tab = malloc(sizeof(char)));
                                   uint32_t t;
                                   
                                   int nb = 0;

                                   int fd_a = open(arg,O_RDONLY);
                                   bool end = false;
                                   while ( !end) {

                                        int res = read(fd_a, &t, sizeof(uint32_t));
                                        if (res == 0) {
                                             end = true;
                                        } else if (end < 0) {
                                             perror("Error de read");
                                             exit(EXIT_FAILURE);
                                        } else {
                                             printf("jai pu lire un uint 32\n");
                                             
                                             t = htobe32(t);
                                             printf("val de t : %u\n",t);
                                             printf("je suis au nb = %d\n",nb);

                                             char *c = malloc(sizeof(char) * t);
                                             is_malloc_error(c);
                                             read(fd_a, c, (sizeof(char) * t));   

                                             //printf("un arg : %s\n",c); 

                                             if (nb == 0) {
                                                  printf("cas nb 0\n");
                                                  tab = realloc(tab, sizeof(char)*(t));
                                             } else {
                                                  printf("cas nb > 0\n");
                                                  tab = realloc(tab, sizeof(tab) + ( sizeof(char) * (t)) );
                                             }
                                             printf("fin de realloc \n");
                                             printf("taille de c : %ld\n",sizeof(c));
                                             printf("taille de tab : %ld\n",sizeof(tab));
                                             tab[nb] = c;
                                             nb = nb+1;
                                             free(c);
                                        }
                                   }

                                   tab = realloc(tab, sizeof(tab) + sizeof(NULL) );
                                   tab[nb] = NULL;

                                   printf("GO EXECUTER\n");
                                   int status;
                                   int f = fork();
                                   if (f == -1) {
                                        perror("Erreur de fork");
                                        exit(EXIT_FAILURE);
                                   } else if (f == 0) {
                                        execvp(tab[0],tab);
                                        printf("fin EXECUTER\n");
                                        exit(2);
                                   } else {
                                        printf("reprise processus intial\n");

                                        wait(&status);
                                        if (WIFEXITED(status)) {
                                             //ecrire dans le fichiers run la date 
                                             for (int i = 0; i < nb; i++){
                                                  free(tab[i]);
                                             }
                                             free(tab);
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
                    free(tim);
                    close(fd_t);
               }
               free(path);
               free(remo);
               close(fd);
          }
     }
     closedir(d);
}

int main(int argc, char * argv[]) {

     create_files();
     create_pipes();

     char *pipes_directory = write_default_pipes_directory();
     char *request_pipe_name = get_pipe_name(pipes_directory, "saturnd-request-pipe");

     while (1) {

          run_tasks();
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