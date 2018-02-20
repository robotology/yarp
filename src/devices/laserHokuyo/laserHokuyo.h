/*
 * Copyright (C) 2010 Istituto Italiano di Tecnologia (IIT)
 * Author: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

// ********************************************************
// *** THIS FILE IS CURRENTLY UNDER DEVELOPMENT / DEBUG ***
// ********************************************************

#ifndef LASERHOKUYO_THREAD_H
#define LASERHOKUYO_THREAD_H

//#include <cstdio>
#include <string>

#include <yarp/os/RateThread.h>
#include <yarp/os/Mutex.h>
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

    yarp::os::Mutex mutex;

    int cardId;
    int period;
    int sensorsNum;
    int start_position;
    int end_position;
    double min_angle;
    double max_angle;
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
    laserHokuyo(int period=20) : RateThread(period),
        pSerial(nullptr),
        mutex(),
        cardId(0),
        period(period),
        sensorsNum(0),
        start_position(0),
        end_position(0),
        min_angle(0.0),
        max_angle(0.0),
        error_codes(0),
        internal_status(0),
        info(""),
        device_status(Device_status::DEVICE_OK_STANBY),
        laser_mode(Laser_mode_type::FAKE_MODE)
    {}


    ~laserHokuyo()
    {
    }

    virtual bool open(yarp::os::Searchable& config) override;
    virtual bool close() override;
    virtual bool threadInit() override;
    virtual void threadRelease() override;
    virtual void run() override;

public:
    //IRangefinder2D interface
    virtual bool getRawData(yarp::sig::Vector &data) override;
    virtual bool getLaserMeasurement(std::vector<LaserMeasurementData> &data) override;
    virtual bool getDeviceStatus     (Device_status &status) override;
    virtual bool getDeviceInfo       (yarp::os::ConstString &device_info) override;
    virtual bool getDistanceRange    (double& min, double& max) override;
    virtual bool setDistanceRange    (double min, double max) override;
    virtual bool getScanLimits        (double& min, double& max) override;
    virtual bool setScanLimits        (double min, double max) override;
    virtual bool getHorizontalResolution      (double& step) override;
    virtual bool setHorizontalResolution      (double step) override;
    virtual bool getScanRate         (double& rate) override;
    virtual bool setScanRate         (double rate) override;

private:
    //laser methods
    int  calculateCheckSum(const char* buffer, int size, char actual_sum);
    long decodeDataValue(const char* data, int data_byte);
    int  readData(const Laser_mode_type laser_mode, const char* text_data, const int lext_data_len, int current_line, yarp::sig::Vector& values);
};

#endif
