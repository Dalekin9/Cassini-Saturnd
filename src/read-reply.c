//
///* Reads from the reply pipe when a response to -l ; returns a task list */
//task **read_list_task(int pipedes, long taskNb){
//    task **tasks = malloc(sizeof(**tasks));
//    for(long i = 0; i < taskNb; i++){
//        task a_task;
//        long id;
//        uint64_t minutes;
//        uint32_t hours;
//        uint8_t daysofweek;
//
//        read_err(read(pipedes,&id, 64));
//        a_task.taskid = id;
//
//        read_err(read(pipedes,&minutes, 64));
//        read_err(read(pipedes,&hours, 32));
//        read_err(read(pipedes,&daysofweek, 8));
//        struct timing time = {.minutes = minutes, .hours = hours, .daysofweek = daysofweek};
//        a_task.t = &time;
//
//        commandline cmd;
//        uint32_t argc;
//        read_err(read(pipedes, &argc, 32));
//        cmd.argc = argc;
//
//        string *args = malloc(sizeof(*args));
//        for(long j = 0; j < argc; j++){
//            uint32_t length;
//            BYTE arg;
//            read_err(read(pipedes, &length, 32));
//            read_err(read(pipedes, &arg, length));
//            string str = {.length = length, .s = &arg};
//            args[j] = str;
//        }
//        cmd.argv = &args;
//        a_task.command = cmd;
//        tasks[i] = &a_task;
//    }
//    return tasks;
//}
//
///* Reads and returns the ID of the task that has been created */
//uint64_t read_create(int pipedes){
//    uint64_t taskID;
//    read_err(read(pipedes, &taskID, 64));
//    return taskID;
//}
//
///* Gathers the list of past runs from the reply pipe and returns it */
//run **read_times_exitcode(int pipedes,long runNb){
//    run **runs = malloc(sizeof(**runs));
//    for (int i = 0; i < runNb; ++i) {
//        int64_t time;
//        uint16_t exitcode;
//        read_err(read(pipedes, &time, 64));
//        read_err(read(pipedes, &exitcode, 16));
//        run a_run = {.time = time, .exitcode = exitcode};
//        *runs[i] = a_run;
//    }
//    return runs;
//}
//
//
///* Reads from the reply pipe and prints the result depending on the request */
//void read_answer(int pipedes,uint16_t ope ) {
//    uint16_t rep;
//    uint64_t id;
//    long taskNb;
//    long runNb;
//    string a = {0, 0};
//    read_err(read(pipedes, &rep, 16));
//    switch (ope) {
//        case CLIENT_REQUEST_LIST_TASKS:
//            read_err(read(pipedes, &taskNb, 32));
//            task **tasks = read_list_task(pipedes, taskNb);
//            print_response(ope, rep, 0, taskNb, 0, tasks, 0, a);
//            break;
//        case CLIENT_REQUEST_CREATE_TASK:
//            id = read_create(pipedes);
//            print_response(ope, rep, 0, 0, id, 0, 0, a);
//            break;
//        case CLIENT_REQUEST_TERMINATE:
//            print_response(ope, rep, 0, 0, 0, 0, 0, a);
//            break;
//        case CLIENT_REQUEST_REMOVE_TASK:
//            if (rep == SERVER_REPLY_OK) {
//                print_response(ope, rep, 0, 0, 0, 0, 0, a);
//                break;
//            } else {
//                uint16_t err;
//                read_err(read(pipedes, &err, 16)); //ERRCODE
//                print_response(ope, rep, err, 0, 0, 0, 0, a);
//                break;
//            }
//        case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
//            if (rep == SERVER_REPLY_OK) {
//                read_err(read(pipedes, &runNb, 32));
//                run **runs = read_times_exitcode(pipedes, runNb);
//                print_response(ope, rep, 0, runNb, 0, 0, runs, a);
//                break;
//            } else {
//                uint16_t err;
//                read_err(read(pipedes, &err, 16)); //ERRCODE
//                print_response(ope, rep, err, 0, 0, 0, 0, a);
//                break;
//            }
//        case CLIENT_REQUEST_GET_STDOUT:
//        case CLIENT_REQUEST_GET_STDERR:
//            if (rep == SERVER_REPLY_OK) {
//                uint32_t length;
//                BYTE str;
//                read_err(read(pipedes, &length, 32));
//                read_err(read(pipedes, &str, length));
//                print_response(ope, rep, 0, 0, 0, 0, 0, str);
//                break;
//            } else {
//                uint16_t err;
//                read_err(read(pipedes, &err, 16)); //ERRCODE
//                print_response(ope, rep, err, 0, 0, 0, 0, a);
//                break;
//            }
//        default:
//            perror("?");
//            exit(1);
//    }
//}