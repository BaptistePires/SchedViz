#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "../module/monitor_blocked_time_common.h"

#define MAX_PATH_LEN 4096

static const char *debugfs_path_user = "/sys/kernel/debug/sched/user";
static const char *debugfs_path_kern = "/sys/kernel/debug/sched/kernel";
static const char *debugfs_path_other = "/sys/kernel/debug/sched/other";

static const char *out_file_fmt = "./out/%s/%s";

int main(int argc, char **argv)
{

    DIR *d;
    FILE *of_user, *of_kernel, *of_other, *of_meta, *curr_task;
    struct dirent *dir;
    int fd, ret;
    struct log_entry *buf;
    size_t i, c_user = 0, c_kernel = 0, c_other = 0;
    char path[4096];

    if (argc != 2) {
        fprintf(stderr, "Usage : %s <d_out>\n", argv[0]);
        return EXIT_FAILURE;
    }

    memset(path, 0, MAX_PATH_LEN);
    snprintf(path, MAX_PATH_LEN, out_file_fmt, argv[1], "user");
    of_user = fopen(path, "w");

    if (!of_user) {
        fprintf(stderr, "Can't open file %s\n", path);
        return EXIT_FAILURE;
    }

    memset(path, 0, MAX_PATH_LEN);
    snprintf(path, MAX_PATH_LEN, out_file_fmt, argv[1], "kernel");
    of_kernel = fopen(path, "w");

    if (!of_kernel) {
        fprintf(stderr, "Can't open file %s\n", path);
        return EXIT_FAILURE;
    }

    memset(path, 0, MAX_PATH_LEN);
    snprintf(path, MAX_PATH_LEN, out_file_fmt, argv[1], "other");
    of_other = fopen(path, "w");

    if (!of_other) {
        fprintf(stderr, "Can't open file %s\n", path);
        return EXIT_FAILURE;
    }

    ret = 0;
    buf = malloc(sizeof(struct log_entry) * BUFFER_SIZE);
    memset(buf, 0, BUFFER_SIZE * sizeof(struct log_entry));

    if (!buf) {
        perror("malloc");
        return EXIT_FAILURE;
    }


    // First read userspace threads
    d = opendir(debugfs_path_user);

    if (d == NULL) {
        perror("opendir");
        goto free_buf;
        return EXIT_FAILURE;
    }

    

    while (((dir = readdir(d)) != NULL)) {
        if (dir->d_name[0] == '.') continue;

        sprintf(path, "%s/%s", debugfs_path_user, dir->d_name);
        fd = open(path, O_RDONLY);

        if (!fd) {
            perror("open");
            ret = -1;
            goto free_buf;
        }

        memset(buf, 0, BUFFER_SIZE * sizeof(struct log_entry));
        read(fd, (char *)buf, BUFFER_SIZE * sizeof(struct log_entry));
        
        close(fd);
        
        for (i = 0; i < BUFFER_SIZE && buf[i].duration; i++) {
            fprintf(of_user, "%lu ", buf[i].duration);
        }

        sprintf(path, "./out/%s/tasks/user/%s", argv[1], dir->d_name);
        curr_task = fopen(path, "w");
        if (!curr_task) {
            fprintf(stderr, "Can't open file %s\n", path);
            c_user++;
            continue;
        }
        for (i = 0; i < BUFFER_SIZE && buf[i].duration; i++) {
            fprintf(curr_task, "%lu ", buf[i].wakeup_date);
        }
        fprintf(curr_task, "\n");
        for (i = 0; i < BUFFER_SIZE && buf[i].duration; i++) {
            fprintf(curr_task, "%lu ", buf[i].duration);
        }
        fclose(curr_task);

        c_user++;            
    }

    // Now read kernel threads
    d = opendir(debugfs_path_kern);

    if (d == NULL) {
        perror("opendir");
        goto free_buf;
        return EXIT_FAILURE;
    }

    

    while (((dir = readdir(d)) != NULL)) {
        if (dir->d_name[0] == '.') continue;

        sprintf(path, "%s/%s", debugfs_path_kern, dir->d_name);
        fd = open(path, O_RDONLY);

        if (!fd) {
            perror("open");
            ret = -1;
            goto free_buf;
        }

        memset(buf, 0, BUFFER_SIZE * sizeof(struct log_entry));
        read(fd, (char *)buf, BUFFER_SIZE * sizeof(struct log_entry));
        close(fd);

        for (i = 0; i < BUFFER_SIZE && buf[i].duration; i++){
            fprintf(of_kernel, "%lu ", buf[i].duration);
        }

        sprintf(path, "./out/%s/tasks/kernel/%s", argv[1], dir->d_name);
        curr_task = fopen(path, "w");
        if (!curr_task) {
            fprintf(stderr, "Can't open file %s\n", path);
            c_kernel++;
            continue;
        }
        for (i = 0; i < BUFFER_SIZE && buf[i].duration; i++) {
            fprintf(curr_task, "%lu ", buf[i].wakeup_date);
        }
        fprintf(curr_task, "\n");
        for (i = 0; i < BUFFER_SIZE && buf[i].duration; i++) {
            fprintf(curr_task, "%lu ", buf[i].duration);
        }
        fclose(curr_task);

        c_kernel++;            
    }

    // Now read others threads
    d = opendir(debugfs_path_other);

    if (d == NULL) {
        perror("opendir");
        goto free_buf;
        return EXIT_FAILURE;
    }

    

    while (((dir = readdir(d)) != NULL)) {
        if (dir->d_name[0] == '.') continue;

        sprintf(path, "%s/%s", debugfs_path_other, dir->d_name);
        fd = open(path, O_RDONLY);

        if (!fd) {
            perror("open");
            ret = -1;
            goto free_buf;
        }

        memset(buf, 0, BUFFER_SIZE * sizeof(struct log_entry));
        read(fd, (char *)buf, BUFFER_SIZE * sizeof(struct log_entry));
        close(fd);

        for (i = 0; i < BUFFER_SIZE && buf[i].duration; i++){
            fprintf(of_other, "%lu ", buf[i].duration);
        }
        sprintf(path, "./out/%s/tasks/other/%s", argv[1], dir->d_name);
        curr_task = fopen(path, "w");
        if (!curr_task) {
            fprintf(stderr, "Can't open file %s\n", path);
            c_other++;
            continue;
        }
        for (i = 0; i < BUFFER_SIZE && buf[i].duration; i++) {
            fprintf(curr_task, "%lu ", buf[i].wakeup_date);
        }
        fprintf(curr_task, "\n");
        for (i = 0; i < BUFFER_SIZE && buf[i].duration; i++) {
            fprintf(curr_task, "%lu ", buf[i].duration);
        }
        fclose(curr_task);

        c_other++;
            
    }

    memset(path, 0, MAX_PATH_LEN);
    snprintf(path, MAX_PATH_LEN, out_file_fmt, argv[1], "meta");
    of_meta = fopen(path, "a");

    if(!of_meta) {
        fprintf(stderr, "Error opening meta file..\n");
        goto free_buf;
    }
    printf("here\n");
    fprintf(of_meta, "user %lu\n", c_user);
    fprintf(of_meta, "kernel %lu\n", c_kernel);
    fprintf(of_meta, "other %lu\n", c_other);

    fclose(of_meta);



free_buf:
    free(buf);

    fclose(of_user);
    fclose(of_kernel);
    fclose(of_other);

    return ret;
}
