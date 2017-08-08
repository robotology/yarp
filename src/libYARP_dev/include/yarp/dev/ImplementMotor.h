/*
 * Copyright (C) 2015 Istituto Italiano di Tecnologia (IIT)
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

    virtual bool getNumberOfMotors(int *num) YARP_OVERRIDE;

    virtual bool getTemperature      (int m, double* val) YARP_OVERRIDE;
    virtual bool getTemperatures     (double *vals) YARP_OVERRIDE;
    virtual bool getTemperatureLimit (int m, double* val) YARP_OVERRIDE;
    virtual bool setTemperatureLimit (int m, const double val) YARP_OVERRIDE;
    virtual bool getGearboxRatio     (int m, double* val) YARP_OVERRIDE;
    virtual bool setGearboxRatio     (int m, const double val) YARP_OVERRIDE;
};

#endif // YARP_DEV_IMPLEMENTMOTOR_H
