#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define FILENAME "/dev/processlist"
#define BUF_LEN 10000

int main() {
    int fd = open(FILENAME, O_RDONLY);
    char processes[BUF_LEN];
    while (read(fd, processes, BUF_LEN) > 0);
    close(fd);
    printf("%s", processes);
    return 0;
}