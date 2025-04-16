// Benjamin Eder, 210894

#include "worker.h"

#include <errno.h>
#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"

void send_result_msg(mqd_t done_mq, size_t worker_id, pid_t pid,
                     size_t task_count, size_t total_work_done) {
    ssize_t sent;
    do {
        DoneMsg done_msg = {worker_id, pid, task_count, total_work_done};
        sent = mq_send(done_mq, (void*)&done_msg, sizeof(DoneMsg), 3);
    } while (sent == -1 && errno == EINTR);
}

void start_polling(size_t worker_id, mqd_t cmd_mq, mqd_t done_mq) {
    size_t task_count = 0;
    size_t total_work_done = 0;

    bool termination_received = false;
    do {
        ssize_t received;
        CmdMsg task;
        do {
            received = mq_receive(cmd_mq, (void*)&task, sizeof(CmdMsg), NULL);
        } while (received == -1 && errno == EINTR);

        if (task.effort == 0) {
            print_time();
            printf(" | Worker #%02ld | Received termination task\n", worker_id);

            termination_received = true;
        } else {
            print_time();
            printf(" | Worker #%02ld | Received task with effort %ld\n",
                   worker_id, task.effort);

            // doing some really important work
            sleep(task.effort);

            task_count++;
            total_work_done += task.effort;
        }

    } while (!termination_received);

    send_result_msg(done_mq, worker_id, getpid(), task_count, total_work_done);

    mq_close(cmd_mq);
    mq_close(done_mq);

    exit(EXIT_SUCCESS);
}

pid_t start_worker(size_t worker_id, char* cmd_mq_name, char* done_mq_name) {
    pid_t pid = fork();

    if (pid == 0) {
        // is worker process
        print_time();
        printf(" | Worker #%02ld | Started worker PID %d\n", worker_id,
               getpid());

        mqd_t cmd_mq = mq_open(cmd_mq_name, O_RDONLY);
        mqd_t done_mq = mq_open(done_mq_name, O_WRONLY);

        start_polling(worker_id, cmd_mq, done_mq);
    }

    return pid;
}