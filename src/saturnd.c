#include "saturnd.h"

// saturnd option create : -c
void saturnd_opt_c(int fd, int fd_rep) {
    saturnd_read_reply_c(fd, fd_rep);
}

int main(int argc, char * argv[]) {

     create_files();
     create_pipes();

     char *pipes_directory = write_default_pipes_directory();
     char *request_pipe_name = get_pipe_name(pipes_directory, "saturnd-request-pipe");
     char *reply_pipe_name = get_pipe_name(pipes_directory, "saturnd-reply-pipe");
    
     while (1) {

          //ouverture des pipes
          int fd_req = open_pipe(request_pipe_name, O_RDONLY);
          int fd_rep = open_pipe(reply_pipe_name, O_WRONLY);

          uint16_t op;
          read(fd_req,&op,sizeof(uint16_t));
          op = be16toh(op);

          switch (op){
               case CLIENT_REQUEST_CREATE_TASK :
                    saturnd_opt_c(fd_req, fd_rep);
                    break;
               case CLIENT_REQUEST_REMOVE_TASK :
                    break;
               case CLIENT_REQUEST_GET_STDERR :
                    break;
               case CLIENT_REQUEST_GET_STDOUT :
                    break;
               case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES :
                    break;
               case CLIENT_REQUEST_LIST_TASKS :
                    break;
               case CLIENT_REQUEST_TERMINATE :
                    break;
               default:
                    break;
          }
     }
     

}