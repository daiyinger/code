/*
 * Filename:  H264FramedLiveSource.cpp
 * Auther:  mlj
 * Create date: 2013/ 1/22
 */

#include "H264FramedLiveSource.hh"
 

extern "C" int end_encode(void);
extern "C" int encode_init(void);
H264FramedLiveSource::H264FramedLiveSource( UsageEnvironment& env,  
    char const* fileName, 
    unsigned preferredFrameSize, 
    unsigned playTimePerFrame )
    : FramedSource(env)
{
    //encode_init(); 
    if(encode_init() != 0)
    {
        fprintf(stderr,"encode_init error \n");
    }
    //fp = fopen( fileName, "rb" );  
    fprintf(stderr,"fopen  %x\n",(unsigned int)&fp);
}

H264FramedLiveSource* H264FramedLiveSource::createNew( UsageEnvironment& env,
                                           char const* fileName, 
                                           unsigned preferredFrameSize /*= 0*/, 
                                           unsigned playTimePerFrame /*= 0*/ )
{ 
    H264FramedLiveSource* newSource = new H264FramedLiveSource(env, fileName, preferredFrameSize, playTimePerFrame);
 
    return newSource;
}

H264FramedLiveSource::~H264FramedLiveSource()
{
    end_encode();
    fprintf(stderr,"fclose %x\n",(unsigned int)&fp);
    //fclose(fp);

}


long filesize(FILE *stream)
{
    long curpos, length;
    curpos = ftell(stream);
    fseek(stream, 0L, SEEK_END);
    length = ftell(stream);
    fseek(stream, curpos, SEEK_SET);

    return length;
}
extern "C" int readOneFrame(unsigned char *buf, int want_size);
void H264FramedLiveSource::doGetNextFrame()
{
#if 0
    static int flag = 0;
    if( (unsigned int)filesize(fp) >  fMaxSize)
    {
        if(fMaxSize > 1000)
		{
            fFrameSize = fread(fTo,1,fMaxSize/10,fp); 
        }
        else
        {
            fFrameSize = fread(fTo,1,fMaxSize,fp);
        } 
		if(fFrameSize == 0 )
		{
			if(flag == 0)
				printf("LEN %d max size %d\n", fFrameSize, fMaxSize);
			flag = 1;
		}
		else
		{
			printf("LEN %d max size %d\n", fFrameSize, fMaxSize);
            fprintf(stderr,"fd %x\n",(unsigned int)&fp);
		}
	}
    else
    {
        fFrameSize = fread(fTo,1,filesize(fp),fp);
        fseek(fp, 0, SEEK_SET);
		printf("len %d\n",fFrameSize);
    }
#else
	//printf("--------------------------in\n");
    //fFrameSize = fMaxSize;
    fFrameSize = readOneFrame(fTo, fMaxSize);
    if(fFrameSize == 0)
    {
	usleep(100000);
    }
    if(fFrameSize > fMaxSize)
    {
	fNumTruncatedBytes = fFrameSize - fMaxSize;
	fprintf(stderr,"fMaxSize = %d fFrameSize %d\n",fMaxSize,fFrameSize);
    }
    if(fFrameSize > 0)
	printf("fMaxSize = %d fFrameSize %d\n",fMaxSize,fFrameSize);
    //fDurationInMicroseconds = 20000;
#endif
    nextTask() = envir().taskScheduler().scheduleDelayedTask( 0,
        (TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数
    return;
}
