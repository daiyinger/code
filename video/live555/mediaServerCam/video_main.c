#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <linux/videodev2.h>
#include <asm/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include "simple_code.h"
//#include "fscale.h"
//#include "tcpTool.h"
#include "tinyjpeg.h"

typedef struct VideoBuffer
{
    void *start;
    size_t length;
}VideoBuffer;
typedef struct data
{
    unsigned int datasize;
    char buf[];
}buf_t;
buf_t *databuf;
static VideoBuffer *buffers = NULL;
pthread_mutex_t g_lock,g_lock1;
pthread_cond_t g_cond;
int exitFlag = 0;
int fd;

//设置视频的制式及格式
int setMark(void)
{
    int ret;
    struct v4l2_capability cap;		//获取视频设备的功能
    struct v4l2_format fmt;			//设置视频的格式

    do
    {
        ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);	//查询驱动功能
    }while(ret == -1 && errno == EAGAIN);

    if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)	//查询视频设备是否支持数据流类型
    {
        printf("capability is V4L2_CAP_VIDEO_CAPTURE\n");
    }
    
    memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//数据流类型
    fmt.fmt.pix.width = 320;//宽，必须是16的倍数
    fmt.fmt.pix.height = 240;//高，必须是16的倍数
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;//=V4L2_PIX_FMT_YUYV;// 视频数据存储类型
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) <0)	//设置摄像头视频格式
    {
        printf("set format failed\n");
        return -1;
    }
    return 0;
}

//申请物理内存
int AllocMem(void)
{
	int numBufs=0;
	struct v4l2_requestbuffers  req;	//分配内存
	struct v4l2_buffer buf;				//V4L2视频Buffer

	req.count=4;											//缓存数量
	req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;					//缓存类型 数据流
	req.memory=V4L2_MEMORY_MMAP;							//
	buffers=calloc(req.count,sizeof(VideoBuffer));

	if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) 
	{
		return -1;
	}

	for(numBufs=0;numBufs<req.count;numBufs++)
	{
		memset(&buf,0,sizeof(buf));

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = numBufs;

		if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1)//读取缓存信息 
		{
            printf("VIDIOC_QUERYBUF error\n");
            return -1;
		}
		buffers[numBufs].length = buf.length;
		buffers[numBufs].start = mmap(NULL,buf.length,
						            PROT_READ | PROT_WRITE,
						            MAP_SHARED,fd, buf.m.offset);//转换成相对地址
		if(buffers[numBufs].start == MAP_FAILED)
		{
			return -1;
		}
		if(ioctl(fd,VIDIOC_QBUF,&buf) == -1)//放入缓存队列
		{
			return -1;
		}
	}
}

//开启视频采集
int video_on()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;   
    if (ioctl (fd, VIDIOC_STREAMON, &type) < 0) 
    {
        printf("VIDIOC_STREAMON error\n");
        return -1;
    }
}

void video_off(void);
FILE *fps;
//视频采集线程函数
void *pthread_video(int arg)
{
    pthread_detach(pthread_self());
    video_on();
    databuf = (buf_t *)malloc(sizeof(buf_t)+buffers[0].length);
    video(arg);
    video_off();

    return NULL;
}
unsigned char tbuffers[1024*1024*2];
unsigned char Sendbufs[1024*500];
unsigned char FirstFramebufs[1024*500];
int FirstFrameLen = 0;
int g_encodeDataLen = 0;
unsigned char read_flag = 0;
unsigned int delayCnt = 0;
unsigned int firstFrameCnt = 0;
//unsigned char data[8][1024*100];
static int FindStartCode2 (unsigned char *Buf)
{
    if (Buf[0] != 0 || Buf[1] != 0 || Buf[2] != 1)
    {
	return 0; //判断是否为0x000001,如果是返回1
    }
    else
    {
	return 1;
    }
}

static int FindStartCode3 (unsigned char *Buf)
{
    if (Buf[0] != 0 || Buf[1] != 0 || Buf[2] != 0 || Buf[3] != 1)
    {
	return 0;//判断是否为0x00000001,如果是返回1
    }
    else
    {
	return 1;
    }
}
void *pthread_encode(void *arg)
{
    int cnt = 0;
    unsigned char *ptrSendDataBuf;   //发送Buffer数据指针
    int encodeDatalen = 0;

    ptrSendDataBuf = Sendbufs + 4;  //发送的报文是4个字节的头加数据

    pthread_detach(pthread_self());
    fprintf(stderr,"start!\n");
    pthread_mutex_lock(&g_lock);
    while(exitFlag == 0)
    {
        pthread_mutex_lock(&g_lock);
        cnt++;
        fprintf(stderr,". ");
	clock_t clockStart = clock();
	if(jpg2yuv(databuf->buf, databuf->datasize, tbuffers) != 0)
	{
	    fprintf(stderr,"jpg2rgb error!\n");
	}
	else
	{ 
	    //fprintf(stderr,"- %d ",(unsigned int)(clock()-clockStart));
	    //printf("start\n");
	    /*encode_one_frame(tbuffers, ptrSendDataBuf, &encodeDatalen);
	    unsigned int diff = (unsigned int)(clock()-clockStart);
	    printf("- %d \n",diff/1000);
	    g_encodeDataLen = encodeDatalen;*/
	    if(firstFrameCnt < 1)
	    {
#if 1
		unsigned char data[8][1024*50];
		unsigned int len[8]={0};
		int i = 0;
		printf("len %d\n",sizeof(data[0]));
		encode_one_frameExt(tbuffers,(unsigned char **)data,sizeof(data[0]), len);
		i = 0;
		while((len[i] != 0) &&(i < 8))
		{
		    if(FindStartCode2(data[i]))
		    {
			printf("Type %d\n",data[i][3]&0x1F);
			if((data[i][3]&0x1F) == 0x05)
			{
			    data[i][3] &= 0xE0;
			    data[i][3] |= 0x05;
			}
		    }	
		    if(FindStartCode3(data[i]))
		    {
			printf("type %d\n",data[i][4]&0x1F);
			 if((data[i][4]&0x1F) == 0x05)
			 {
			    data[i][4] &= 0xE0;
			    data[i][4] |= 0x05;
                         }

		    }
		    memcpy(FirstFramebufs+FirstFrameLen, data[i],len[i]);
	            FirstFrameLen += len[i];
		    printf("FirstFrameLen %d\n",FirstFrameLen);
		    i++;
		}
#else
		memcpy(FirstFramebufs+FirstFrameLen, ptrSendDataBuf, encodeDatalen);
	        FirstFrameLen += encodeDatalen;
		printf("FirstFrameLen %d\n",FirstFrameLen);
#endif
		firstFrameCnt++;
	    }
	    else
	    {
		encode_one_frame(tbuffers, ptrSendDataBuf, &encodeDatalen);
		unsigned int diff = (unsigned int)(clock()-clockStart);
		printf("- %d \n",diff/1000);
		g_encodeDataLen = encodeDatalen;
		read_flag = 1;
	    }
	    //SendData(Sendbufs, encodeDatalen, NORMAL_PACK);
	}
        pthread_mutex_unlock(&g_lock1);
    }
    usleep(100);
    //SendData(Sendbufs, 0, END_PACK);
}
unsigned char rtsp_buf[1024*1024];
unsigned int rtspDataRemainLen = 0;
int readOneFrame(unsigned char *buf, int want_size, unsigned char flag)
{
    unsigned char *ptrSendDataBuf;   //发送Buffer数据指针
    int data_len;
    ptrSendDataBuf = Sendbufs + 4;  //发送的报文是4个字节的头加数据
    if(read_flag > 0)
    {
#if 0
	memcpy(buf, ptrSendDataBuf, g_encodeDataLen);
	read_flag--;
	//printf("g_encodeDataLen = %d\n",g_encodeDataLen);
	return g_encodeDataLen;
#else	
	if((flag == 1) && (FirstFrameLen > 0))
	{
	    memcpy(buf,FirstFramebufs,FirstFrameLen);
	    return FirstFrameLen;
	}
	data_len = rtspDataRemainLen + g_encodeDataLen;
	if(want_size < data_len)
	{
	    if(rtspDataRemainLen < want_size)
	    {
		if( rtspDataRemainLen > 0)
		{	
		    memcpy(buf, rtsp_buf, rtspDataRemainLen);
		}
		memcpy(buf+rtspDataRemainLen, ptrSendDataBuf, 
		    want_size-rtspDataRemainLen);
		memcpy(rtsp_buf, ptrSendDataBuf + \
		    (want_size-rtspDataRemainLen),g_encodeDataLen\
		    -(want_size-rtspDataRemainLen));
		rtspDataRemainLen = 
		    g_encodeDataLen-(want_size-rtspDataRemainLen);
		 fprintf(stderr,"\n1 rtspDataRemainLen %d want %d\n",rtspDataRemainLen, want_size);
	    }
	    else
	    {
		memcpy(buf, rtsp_buf, want_size);
		memcpy(rtsp_buf, rtsp_buf+want_size,
		    rtspDataRemainLen - want_size);
		memcpy(rtsp_buf+rtspDataRemainLen-want_size,ptrSendDataBuf,
		    g_encodeDataLen);
		rtspDataRemainLen = rtspDataRemainLen - want_size 
		    + g_encodeDataLen;
		fprintf(stderr,"\n 2 rtspDataRemainLen %d\n",rtspDataRemainLen);
	    }
	    read_flag--;
	    return want_size;
	}
	else
	{
	    if( rtspDataRemainLen > 0)
            {
		 memcpy(buf, rtsp_buf, rtspDataRemainLen);
	    }
	    memcpy(buf+rtspDataRemainLen, ptrSendDataBuf,
		g_encodeDataLen);
	    read_flag--;
	    rtspDataRemainLen = 0;
	    return data_len;
	}
#endif
    }
    else
    {
	if(delayCnt > 0)
	    delayCnt--;
	return 0;
    }
}
//视频采集循环函数
int video(int num)
{
    struct v4l2_buffer buf;
    memset(&buf,0,sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    num = 1000000;
    int cnt = 0;
    while(num > 0)
    {
        num--;
        
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1)
        {
            return -1;
        }
        //fprintf(stderr,"sample one frame!\n");
        pthread_mutex_lock(&g_lock1);
        memcpy(databuf->buf, buffers[buf.index].start, buffers[buf.index].length);
        databuf->datasize = buf.bytesused;
        //pthread_cond_signal(&g_cond);
        //if(cnt > 0)
        while(read_flag == 1)
	{
	    usleep(1000);
	}
	pthread_mutex_unlock(&g_lock);
	cnt++;
        if(ioctl(fd,VIDIOC_QBUF,&buf) == -1)
        {
            return -1;
        }
    }
    exitFlag = 1;
    fprintf(stderr,"\n");
    return 0;
}
//关闭视频采集
void video_off()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    int ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
    if(ret == -1)
    {
    	printf("vidio OFF error!\n");
    }
	 if(buffers != NULL)
	  {
		free(buffers);
	  }
    close(fd);
}
void my_handler(int s)
{  
    printf("Caught signal %d\n",s);  
    //video_off();
    //tcpToolEnd();
    exit(1);   
} 
void *video_init(void *arg)
{
    int ret;
    int num;

    pthread_t thread_enc;
     
    pthread_detach(pthread_self());
    //signal(SIGPIPE,SIG_IGN);
    //fd=open("dev/video0",O_RDWR | O_NONBLOCK,0);
    fd = open("/dev/video0",O_RDWR,0);	//打开视频设备
    if(fd == -1)
    {
        perror("open");
        return NULL;
    }
        
    struct sigaction sigIntHandler;  

    sigIntHandler.sa_handler = my_handler;  
    sigemptyset(&sigIntHandler.sa_mask);  
    sigIntHandler.sa_flags = 0;  

    sigaction(SIGINT, &sigIntHandler, NULL);  
    
    pthread_mutex_init(&g_lock, NULL);
    pthread_mutex_init(&g_lock1, NULL);
    pthread_cond_init(&g_cond, NULL);

    setMark();	                            //设置V4L2格式
    AllocMem();	                        //设置内存映射

    if(encode_init() != 0)
    {
      //  fprintf(stderr,"encode_init error \n");
    }
    //if(sacle_init() != 0)
    {
        //fprintf(stderr,"sacle_init error \n");
    }
    //tcpToolInit();
    pthread_create(&thread_enc, NULL, pthread_encode, NULL);
    pthread_video(num);
    //pthread_join(&thread_enc,NULL);
    sleep(1000);
    //end_encode();
    //end_scale();
    //tcpToolEnd();
    return NULL;
}
