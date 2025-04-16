// Benjamin Eder, 210894

#include <stdio.h>
#include <time.h>

void print_time() {
    time_t t = time(NULL);

    int hours = (t / 3600) % 24;
    int minutes = (t / 60) % 60;
    int seconds = t % 60;

    printf("%02d:%02d:%02d", hours, minutes, seconds);
}

void log_action(char* name, char* msg) {
    time_t t = time(NULL);

    int hours = (t / 3600) % 24;
    int minutes = (t / 60) % 60;
    int seconds = t % 60;

    printf("%02d:%02d:%02d | %s | %s\n", hours, minutes, seconds, name, msg);
}