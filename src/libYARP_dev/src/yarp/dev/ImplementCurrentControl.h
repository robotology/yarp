/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTCURRENTCONTROL_H
#define YARP_DEV_IMPLEMENTCURRENTCONTROL_H

#include <yarp/dev/ICurrentControl.h>
#include <yarp/dev/api.h>

namespace yarp::dev {
class ImplementCurrentControl;
}

namespace yarp::dev::impl {

template <typename T>
class FixedSizeBuffersManager;

} // namespace yarp::dev::impl

class YARP_dev_API yarp::dev::ImplementCurrentControl: public ICurrentControl
{
protected:
    yarp::dev::ICurrentControlRaw *iCurrentRaw;
    void *helper;
    yarp::dev::impl::FixedSizeBuffersManager<int> *intBuffManager;
    yarp::dev::impl::FixedSizeBuffersManager<double> *doubleBuffManager;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize(int size, const int *amap, const double* ampsToSens);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize();

public:
    /* Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementCurrentControl(yarp::dev::ICurrentControlRaw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementCurrentControl();

    yarp::dev::ReturnValue getNumberOfMotors(int *ax) override;
    yarp::dev::ReturnValue getRefCurrent(int j, double *) override;
    yarp::dev::ReturnValue getRefCurrents(double *t) override;
    yarp::dev::ReturnValue setRefCurrents(const double *t) override;
    yarp::dev::ReturnValue setRefCurrent(int j, double t) override;
    yarp::dev::ReturnValue setRefCurrents(const int n_joint, const int *joints, const double *t) override;
    yarp::dev::ReturnValue getCurrents(double *t) override;
    yarp::dev::ReturnValue getCurrent(int j, double *t) override;
    yarp::dev::ReturnValue getCurrentRange(int j, double *min, double *max) override;
    yarp::dev::ReturnValue getCurrentRanges(double *min, double *max) override;
};

#endif // YARP_DEV_IMPLEMENTCURRENTCONTROL_H
