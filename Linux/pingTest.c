#include <stdio.h>
#include <stdlib.h>

int main(int argc ,char *argv[])
{
    int ret;
    ret = system("ping www.baidu.com -c 1 >/dev/null");
    printf("ret = %d\n",ret);
    ret = system("ping 192.168.125.1 -c 1 >/dev/null");
    printf("ret = %d\n",WIFEXITED(ret));
    return 0;
}
