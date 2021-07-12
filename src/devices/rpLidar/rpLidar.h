/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef RPLIDAR_H
#define RPLIDAR_H


#include <yarp/os/LogComponent.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/ISerialDevice.h>

#include <mutex>
#include <string>
#include <vector>

using namespace yarp::os;
using namespace yarp::dev;

typedef unsigned char byte;


YARP_DECLARE_LOG_COMPONENT(RPLIDAR)


class rpLidarCircularBuffer
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
            yCError(RPLIDAR, "rpLidar buffer overrun!");
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
            yCError(RPLIDAR, "rpLidar buffer underrun!");
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

    rpLidarCircularBuffer(int bufferSize);
    ~rpLidarCircularBuffer();
};

//---------------------------------------------------------------------------------------------------------------
struct Range_t
{
    double min;
    double max;
};

//---------------------------------------------------------------------------------------------------------------
/**
 * @ingroup dev_impl_lidar
 *
 * \brief `rpLidar`: Documentation to be added
 */
class RpLidar : public PeriodicThread, public yarp::dev::IRangefinder2D, public DeviceDriver
{
protected:
    PolyDriver driver;
    ISerialDevice *pSerial;

    std::mutex mutex;
    rpLidarCircularBuffer * buffer;

    int sensorsNum;

    double min_angle;
    double max_angle;
    double min_distance;
    double max_distance;
    double resolution;
    bool clip_max_enable;
    bool clip_min_enable;
    bool do_not_clip_infinity_enable;
    std::vector <Range_t> range_skip_vector;

    std::string info;
    Device_status device_status;

    yarp::sig::Vector laser_data;

public:
    RpLidar(double period = 0.01) : PeriodicThread(period),
        pSerial(nullptr),
        sensorsNum(0),
        min_angle(0.0),
        max_angle(0.0),
        min_distance(0.0),
        max_distance(0.0),
        resolution(0.0),
        clip_max_enable(false),
        clip_min_enable(false),
        do_not_clip_infinity_enable(false),
        device_status(Device_status::DEVICE_OK_STANBY)
    {
        buffer = new rpLidarCircularBuffer(20000);
    }


    ~RpLidar()
    {
        if (buffer)
        {
            delete buffer;
            buffer = 0;
        }
    }

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

public:
    //IRangefinder2D interface
    bool getRawData(yarp::sig::Vector &data) override;
    bool getLaserMeasurement(std::vector<LaserMeasurementData> &data) override;
    bool getDeviceStatus     (Device_status &status) override;
    bool getDeviceInfo       (std::string &device_info) override;
    bool getDistanceRange    (double& min, double& max) override;
    bool setDistanceRange    (double min, double max) override;
    bool getScanLimits        (double& min, double& max) override;
    bool setScanLimits        (double min, double max) override;
    bool getHorizontalResolution      (double& step) override;
    bool setHorizontalResolution      (double step) override;
    bool getScanRate         (double& rate) override;
    bool setScanRate         (double rate) override;

public:
    //Lidar2DDeviceBase
    //bool acquireDataFromHW() override final;

private:
    bool  HW_getHealth();
    bool  HW_reset();
    bool  HW_start();
    bool  HW_stop();
    bool  HW_getInfo(std::string& s_info);

};

#endif
