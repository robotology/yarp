/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    int nj;


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

    virtual bool getAxes(int *ax) override;

    virtual bool resetEncoder(int j) override;
    virtual bool resetEncoders() override;
    virtual bool setEncoder(int j, double val) override;
    virtual bool setEncoders(const double *vals) override;
    virtual bool getEncoder(int j, double *v) override;
    virtual bool getEncodersTimed(double *encs, double *time) override;
    virtual bool getEncoderTimed(int j, double *v, double *t) override;
    virtual bool getEncoders(double *encs) override;
    virtual bool getEncoderSpeed(int j, double *spds) override;
    virtual bool getEncoderSpeeds(double *spds) override;
    virtual bool getEncoderAcceleration(int j, double *spds) override;
    virtual bool getEncoderAccelerations(double *accs) override;
};

#endif // YARP_DEV_IMPLEMENTENCODERSTIMED_H
