// Benjamin Eder, 210894

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char* argv[]) {
    CliArgs const args = parse_cli_args(argc, argv);
    printf("i: %d, s: %s, b: %i\n", args.i, args.s, args.b);

    return 0;
}