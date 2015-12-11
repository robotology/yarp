// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

// ********************************************************
// *** THIS FILE IS CURRENTLY UNDER DEVELOPMENT / DEBUG ***
// ********************************************************

#ifndef __LASERHOKUYO_THREAD_H__
#define __LASERHOKUYO_THREAD_H__

//#include <stdio.h>
#include <string>

#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/SerialInterfaces.h>
#include <yarp/sig/Vector.h>

using namespace yarp::os;
using namespace yarp::dev;

class laserHokuyo : public RateThread, public yarp::dev::IRangefinder2D, public DeviceDriver
{
protected:
    PolyDriver driver;
    ISerialDevice *pSerial;
   
    yarp::os::Semaphore mutex;

    int cardId;
    int period;
    int sensorsNum;
    int start_position;
    int end_position;
    int error_codes;
    int internal_status;
    std::string info;
    Device_status device_status;

    enum Laser_mode_type {FAKE_MODE=2, GD_MODE=1, MD_MODE=0};
    enum Error_code
    {
        HOKUYO_STATUS_ACQUISITION_COMPLETE =1,
        HOKUYO_STATUS_OK = 0,
        HOKUYO_STATUS_ERROR_BUSY = -1,
        HOKUYO_STATUS_ERROR_INVALID_COMMAND = -2,
        HOKUYO_STATUS_ERROR_INVALID_CHECKSUM = -3,
        HOKUYO_STATUS_ERROR_NOTHING_RECEIVED = -4,
        HOKUYO_STATUS_NOT_READY = -5
    };

    Laser_mode_type laser_mode;

    struct sensor_property_struct 
    {
        std::string MODL;
        int DMIN;
        int DMAX;
        int ARES;
        int AMIN;
        int AMAX;
        int AFRT;
        int SCAN;
    } sensor_properties;

    yarp::sig::Vector laser_data;

public:
    laserHokuyo(int period=20) : RateThread(period),mutex(1)
    {}
    

    ~laserHokuyo()
    {
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
    //laser methods
    int  calculateCheckSum(const char* buffer, int size, char actual_sum);
    long decodeDataValue(const char* data, int data_byte);
    int  readData(const Laser_mode_type laser_mode, const char* text_data, const int lext_data_len, int current_line, yarp::sig::Vector& values);
};

#endif
