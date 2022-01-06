#include "saturnd.h"

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