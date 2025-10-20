/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTENCODERSTIMED_H
#define YARP_DEV_IMPLEMENTENCODERSTIMED_H

#include <yarp/dev/IEncodersTimed.h>

namespace yarp::dev {
class ImplementEncodersTimed;
}

namespace yarp::dev::impl {

template <typename T>
class FixedSizeBuffersManager;

} // namespace yarp::dev::impl

class YARP_dev_API yarp::dev::ImplementEncodersTimed: public IEncodersTimed
{
protected:
    IEncodersTimedRaw *iEncoders;
    void *helper;
    yarp::dev::impl::FixedSizeBuffersManager<double> *buffManager;


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

    yarp::dev::ReturnValue getAxes(int *ax) override;

    yarp::dev::ReturnValue resetEncoder(int j) override;
    yarp::dev::ReturnValue resetEncoders() override;
    yarp::dev::ReturnValue setEncoder(int j, double val) override;
    yarp::dev::ReturnValue setEncoders(const double *vals) override;
    yarp::dev::ReturnValue getEncoder(int j, double *v) override;
    yarp::dev::ReturnValue getEncodersTimed(double *encs, double *time) override;
    yarp::dev::ReturnValue getEncoderTimed(int j, double *v, double *t) override;
    yarp::dev::ReturnValue getEncoders(double *encs) override;
    yarp::dev::ReturnValue getEncoderSpeed(int j, double *spds) override;
    yarp::dev::ReturnValue getEncoderSpeeds(double *spds) override;
    yarp::dev::ReturnValue getEncoderAcceleration(int j, double *spds) override;
    yarp::dev::ReturnValue getEncoderAccelerations(double *accs) override;
};

#endif // YARP_DEV_IMPLEMENTENCODERSTIMED_H
