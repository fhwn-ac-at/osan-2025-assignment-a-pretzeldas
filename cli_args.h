#pragma once

#include "stdbool.h"
#include "stdlib.h"

typedef struct CliArgs {
    size_t workers;
    size_t tasks;
    size_t queue_size;
} CliArgs;

CliArgs parse_cli_args(int argc, char* argv[]);