// Benjamin Eder, 210894

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
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

int child_labour() {
    // DO SOME WORK
    printf("[%d] Doing some work for (%d)...\n", getpid(), getppid());

    srand(getpid());
    sleep(rand() % 5);

    printf("[%d] Job's done!\n", getpid());
    printf("[%d] Bringing coal to %d...\n", getpid(), getppid());

    return getpid();
}

int main(int argc, char* argv[]) {
    // CliArgs const args = parse_cli_args(argc, argv);
    // printf("i: %d, s: %s, b: %i\n", args.i, args.s, args.b);

    printf("[%d] Sending a child into the mines...\n", getpid());

    for (int i = 0; i < 10; i++) {
        pid_t forked = fork();
        if (forked == 0) {
            return child_labour();
        }
    }

    printf("[%d] Enjoying some brandy...\n", getpid());
    printf("[%d] Where the fudge is my coal???\n", getpid());

    for (int i = 0; i < 10; i++) {
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

    return 0;
}