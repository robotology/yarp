/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTENCODERSTIMED_H
#define YARP_DEV_IMPLEMENTENCODERSTIMED_H

#include <yarp/dev/IEncodersTimed.h>

namespace yarp {
    namespace dev {
        class ImplementEncodersTimed;
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

    bool getAxes(int *ax) override;

    bool resetEncoder(int j) override;
    bool resetEncoders() override;
    bool setEncoder(int j, double val) override;
    bool setEncoders(const double *vals) override;
    bool getEncoder(int j, double *v) override;
    bool getEncodersTimed(double *encs, double *time) override;
    bool getEncoderTimed(int j, double *v, double *t) override;
    bool getEncoders(double *encs) override;
    bool getEncoderSpeed(int j, double *spds) override;
    bool getEncoderSpeeds(double *spds) override;
    bool getEncoderAcceleration(int j, double *spds) override;
    bool getEncoderAccelerations(double *accs) override;
};

#endif // YARP_DEV_IMPLEMENTENCODERSTIMED_H
