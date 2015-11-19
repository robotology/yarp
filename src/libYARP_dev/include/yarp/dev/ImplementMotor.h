// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARPMOTORIMPL__
#define __YARPMOTORIMPL__

#include <yarp/dev/IMotor.h>

namespace yarp {
    namespace dev {
        class ImplementMotor;
    }
}

class YARP_dev_API yarp::dev::ImplementMotor: public IMotor
{
protected:
    IMotorRaw *imotor;
    void *helper;
    double *temp1;
    double *temp2;


    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize ();

public:
    /* Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementMotor(yarp::dev::IMotorRaw *y);


    virtual ~ImplementMotor();

    virtual bool getNumberOfMotors(int *num);

    virtual bool getTemperature      (int m, double* val);
    virtual bool getTemperatures     (double *vals);
    virtual bool getTemperatureLimit (int m, double* val);
    virtual bool setTemperatureLimit (int m, const double val);
    virtual bool getMotorOutputLimit (int m, double* val);
    virtual bool setMotorOutputLimit (int m, const double val);
    virtual bool getGearboxRatio     (int m, double* val);
    virtual bool setGearboxRatio     (int m, const double val);
};

#endif
