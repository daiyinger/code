#include <stdio.h>


//该代码主要为了尝试访问未定义的变量的地址是否会出现段错误
//查找simple_tcp视频传输工程 jpg转rgb运行故障的原因

static  long iclip[1024];   
static  long *iclp;
int main(int argc,char *argv[])
{
int
x0 = -1265650,
x1 = 21756099,
x2 = -4262567,
x3 = 1911282,
x4 = 7269452,
x5 = -31884341,
x6 = 31914851,
x7 = 3262014,
x8 = -3653694;
printf("%d\n",(x7+x1)>>14);
printf("%d\n",(x3+x2)>>14);
printf("%d\n",(x0+x4)>>14);
printf("%d\n",(x8+x6)>>14);
printf("%d\n",(x8-x6)>>14);
printf("%d\n",(x0-x4)>>14);
printf("%d\n",(x3-x2)>>14);
printf("%d\n",(x7-x1)>>14);
iclp = iclip + 512;
printf("out\n");
printf("%d\n",iclp[(x7+x1)>>14]);
printf("%d\n",iclp[(x3+x2)>>14]);
printf("%d\n",iclp[(x0+x4)>>14]);
printf("%d\n",iclp[(x8+x6)>>14]);
printf("%d\n",iclp[(x8-x6)>>14]);
printf("%d\n",iclp[(x0-x4)>>14]);
printf("%d\n",iclp[(x3-x2)>>14]);
printf("%d\n",iclp[(x7-x1)>>14]);
return 0;
}
