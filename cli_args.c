// Benjamin Eder, 210894

#include "cli_args.h"

#include <getopt.h>
#include <stdio.h>

CliArgs parse_cli_args(int argc, char* argv[]) {
    char* shortopts = "w:t:s:";

    size_t workers, tasks, queue_size;

    int optgot = -1;
    do {
        optgot = getopt(argc, argv, shortopts);

        switch (optgot) {
            case 'w':
                workers = atoi(optarg);
                break;
            case 't':
                tasks = atoi(optarg);
                break;
            case 's':
                queue_size = atoi(optarg);
                break;
            case '?':
                printf("Usage: %s -w <workers> -t <tasks> -s <queue_size>\n",
                       argv[0]);
                exit(EXIT_FAILURE);
        }

    } while (optgot != -1);

    if (workers <= 0 || tasks <= 0 || queue_size <= 0) {
        printf("Usage: %s -w <workers> -t <tasks> -s <queue_size>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    CliArgs args = {
        .workers = workers,
        .tasks = tasks,
        .queue_size = queue_size,
    };

    return args;
}
