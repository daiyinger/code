#include <stdio.h>
#include <stdlib.h>

int main(int argc ,char *argv[])
{
    int ret;
    ret = system("ping www.baidu.com -c 1 >/dev/null");
    printf("ret = %d\n",ret);
    ret = system("ssh pi@daiyinger.nat123.net -p 50250 >/dev/null");
    printf("ret = %d\n",WIFEXITED(ret));
    return 0;
}
