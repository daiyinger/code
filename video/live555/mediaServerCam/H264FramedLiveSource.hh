/*
 * Filename: H264FramedLiveSource.hh
 * Auther: chenbin
 * Create date: 2013/ 1/22
 */

#ifndef _H264FRAMEDLIVESOURCE_HH
#define _H264FRAMEDLIVESOURCE_HH
#ifndef _MPEG_VIDEO_STREAM_FRAMER_HH
#include "MPEGVideoStreamFramer.hh"
#endif
#include <FramedSource.hh>
#include "liveMedia.hh"
class CamH264VideoStreamFramer: public H264VideoStreamFramer
{
public:
  virtual ~CamH264VideoStreamFramer();
  CamH264VideoStreamFramer(UsageEnvironment& env, 
    FramedSource* inputSource);
  
  static CamH264VideoStreamFramer* createNew(UsageEnvironment& env, FramedSource* inputSource);
  virtual void doGetNextFrame();

private:
    int init_flag;
    virtual unsigned maxFrameSize() const
    { 
	return 100*1024; //根据实际情况设置一个合理的值
    }
};

class H264FramedLiveSource : public FramedSource
{
public:
    static H264FramedLiveSource* createNew(UsageEnvironment& env,
        char const* fileName,
        unsigned preferredFrameSize = 0,
        unsigned playTimePerFrame = 0); 

protected:
    H264FramedLiveSource(UsageEnvironment& env,
        char const* fileName, 
        unsigned preferredFrameSize,
        unsigned playTimePerFrame);
    // called only by createNew()
    ~H264FramedLiveSource();

private:
    // redefined virtual functions:
    virtual void doGetNextFrame();
    int TransportData( unsigned char* to, unsigned maxSize );
    virtual unsigned maxFrameSize() const
    { 
	return 100*1024; //根据实际情况设置一个合理的值
    }
protected:
    FILE *fp;
    int init_flag;
};

#endif
