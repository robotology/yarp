/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTTORQUECONTROL_H
#define YARP_DEV_IMPLEMENTTORQUECONTROL_H

#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/api.h>

namespace yarp::dev {
class ImplementTorqueControl;
}

namespace yarp::dev::impl {

template <typename T>
class FixedSizeBuffersManager;

} // namespace yarp::dev::impl

class YARP_dev_API yarp::dev::ImplementTorqueControl: public ITorqueControl
{
protected:
    yarp::dev::ITorqueControlRaw *iTorqueRaw;
    void *helper;
    yarp::dev::impl::FixedSizeBuffersManager<int> *intBuffManager;
    yarp::dev::impl::FixedSizeBuffersManager<double> *doubleBuffManager;
    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap, const double *enc, const double *zos, const double *nw, const double* amps, const double* dutys, const double* bemfs, const double* ktaus);

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
    ImplementTorqueControl(yarp::dev::ITorqueControlRaw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementTorqueControl();

    yarp::dev::ReturnValue getAxes(int *ax) override;
    yarp::dev::ReturnValue getRefTorque(int j, double *) override;
    yarp::dev::ReturnValue getRefTorques(double *t) override;
    yarp::dev::ReturnValue setRefTorques(const double *t) override;
    yarp::dev::ReturnValue setRefTorque(int j, double t) override;
    yarp::dev::ReturnValue setRefTorques(const int n_joint, const int *joints, const double *t) override;
    yarp::dev::ReturnValue getTorques(double *t) override;
    yarp::dev::ReturnValue getTorque(int j, double *t) override;
    yarp::dev::ReturnValue setMotorTorqueParams(int j, const yarp::dev::MotorTorqueParameters params) override;
    yarp::dev::ReturnValue getMotorTorqueParams(int j, yarp::dev::MotorTorqueParameters *params) override;
    yarp::dev::ReturnValue getTorqueRange(int j, double *min, double *max) override;
    yarp::dev::ReturnValue getTorqueRanges(double *min, double *max) override;
};

#endif // YARP_DEV_IMPLEMENTTORQUECONTROL_H
