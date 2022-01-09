#ifndef RUN_TASK_H
#define RUN_TASK_H

#include <math.h>
#include <signal.h>
#include <sys/wait.h>
#include <dirent.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/server-reply.h"
#include "../include/common-folder.h"

void run_tasks();

#endif // RUN_TASK
