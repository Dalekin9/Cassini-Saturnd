#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "cassini.h"
#include "timing-text-io.h"
#include "timing.h"

void open_pipes_cassini(int* fd, char *pipes_directory);

void open_or_create_pipes_saturnd(int *fd, char *pipes_directory);
