#ifndef SY5_PROJET_2021_2022_COMMON_FOLDER_H
#define SY5_PROJET_2021_2022_COMMON_FOLDER_H

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common-read.h"

char* get_directory_path();
void is_mkdir_error(int res);
char* get_file_path(char *directory, char *file);

#endif //SY5_PROJET_2021_2022_COMMON_FOLDER_H
