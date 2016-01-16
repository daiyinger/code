#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    struct stat buf;
    while(1)
    {
        stat("1234.264", &buf);
        printf("1234.264 file stat = %d\n", buf.st_nlink);
        sleep(1);
    }
    return 0;
}

