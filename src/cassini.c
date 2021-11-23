#include "cassini.h"

const char usage_info[] = "\
   usage: cassini [OPTIONS] -l -> list all tasks\n\
      or: cassini [OPTIONS]    -> same\n\
      or: cassini [OPTIONS] -q -> terminate the daemon\n\
      or: cassini [OPTIONS] -c [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] COMMAND_NAME [ARG_1] ... [ARG_N]\n\
          -> add a new task and print its TASKID\n\
             format & semantics of the \"timing\" fields defined here:\n\
             https://pubs.opengroup.org/onlinepubs/9699919799/utilities/crontab.html\n\
             default value for each field is \"*\"\n\
      or: cassini [OPTIONS] -r TASKID -> remove a task\n\
      or: cassini [OPTIONS] -x TASKID -> get info (time + exit code) on all the past runs of a task\n\
      or: cassini [OPTIONS] -o TASKID -> get the standard output of the last run of a task\n\
      or: cassini [OPTIONS] -e TASKID -> get the standard error\n\
      or: cassini -h -> display this message\n\
\n\
   options:\n\
     -p PIPES_DIR -> look for the pipes in PIPES_DIR (default: /tmp/<USERNAME>/saturnd/pipes)\n\
";

/*
Parses a char* into a string* :
- changes the char* into a BYTE and put the data into it
  (without the trailing \0)
- and computes the length of the string (still without the \0) into a uint32
 */
string* argToString (char* c) {
  string *str = malloc(sizeof(string));

  // length without the \0
  str->length = strlen(c);

  // copy the string into the array WITHOUT the trailling \0
  str->s = malloc(str->length);
  memcpy(c, str->s, str->length);

  return str;
}


/*
Parses the arguments given that were passed to cassini.
The arguments are transformed from char* to string*.
Returns a commandline struct that contains all the args, with the name of the
command as the first element of the array of string*.

Exits the program with return code 1  if there isn't at least one argument
to parse (or if malloc fails).
 */
commandline* get_commandline_arguments (int argc, char *argv[], int optind) {
  if (optind < argc) {
	// malloc the struct and the array
    commandline *c = malloc(sizeof(commandline));
	if (c == NULL) goto malloc_error;
	c->argv = malloc(argc * sizeof(string));
	if (argv == NULL) goto malloc_error;

	// find the number of arguments
	c->argc = argc - optind;

	// put the arguments into strings and into the array
	for(int i = 0; i < argc-optind; i++) {
	  c->argv[i] = argToString(argv[optind+i]);
	}

	return c;

  } else { // the user didn't give a command to execute
    fprintf(stderr, "Missing a command name.\n %s", usage_info);
    exit(1);
  }

 malloc_error:
  fprintf(stderr, "Malloc failure\n");
  exit(1);
}


int main(int argc, char * argv[]) {
  errno = 0;
  
  char * minutes_str = "*";
  char * hours_str = "*";
  char * daysofweek_str = "*";
  char * pipes_directory = NULL;
  
  uint16_t operation = CLIENT_REQUEST_LIST_TASKS;
  uint64_t taskid;
  commandline *command;
  int pipedes;

  int opt;
  char * strtoull_endp;
  while ((opt = getopt(argc, argv, "hlcqm:H:d:p:r:x:o:e:")) != -1) {
    switch (opt) {
    case 'm':
      minutes_str = optarg;
      break;
    case 'H':
      hours_str = optarg;
      break;
    case 'd':
      daysofweek_str = optarg;
      break;
    case 'p':
      pipes_directory = strdup(optarg);
      if (pipes_directory == NULL) goto error;
      break;
    case 'l':
      operation = CLIENT_REQUEST_LIST_TASKS;
      break;
    case 'c':
      operation = CLIENT_REQUEST_CREATE_TASK;
      break;
    case 'q':
      operation = CLIENT_REQUEST_TERMINATE;
      break;
    case 'r':
      operation = CLIENT_REQUEST_REMOVE_TASK;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'x':
      operation = CLIENT_REQUEST_GET_TIMES_AND_EXITCODES;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'o':
      operation = CLIENT_REQUEST_GET_STDOUT;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'e':
      operation = CLIENT_REQUEST_GET_STDERR;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'h':
      printf("%s", usage_info);
      return 0;
    case '?':
      fprintf(stderr, "%s", usage_info);
      goto error;
    }
  }

  /* if creating a task, get all the command arguments */
  if (operation == CLIENT_REQUEST_CREATE_TASK) {
	command = get_commandline_arguments(argc, argv, optind);
  }

  /* at this point in the code :
	 - operation contains the operation code
	 - if needed, taskid contains the id of the task
	 - if needed, command contains the command to create and its arguments
   */

    /* Needs a timing ? */
    //write_request(pipedes, operation, taskid,  , command);

    /* Prints on stdout and stderr are in the method  */
    //read_answer(pipedes, operation);



  return EXIT_SUCCESS;


  error:
    if (errno != 0) perror("main");
    free(pipes_directory);
    pipes_directory = NULL;
    return EXIT_FAILURE;
}

