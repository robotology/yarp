/*
* Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*/


#ifndef RPLIDAR_H
#define RPLIDAR_H

#include <string>

#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/SerialInterfaces.h>

using namespace yarp::os;
using namespace yarp::dev;

typedef unsigned char byte;

class circularBuffer
{
    int         maxsize;
    int         start;
    int         end;
    byte       *elems;

public:
    inline bool isFull()
    {
        return (end + 1) % maxsize == start;
    }

    inline const byte* getRawData()
    {
        return elems;
    }

    inline bool isEmpty()
    {
        return end == start;
    }

    inline bool write_elem(byte elem)
    {
        elems[end] = elem;
        end = (end + 1) % maxsize;
        if (end == start)
        {
            yError("rpLidar buffer overrun!");
            start = (start + 1) % maxsize; // full, overwrite 
            return false;
        }
        return true;
    }

    inline bool write_elems(byte* elems, int size)
    {
        for (int i = 0; i < size; i++)
        {
            if (write_elem(elems[i]) == false) return false;
        }
        return true;
    }

    inline int size()
    {
        int i;
        if (end>start)
            i = end - start;
        else if (end == start)
            i = 0;
        else
            i = maxsize - start + end;
        return i;
    }

    inline bool read_elem(byte* elem)
    {
        if (end == start)
        {
            yError("rpLidar buffer underrun!");
            return false;
        }
        *elem = elems[start];
        start = (start + 1) % maxsize;
        return true;
    }

    inline void throw_away_elem()
    {
        start = (start + 1) % maxsize;
    }

    inline void throw_away_elems(int size)
    {
        start = (start + size) % maxsize;
    }

    inline byte select_elem(int offset)
    {
        return elems[(start+offset) % maxsize];
    }

    inline void select_elems(byte* elems, int size)
    {
        for (int i = 0; i < size; i++)
        {
            elems[i] = select_elem(i);
        }
    }

    inline bool read_elems(byte* elems, int size)
    {
        for (int i = 0; i < size; i++)
        {
            if (read_elem(&elems[i]) == false) return false;
        }
        return true;
    }

    inline unsigned int getMaxSize()
    {
        return maxsize;
    }

    inline void clear()
    {
        start = 0;
        end = 0;
    }

    inline unsigned int get_start()
    {
        return start;
    }

    inline unsigned int get_end()
    {
        return end;
    }

    circularBuffer(int bufferSize);
    ~circularBuffer();
};

//---------------------------------------------------------------------------------------------------------------

class RpLidar : public RateThread, public yarp::dev::IRangefinder2D, public DeviceDriver
{
protected:
    PolyDriver driver;
    ISerialDevice *pSerial;

    yarp::os::Mutex mutex;
    circularBuffer * buffer;

    int sensorsNum;

    double min_angle;
    double max_angle;
    double min_distance;
    double max_distance;
    double resolution;
    bool clip_max_enable;
    bool clip_min_enable;
    bool do_not_clip_infinity_enable;

    std::string info;
    Device_status device_status;

    yarp::sig::Vector laser_data;

public:
    RpLidar(int period = 10) : RateThread(period)
    {
        buffer = new circularBuffer(20000);
    }
    

    ~RpLidar()
    {
        if (buffer)
        {
            delete buffer;
            buffer = 0;
        }
    }

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();
    virtual bool threadInit();
    virtual void threadRelease();
    virtual void run();

public:
    //IRangefinder2D interface
    virtual bool getMeasurementData  (yarp::sig::Vector &out);
    virtual bool getDeviceStatus     (Device_status &status);
    virtual bool getDeviceInfo       (yarp::os::ConstString &device_info);
    virtual bool getDistanceRange    (double& min, double& max);
    virtual bool setDistanceRange    (double min, double max);
    virtual bool getScanLimits        (double& min, double& max);
    virtual bool setScanLimits        (double min, double max);
    virtual bool getHorizontalResolution      (double& step);
    virtual bool setHorizontalResolution      (double step);
    virtual bool getScanRate         (double& rate);
    virtual bool setScanRate         (double rate);

private:
    bool  HW_getHealth();
    bool  HW_reset();
    bool  HW_start();
    bool  HW_stop();
    bool  HW_getInfo(std::string& s_info);

};

#endif
