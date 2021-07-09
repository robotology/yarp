/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef LASERHOKUYO_THREAD_H
#define LASERHOKUYO_THREAD_H

//#include <cstdio>
#include <string>

#include <yarp/os/PeriodicThread.h>
#include <mutex>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ISerialDevice.h>
#include <yarp/sig/Vector.h>

using namespace yarp::os;
using namespace yarp::dev;

/**
 * @ingroup dev_impl_lidar
 *
 * \brief `laserHokuyo`: Documentation to be added
 */
class laserHokuyo : public PeriodicThread, public yarp::dev::IRangefinder2D, public DeviceDriver
{
protected:
    PolyDriver driver;
    ISerialDevice *pSerial;

    std::mutex mutex;

    int cardId;
    double period;
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
    laserHokuyo(double period = 0.02) : PeriodicThread(period),
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

private:
    //laser methods
    int  calculateCheckSum(const char* buffer, int size, char actual_sum);
    long decodeDataValue(const char* data, int data_byte);
    int  readData(const Laser_mode_type laser_mode, const char* text_data, const int lext_data_len, int current_line, yarp::sig::Vector& values);
};

#endif
