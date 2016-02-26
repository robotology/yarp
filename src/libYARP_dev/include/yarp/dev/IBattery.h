/*
 * Copyright (C) 2015 ICub Facility - Istituto italiano di Tecnologia
 * Author: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPBATTERYINTERFACE
#define YARPBATTERYINTERFACE

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Vector.h>

#define VOCAB_IBATTERY     VOCAB4('i','b','a','t')
#define VOCAB_BATTERY_INFO VOCAB4('b','t','n','f')

/*! \file Ibattery.h battery interface */
namespace yarp {
    namespace dev {
        class IBattery;
    }
}

/**
 * @ingroup dev_iface_other
 *
 * A generic battery interface
 */
class YARP_dev_API yarp::dev::IBattery
{
public:
    enum
    {
        BATTERY_OK_STANBY        = 0,
        BATTERY_OK_IN_CHARGE     = 1,
        BATTERY_OK_IN_USE        = 2,
        BATTERY_GENERAL_ERROR    = 3,
        BATTERY_TIMEOUT          = 4,
        BATTERY_LOW_WARNING      = 5,
        BATTERY_CRITICAL_WARNING = 6
    };

    virtual ~IBattery(){}

    /**
     * Get the instantaneous voltage measurement
     * @param voltage the voltage measurement
     * @return true/false.
     */
    virtual bool getBatteryVoltage(double &voltage)=0;

    /**
     * Get the instantaneous current measurement
     * @param current the current measurement
     * @return true/false.
     */
    virtual bool getBatteryCurrent(double &current) = 0;

    /**
     * get the battery status of charge
     * @param charge the charge measurement (0-100%)
     * @return true/false.
     */
    virtual bool getBatteryCharge(double &charge) = 0;

    /**
    * get the battery status
    * @param status the battery status
    * @return true/false.
    */
    virtual bool getBatteryStatus(int &status) = 0;

    /**
    * get the battery temperature
    * @param temprature the battery temperature
    * @return true/false.
    */
    virtual bool getBatteryTemperature(double &temperature) = 0;

    /**
    * get the battery hardware charactestics (e.g. max voltage etc)
    * @param a string containing the battery infos
    * @return true/false.
    */
    virtual bool getBatteryInfo(yarp::os::ConstString &battery_info) = 0;
};

#endif
