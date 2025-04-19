#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "../module/monitor_blocked_time_common.h"

int main(int argc, char **argv)
{
    pid_t child;
    int status;
    if ((child = fork())) {
        wait(&status);
    } else {
        FILE *fptr = fopen("/sys/kernel/debug/sched/add", "w");
        char buf[12];
        pid_t parent = getppid();
        snprintf(buf, 12, "%d\n", parent);
        fwrite(buf, strlen(buf), 1, fptr);
        fclose(fptr);
        exit(0);
    }   


    if((child = fork()) == 0) {
        if (execv(argv[1], &argv[1])) {
            perror(argv[1]);
            return -1;
        }
        printf("lol\n");
    } else
        wait(&status);
    return EXIT_SUCCESS;
}
