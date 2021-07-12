/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTCURRENTCONTROL_H
#define YARP_DEV_IMPLEMENTCURRENTCONTROL_H

#include <yarp/dev/ICurrentControl.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class ImplementCurrentControl;
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

    bool getNumberOfMotors(int *ax) override;
    bool getRefCurrent(int j, double *) override;
    bool getRefCurrents(double *t) override;
    bool setRefCurrents(const double *t) override;
    bool setRefCurrent(int j, double t) override;
    bool setRefCurrents(const int n_joint, const int *joints, const double *t) override;
    bool getCurrents(double *t) override;
    bool getCurrent(int j, double *t) override;
    bool getCurrentRange(int j, double *min, double *max) override;
    bool getCurrentRanges(double *min, double *max) override;
};

#endif // YARP_DEV_IMPLEMENTCURRENTCONTROL_H
