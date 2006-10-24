// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Radu Bogdan Rasu, Alexis Maldonado
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __XSENSMTX__
#define __XSENSMTX__

#include "MTComm.h"
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/GenericSensorInterfaces.h>
#include <string>

namespace yarp{
    namespace dev{
        class XSensMTx;
    }
}

struct XSensMTxParameters
{
    std::string comPortString;
    short comPort;
};

/**
 *
 * @ingroup dev_impl
 *
 * Driver for XSens's MTx IMU unit.
 * @author Radu Bogdan Rusu, Alexis Maldonado
 */
class yarp::dev::XSensMTx : public IGenericSensor, public DeviceDriver
{
public:
    XSensMTx();
    ~XSensMTx();
    
    // IGenericSensor interface.
    virtual bool read(yarp::sig::Vector &out);
    virtual bool getChannels(int *nc);
    virtual bool open(yarp::os::Searchable &config);
    virtual bool calibrate(int ch, double v);
    virtual bool close();

    // Open the device
    bool open(const XSensMTxParameters &par);

private:
    bool start();
    bool stop();

    void *system_resources;
    int nchannels;
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_xsensmtx XSensMTx

 An inertial sensor source, see yarp::dev::XSensMTx.

*/

#endif
