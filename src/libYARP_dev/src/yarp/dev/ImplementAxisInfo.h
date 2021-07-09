/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTAXISINFO_H
#define YARP_DEV_IMPLEMENTAXISINFO_H

#include <yarp/dev/ControlBoardInterfaces.h>

namespace yarp {
    namespace dev {
        class ImplementAxisInfo;
    }
}

class YARP_dev_API yarp::dev::ImplementAxisInfo: public IAxisInfo
{
protected:
    IAxisInfoRaw *iinfo;
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
    ImplementAxisInfo(yarp::dev::IAxisInfoRaw *y);

    virtual ~ImplementAxisInfo();

    bool getAxisName(int axis, std::string& name) override;
    bool getJointType(int axis, yarp::dev::JointTypeEnum& type) override;
};

#endif // YARP_DEV_IMPLEMENTAXISINFO_H
