#include <time.h>

int main(int argc, char **argv)
{
    struct timespec ts;
    ts.tv_sec = 1;
    nanosleep(&ts, NULL);
    return 0;
}