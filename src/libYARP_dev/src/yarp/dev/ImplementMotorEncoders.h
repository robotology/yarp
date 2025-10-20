/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTMOTORENCODERS_H
#define YARP_DEV_IMPLEMENTMOTORENCODERS_H

#include <yarp/dev/IMotorEncoders.h>

namespace yarp::dev {
class ImplementMotorEncoders;
}

namespace yarp::dev::impl {

template <typename T>
class FixedSizeBuffersManager;

} // namespace yarp::dev::impl

class YARP_dev_API yarp::dev::ImplementMotorEncoders: public IMotorEncoders
{
protected:
    IMotorEncodersRaw *iMotorEncoders;
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
    ImplementMotorEncoders(yarp::dev::IMotorEncodersRaw *y);


    virtual ~ImplementMotorEncoders();

    yarp::dev::ReturnValue getNumberOfMotorEncoders(int *num) override;
    yarp::dev::ReturnValue resetMotorEncoder(int m) override;
    yarp::dev::ReturnValue resetMotorEncoders() override;
    yarp::dev::ReturnValue setMotorEncoder(int m, const double val) override;
    yarp::dev::ReturnValue setMotorEncoders(const double *vals) override;
    yarp::dev::ReturnValue setMotorEncoderCountsPerRevolution(int m, const double cpr) override;
    yarp::dev::ReturnValue getMotorEncoderCountsPerRevolution(int m, double *cpr) override;
    yarp::dev::ReturnValue getMotorEncoder(int m, double *v) override;
    yarp::dev::ReturnValue getMotorEncodersTimed(double *encs, double *time) override;
    yarp::dev::ReturnValue getMotorEncoderTimed(int m, double *v, double *t) override;
    yarp::dev::ReturnValue getMotorEncoders(double *encs) override;
    yarp::dev::ReturnValue getMotorEncoderSpeed(int m, double *spds) override;
    yarp::dev::ReturnValue getMotorEncoderSpeeds(double *spds) override;
    yarp::dev::ReturnValue getMotorEncoderAcceleration(int m, double *spds) override;
    yarp::dev::ReturnValue getMotorEncoderAccelerations(double *accs) override;
};

#endif // YARP_DEV_IMPLEMENTMOTORENCODERS_H
