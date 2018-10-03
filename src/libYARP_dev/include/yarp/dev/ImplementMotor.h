/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IMPLEMENTMOTOR_H
#define YARP_DEV_IMPLEMENTMOTOR_H

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

    virtual bool getNumberOfMotors(int *num) override;

    virtual bool getTemperature      (int m, double* val) override;
    virtual bool getTemperatures     (double *vals) override;
    virtual bool getTemperatureLimit (int m, double* val) override;
    virtual bool setTemperatureLimit (int m, const double val) override;
    virtual bool getGearboxRatio     (int m, double* val) override;
    virtual bool setGearboxRatio     (int m, const double val) override;
};

#endif // YARP_DEV_IMPLEMENTMOTOR_H
