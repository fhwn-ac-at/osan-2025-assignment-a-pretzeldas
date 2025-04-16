#pragma once

// Benjamin Eder, 21894

#include <fcntl.h>
#include <mqueue.h>
#include <sys/types.h>

typedef struct DoneMsg {
    size_t worker_id;
    pid_t pid;
    size_t task_count;
    size_t total_work_done;
} DoneMsg;

typedef struct CmdMsg {
    size_t effort;  // 0 -> stop working and send done msg
} CmdMsg;

pid_t start_worker(size_t worker_id, char* cmd_mq_name, char* done_mq_name);