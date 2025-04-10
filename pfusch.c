// Benjamin Eder, 210894

#include <fcntl.h>
#include <getopt.h>
#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <threads.h>
#include <unistd.h>

struct CliArgs {
    int i;
    char const* s;
    bool b;
};

typedef struct CliArgs CliArgs;

CliArgs parse_cli_args(int argc, char* argv[]) {
    char* shortopts = "i:s:b";

    CliArgs args = {0, NULL, false};

    int optgot = -1;
    do {
        optgot = getopt(argc, argv, shortopts);

        switch (optgot) {
            case 'i':
                args.i = atoi(optarg);
                break;
            case 's':
                args.s = optarg;
                break;
            case 'b':
                args.b = true;
                break;
            case '?':
                printf("Usage: %s -i <number> -s <string> -b\n", argv[0]);
                exit(EXIT_FAILURE);
        }

    } while (optgot != -1);

    if (args.i <= 0 || strlen(args.s) < 5) {
        printf("Usage: %s -i <number> -s <string> -b\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return args;
}

struct work_msg {
    int work;
};

int child_labour() {
    mqd_t command_queue = mq_open("/mq_210894", O_RDONLY);
    printf("[%d] mq_open returned %d\n", getpid(), command_queue);
    if (command_queue == -1) {
        fprintf(stderr, "Failed to open msg queue\n");
        return EXIT_FAILURE;
    }

    printf("[%d] Waiting for instructions\n", getpid());
    struct work_msg instructions;
    int received = mq_receive(command_queue, (void*)&instructions,
                              sizeof(struct work_msg), NULL);
    if (received == -1) {
        fprintf(stderr, "Failed to reveice instructions\n");
        return EXIT_FAILURE;
    }

    printf("[%d] Received msg of size %ld bytes: work to do %d\n", getpid(),
           sizeof(instructions), instructions.work);

    printf("[%d] Doing some work for (%d)...\n", getpid(), getppid());

    sleep(instructions.work);

    printf("[%d] Job's done!\n", getpid());
    printf("[%d] Bringing coal to %d...\n", getpid(), getppid());

    mq_close(command_queue);

    return getpid();
}

int main(int argc, char* argv[]) {
    // CliArgs const args = parse_cli_args(argc, argv);
    // printf("i: %d, s: %s, b: %i\n", args.i, args.s, args.b);

    const int CHILD_COUNT = 10;

    struct mq_attr queue_options = {.mq_maxmsg = 1,
                                    .mq_msgsize = sizeof(struct work_msg)};
    mqd_t command_queue =
        mq_open("/mq_210894", O_WRONLY | O_CREAT, S_IRWXU, &queue_options);

    printf("[%d] mq_open returned %d\n", getpid(), command_queue);
    if (command_queue == -1) {
        printf("Failed to open msg queue!\n");
        return EXIT_FAILURE;
    }

    printf("[%d] Sending a child into the mines...\n", getpid());

    for (int i = 0; i < CHILD_COUNT; i++) {
        pid_t forked = fork();
        if (forked == 0) {
            return child_labour();
        }
    }

    for (int i = 0; i < CHILD_COUNT; i++) {
        // send instructions to children
        struct work_msg instructions = {3 + rand() % 7};
        int sent = mq_send(command_queue, (void*)&instructions,
                           sizeof(instructions), 0);
        if (sent == -1) {
            fprintf(stderr, "Failed to send instructions\n");
            return EXIT_FAILURE;
        }
    }

    printf("[%d] Successfully sent instructions to child\n", getpid());

    printf("[%d] Enjoying some brandy...\n", getpid());
    printf("[%d] Where the fudge is my coal???\n", getpid());

    for (int i = 0; i < CHILD_COUNT; i++) {
        int wstatus = 0;
        pid_t const waited = wait(&wstatus);

        if (WIFEXITED(wstatus)) {
            printf("[%d] Child %d exited normally with return code %d\n",
                   getpid(), waited, WEXITSTATUS(wstatus));
        } else if (WIFSIGNALED(wstatus)) {
            printf("[%d] Child %d terminated by signal %d\n", getpid(), waited,
                   WTERMSIG(wstatus));
        } else {
            printf("[%d] Child %d terminated abnormally\n", getpid(), waited);
        }

        printf("[%d] Wait returned %d, status is %d\n", getpid(), waited,
               wstatus);
    }

    printf("All children have returned\n");

    mq_close(command_queue);
    mq_unlink("/mq_210894");
    return 0;
}