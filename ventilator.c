// Benjamin Eder, 21894

#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cli_args.h"
#include "log.h"
#include "worker.h"

#define CMD_MQ_NAME "/cmd_mq_210894"
#define DONE_MQ_NAME "/done_mq_210894"

int main(int argc, char* argv[]) {
    CliArgs args = parse_cli_args(argc, argv);

    // setup msg queues

    print_time();
    printf(
        " | Ventilator | "
        "Starting %ld workers for %ld tasks and a queue size of %ld\n",
        args.workers, args.tasks, args.queue_size);

    struct mq_attr cmd_queue_options = {
        .mq_maxmsg = args.queue_size,
        .mq_msgsize = sizeof(CmdMsg),
    };

    mqd_t command_queue =
        mq_open(CMD_MQ_NAME, O_WRONLY | O_CREAT, S_IRWXU, &cmd_queue_options);

    struct mq_attr done_queue_options = {
        .mq_maxmsg = 1,
        .mq_msgsize = sizeof(DoneMsg),
    };

    mqd_t done_queue =
        mq_open(DONE_MQ_NAME, O_RDONLY | O_CREAT, S_IRWXU, &done_queue_options);

    // start workers
    for (int i = 0; i < args.workers; i++) {
        start_worker(i + 1, CMD_MQ_NAME, DONE_MQ_NAME);
    }

    print_time();
    printf(" | Ventilator | Distributing tasks\n");

    // send actual tasks
    for (int i = 0; i < args.tasks; i++) {
        CmdMsg cmd_msg = {.effort = 3 + rand() % 8};

        print_time();
        printf(" | Ventilator | Queuing task #%d with effort %ld\n", i + 1,
               cmd_msg.effort);

        ssize_t sent;
        do {
            sent = mq_send(command_queue, (void*)&cmd_msg, sizeof(CmdMsg), 2);
        } while (sent == -1 && errno == EINTR);

        if (sent == -1) {
            fprintf(stderr, "[ERR; Ventilator] Failed to send instructions\n");
            return EXIT_FAILURE;
        }
    }

    print_time();
    printf(" | Ventilator | Sending termination tasks\n");

    // send terminate tasks to each worker
    for (int i = 0; i < args.workers; i++) {
        CmdMsg cmd_msg = {.effort = 0};  // 0 --> termination task
        ssize_t sent;

        do {
            // send msg with lower prio to ensure actual tasks are done
            // first
            sent = mq_send(command_queue, (void*)&cmd_msg, sizeof(CmdMsg), 1);
        } while (sent == -1 && errno == EINTR);

        if (sent == -1) {
            fprintf(stderr,
                    "[ERR; Ventilator] Failed to send terminate instruction\n");
            return EXIT_FAILURE;
        }
    }

    print_time();
    printf(" | Ventilator | Waiting for workers to terminate\n");

    // receive done msgs from workers
    for (int i = 0; i < args.workers; i++) {
        ssize_t received;
        DoneMsg done_msg;
        do {
            received =
                mq_receive(done_queue, (void*)&done_msg, sizeof(DoneMsg), NULL);
        } while (received == -1 && errno == EINTR);

        if (received == -1) {
            fprintf(stderr,
                    "[ERR; Ventilator] Failed to receive done instruction "
                    "(ERRNO %d)\n",
                    errno);
            return EXIT_FAILURE;
        }

        print_time();
        printf(
            " | Ventilator | Worker %ld processed %ld tasks in %ld seconds\n",
            done_msg.worker_id, done_msg.task_count, done_msg.total_work_done);

        // after receiving done msg, wait for that worker to exit

        int wstatus = 0;
        pid_t waited;
        do {
            waited = waitpid(done_msg.pid, &wstatus, 0);
        } while (waited == -1 && errno == EINTR);

        if (WIFEXITED(wstatus)) {
            print_time();
            printf(
                " | Ventilator | "
                "Worker %ld with PID %d exited with status %d\n",
                done_msg.worker_id, waited, WEXITSTATUS(wstatus));
        } else {
            fprintf(
                stderr,
                "[ERR; Ventilator] Worker with pid %d didn't exit normally\n",
                waited);
            return EXIT_FAILURE;
        }
    }

    // delete msg-queues
    mq_close(command_queue);
    mq_unlink(CMD_MQ_NAME);

    mq_close(done_queue);
    mq_unlink(DONE_MQ_NAME);

    return EXIT_SUCCESS;
}