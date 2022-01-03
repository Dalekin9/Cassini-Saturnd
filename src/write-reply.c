#include "write-reply.h"

void write_reply_c (struct timing *t, uint32_t argc, string **argv){
    // create the task
    int taskid = create_new_task(t, argc, argv);
    fprintf(stdout, "%d\n", taskid);

    // open the fd
    int fd = open_reply_pipe_saturnd();

    // prepare the reply
    int length = sizeof(uint16_t) + sizeof(uint64_t);
    BYTE buff[length];
    uint16_t op = htobe16(SERVER_REPLY_OK);
    memcpy(buff, &op, sizeof(op));
    taskid = be64toh(taskid);
    memcpy(buff+2, &taskid, sizeof(uint64_t));

    // write the reply to the pipe and close it
    write(fd, buff, length);
    close_pipe(fd);
}

uint32_t get_size_file(int fd) {
    struct stat st;
    fstat(fd, &st);
    uint32_t size = st.st_size;
    return htobe32(size - 1); // need to remove the last \0
}

void write_reply_std_error(uint16_t errcode, int fd) {
    BYTE buf[sizeof(uint16_t) * 2];
    uint16_t code = htobe16(SERVER_REPLY_ERROR);
    memcpy(buf, &code, sizeof(uint16_t));
    errcode = htobe16(errcode);
    memcpy(buf+sizeof(uint16_t), &errcode, sizeof(uint16_t));

    write(fd, buf, sizeof(uint16_t) *2);
}

void write_reply_std(uint64_t taskid, int is_stdout) {
    int fd = open_reply_pipe_saturnd();

    char *folder_path = get_directory_id_path(taskid);
    DIR *dirp = opendir(folder_path);

    if (dirp == NULL) { // no task with this id
        write_reply_std_error(SERVER_REPLY_ERROR_NOT_FOUND, fd);
    } else {
        // get the name of the file
        char *filepath;
        if (is_stdout) {
            filepath = get_file_path(folder_path, "/stdout");
        } else {
            filepath = get_file_path(folder_path, "/stderr");
        }
        free(folder_path);

        int filefd = open(filepath, O_RDONLY);
        if (filefd == -1) { // file doesn't exist == task was never run
            write_reply_std_error(SERVER_REPLY_ERROR_NEVER_RUN, fd);
        } else { // task was run, reply with the content of the file (+ the OK code)
            // make the first buffer with the repcode and the size of the string
            uint16_t code = htobe16(SERVER_REPLY_OK);
            uint32_t length = get_size_file(filefd);

            BYTE buf_numbers [sizeof(uint16_t) + sizeof(uint32_t)];
            memcpy(buf_numbers, &code, sizeof(uint16_t));
            memcpy(&buf_numbers[sizeof(uint16_t)], &length, sizeof(uint32_t));
            write(fd, &buf_numbers, sizeof(uint16_t) + sizeof(uint32_t));

            // write the contents to the pipe in chunks of 1024 bytes util the end of the file
            BYTE *buf = malloc(1024 * sizeof(BYTE));
            int nb_read = 1024;
            while (nb_read == 1024) {
                nb_read = read(filefd, buf, 1024);
                write(fd, buf, nb_read);
            }
            free(buf);
        }
        free(filepath);
    }
    close_pipe(fd);
}