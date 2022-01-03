#include "common-folder.h"

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

/* Returns the default path for the file : "/tmp/<USERNAME>/tasks/id/..." = directory/file*/
char* get_file_path(char *directory, char *file) {

    char *name_file = malloc((strlen(directory) + strlen(file) +1) * sizeof(char));
    is_malloc_error(name_file);

    strcpy(name_file, directory);
    strcat(name_file, file);

    return name_file;
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