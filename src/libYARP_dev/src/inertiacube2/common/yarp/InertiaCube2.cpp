// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "InertiaCube2.h"

#ifdef LINUX
#include "../../linux/dd_orig/include/isense.h"
#include "../../linux/dd_orig/include/types.h"
#include "../../linux/dd_orig/include/isense.c"
#endif
#ifdef WIN32
#include "../../winnt/dd_orig/include/isense.h"
#include "../../winnt/dd_orig/include/types.h"
#include "../../winnt/dd_orig/include/isense.c"
#endif

#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

class IntersenseResources: public Thread
{
 public:
    IntersenseResources(void): _mutex(1)
        {
            _bStreamStarted=false;
            _bError=false;
            
            _last=0;

            _last=new double [3];
            for(int k=0;k<3;k++)
                _last[k]=0.0;
        }

    ~IntersenseResources()
    {
        if (isRunning())
            stop();

        _bStreamStarted=false;

        if (_last!=0)
            {
                delete [] _last;
                _last=0;
            }
    }

    bool _bStreamStarted;
    bool _bError;

    ISD_TRACKER_HANDLE _handle_tracker;
    ISD_TRACKER_INFO_TYPE _info_tracker;
    ISD_STATION_INFO_TYPE _info_station;
    
    double *_last;

    Semaphore _mutex;

    virtual void run();
};

void IntersenseResources::run(void)
{
    bool received;

    ISD_TRACKER_DATA_TYPE reply;

    while(!Thread::isStopping())
        {
            received=ISD_GetData(_handle_tracker, &reply);

            _mutex.wait();
            
            _last[0]=reply.Station[0].Orientation[2]; //roll
            _last[1]=reply.Station[0].Orientation[1]; //pitch
            _last[2]=reply.Station[0].Orientation[0]; //yaw

            if(received)
                _bError=false;
            else
                _bError=true;

            _mutex.post();
        }
}

inline IntersenseResources& RES(void *res) { return *(IntersenseResources *)res; }

InertiaCube2::InertiaCube2()
{
    system_resources=0;
    nchannels=3;
}

InertiaCube2::~InertiaCube2()
{
    // stop thread first
    if (system_resources!=0)
        {
            delete ((IntersenseResources *)(system_resources));
            system_resources=0;
        }
}

bool InertiaCube2::read(Vector &out)
{
    IntersenseResources &d= RES(system_resources);
    bool ret;
    
    if (d._bStreamStarted)
        {
            d._mutex.wait();
            
            out[0]=d._last[0];
            out[1]=d._last[1];
            out[2]=d._last[2];
                
            d._mutex.post();

            ret=d._bError;
        }
    else
        ret=false;

    return ret;
}

bool InertiaCube2::getChannels(int *nc)
{
    *nc=nchannels;
    return true;
}

bool InertiaCube2::start()
{
    IntersenseResources &d=RES(system_resources);

    d.start();
    d._bStreamStarted=true;

    return true;
}

bool InertiaCube2::stop()
{
    IntersenseResources &d=RES(system_resources);

    if (d.isRunning())
        d.stop();

    d._bStreamStarted=false;

    return true;
}

bool InertiaCube2::open(yarp::os::Searchable &config)
{
	InertiaCube2Parameters par;
    Property p;
    p.fromString(config.toString());

	par.comPort=p.findGroup("GENERAL").find("ComPort").asInt();

	return open(par);
}

bool InertiaCube2::open(const InertiaCube2Parameters &par)
{
	if (system_resources!=0)
		return false;

    system_resources=(void *) (new IntersenseResources);

	IntersenseResources &d=RES(system_resources);

    d._handle_tracker = ISD_OpenTracker(0, par.comPort, false, false);
	if (d._handle_tracker<0)
		return false;

    ISD_GetTrackerConfig(d._handle_tracker, &d._info_tracker, true);
    ISD_GetStationConfig(d._handle_tracker, &d._info_station, 1, true);

    // start thread
    return InertiaCube2::start();
}

bool InertiaCube2::close()
{
    // stop thread
    if (system_resources==0)
        return false; //the device was never opened, or there was an error 

    InertiaCube2::stop();

    IntersenseResources &d=RES(system_resources);

    if (ISD_CloseTracker( d._handle_tracker) == false)
	{
		delete ((IntersenseResources *)(system_resources));
		system_resources=0;
        return false;
	}
    else
	{
		delete ((IntersenseResources *)(system_resources));
		system_resources=0;
        return true;
	}
}

