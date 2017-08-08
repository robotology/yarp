/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_IMPLEMENTENCODERSTIMED_H
#define YARP_DEV_IMPLEMENTENCODERSTIMED_H

#include <yarp/dev/IEncodersTimed.h>

namespace yarp {
    namespace dev {
        class ImplementEncodersTimed;
    }
}

class YARP_dev_API yarp::dev::ImplementEncodersTimed: public IEncodersTimed
{
protected:
    IEncodersTimedRaw *iEncoders;
    void *helper;
    double *temp;
    double *temp2;


    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap, const double *enc, const double *zos);

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
    ImplementEncodersTimed(yarp::dev::IEncodersTimedRaw *y);


    virtual ~ImplementEncodersTimed();

    virtual bool getAxes(int *ax) YARP_OVERRIDE;

    virtual bool resetEncoder(int j) YARP_OVERRIDE;
    virtual bool resetEncoders() YARP_OVERRIDE;
    virtual bool setEncoder(int j, double val) YARP_OVERRIDE;
    virtual bool setEncoders(const double *vals) YARP_OVERRIDE;
    virtual bool getEncoder(int j, double *v) YARP_OVERRIDE;
    virtual bool getEncodersTimed(double *encs, double *time) YARP_OVERRIDE;
    virtual bool getEncoderTimed(int j, double *v, double *t) YARP_OVERRIDE;
    virtual bool getEncoders(double *encs) YARP_OVERRIDE;
    virtual bool getEncoderSpeed(int j, double *spds) YARP_OVERRIDE;
    virtual bool getEncoderSpeeds(double *spds) YARP_OVERRIDE;
    virtual bool getEncoderAcceleration(int j, double *spds) YARP_OVERRIDE;
    virtual bool getEncoderAccelerations(double *accs) YARP_OVERRIDE;
};

#endif // YARP_DEV_IMPLEMENTENCODERSTIMED_H
