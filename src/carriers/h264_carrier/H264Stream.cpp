/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "H264Stream.h"
#include "H264LogComponent.h"

#include <yarp/os/LogStream.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>

#include <cstdio>
#include <cstring>


//#define debug_time 1

#ifdef debug_time
    #include <yarp/os/Time.h>
    #define DBG_TIME_PERIOD_PRINTS 10 //10 sec
#endif


using namespace yarp::os;
using namespace yarp::sig;
using namespace std;

H264Stream::H264Stream(h264Decoder_cfgParamters &config) :
        delegate(nullptr),
        blobHeader{0,0,0},
        phase(0),
        cursor(nullptr),
        remaining(0),
        decoder(nullptr),
        cfg(config)
{}

H264Stream::~H264Stream()
{
    delete decoder;
    delete delegate;
}



bool H264Stream::setStream(yarp::os::impl::DgramTwoWayStream *stream)
{
    delegate = stream;
    if(nullptr == delegate)
    {
        return false;
    }
    return true;
}

void H264Stream::start()
{
    decoder = new H264Decoder(this->cfg);
    decoder->init();
    decoder->start();
}

InputStream& H264Stream::getInputStream()
{
    return *this;
}

OutputStream& H264Stream::getOutputStream()
{
    return *this;
}

//using yarp::os::OutputStream::write;


//using yarp::os::InputStream::read;

bool H264Stream::setReadEnvelopeCallback(InputStream::readEnvelopeCallbackType callback, void* data)
{
    return true;
}

yarp::conf::ssize_t H264Stream::read(Bytes& b)
{

#ifdef debug_time
    static bool isFirst = true;
    double start_time = Time::now();
    double start_timeCopy;
    double end_time=0;
    static double last_call;
    static double sumOf_timeBetweenCalls=0;

    static double sumOf_timeOnMutex = 0;
    static double sumOf_timeOfCopyPerPahse[5] ={0};
    static uint32_t count=0;
    static uint32_t countPerPhase[5]={0};
    #define MAX_COUNT  100


    if(isFirst)
    {
        last_call = start_time;
        isFirst = false;
    }
    else
    {
        sumOf_timeBetweenCalls+=(start_time -last_call);
        last_call = start_time;
    }


#endif

    if (remaining==0)
    {
        if (phase==1)
        {
            phase = 2;
            cursor = (char*)(img.getRawImage());
            remaining = img.getRawImageSize();
        } else if (phase==3)
        {
            phase = 4;
            cursor = nullptr;
            remaining = blobHeader.blobLen;
        } else
        {
            phase = 0;
        }
    }
    while (phase==0)
    {
        decoder->mutex.lock();
        int len = 0;
        if(decoder->newFrameIsAvailable())
        {
            ImageOf<PixelRgb> & img_dec = decoder->getLastFrame();
            img.copy(img_dec);
            len = decoder->getLastFrameSize();
            decoder->mutex.unlock();
            #ifdef debug_time
            end_time = Time::now();
            sumOf_timeOnMutex +=(end_time - start_time);
            count++;
            if(count>=MAX_COUNT)
            {
                yCDebug(H264CARRIER,
                       "STREAM On %d times: timeOnMutex is long %.6f sec",
                       MAX_COUNT, (sumOf_timeOnMutex/MAX_COUNT) );
                for(int x=0; x<5; x++)
                {
                    yCDebug(H264CARRIER,
                           "STREAM: phase:%d, count=%u, time=%.6f sec",
                           x,
                           countPerPhase[x],
                           ((countPerPhase[x]==0) ? 0: sumOf_timeOfCopyPerPahse[x]/countPerPhase[x]) );
                    countPerPhase[x] = 0;
                    sumOf_timeOfCopyPerPahse[x] = 0;
                }
                yCDebug(H264CARRIER, "sleep=%.6f", sumOf_timeBetweenCalls/count);
                yCDebug(H264CARRIER);
                count = 0;
                isFirst = true;
                sumOf_timeOnMutex = 0;
                sumOf_timeBetweenCalls = 0;
            }
            #endif

        }
        else
        {
            yCTrace(H264CARRIER, "h264Stream::read has been called but no frame is available!!");
            phase = 0;
            remaining = 0;
            cursor = nullptr;
            decoder->setReq();
            decoder->mutex.unlock();
            decoder->semaphore.waitWithTimeout(1);
            return 0;
        }

        yCTrace(H264CARRIER, "Length is \"%d\"", len);

        imgHeader.setFromImage(img);
        phase = 1;
        cursor = (char*)(&imgHeader);
        remaining = sizeof(imgHeader);
    }

    if (remaining>0)
    {
        size_t allow = remaining;
        if (b.length()<allow)
        {
            allow = b.length();
        }
        if (cursor!=nullptr)
        {
            #ifdef debug_time
            start_timeCopy = Time::now();
            #endif
            memcpy(b.get(),cursor,allow);
            cursor+=allow;
            remaining-=allow;
            yCDebug(H264CARRIER, "returning %zd bytes", allow);
            #ifdef debug_time
                end_time = Time::now();
                sumOf_timeOfCopyPerPahse[phase] +=(end_time - start_timeCopy);
                countPerPhase[phase]++;
            #endif
            return allow;
        } else
        {
            yarp::conf::ssize_t result = delegate->getInputStream().read(b);
            yCTrace(H264CARRIER, "Read %zu bytes", result);
            if (result>0)
            {
                remaining-=result;
                yCTrace(H264CARRIER, "%zu bytes of meat", result);
                return result;
            }
        }
    }
    return -1;
}


void H264Stream::write(const Bytes& b)
{
    delegate->getOutputStream().write(b);
}
