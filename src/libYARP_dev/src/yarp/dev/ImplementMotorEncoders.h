/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTMOTORENCODERS_H
#define YARP_DEV_IMPLEMENTMOTORENCODERS_H

#include <yarp/dev/IMotorEncoders.h>

namespace yarp {
    namespace dev {
        class ImplementMotorEncoders;
    }
}

namespace yarp {
namespace dev {
namespace impl {

template <typename T>
class FixedSizeBuffersManager;

} // namespace impl
} // namespace dev
} // namespace yarp

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

    bool getNumberOfMotorEncoders(int *num) override;

    bool resetMotorEncoder(int m) override;
    bool resetMotorEncoders() override;
    bool setMotorEncoder(int m, const double val) override;
    bool setMotorEncoders(const double *vals) override;
    bool setMotorEncoderCountsPerRevolution(int m, const double cpr) override;
    bool getMotorEncoderCountsPerRevolution(int m, double *cpr) override;
    bool getMotorEncoder(int m, double *v) override;
    bool getMotorEncodersTimed(double *encs, double *time) override;
    bool getMotorEncoderTimed(int m, double *v, double *t) override;
    bool getMotorEncoders(double *encs) override;
    bool getMotorEncoderSpeed(int m, double *spds) override;
    bool getMotorEncoderSpeeds(double *spds) override;
    bool getMotorEncoderAcceleration(int m, double *spds) override;
    bool getMotorEncoderAccelerations(double *accs) override;
};

#endif // YARP_DEV_IMPLEMENTMOTORENCODERS_H
