/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTPWMCONTROL_H
#define YARP_DEV_IMPLEMENTPWMCONTROL_H

#include <yarp/dev/IPWMControl.h>
#include <yarp/dev/api.h>
#include <yarp/conf/system.h>

namespace yarp {
    namespace dev {
        class ImplementPWMControl;
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

class YARP_dev_API yarp::dev::ImplementPWMControl: public IPWMControl
{
    void *helper;
    yarp::dev::IPWMControlRaw *raw;
    yarp::dev::impl::FixedSizeBuffersManager<double> *doubleBuffManager;
public:
    bool initialize(int k, const int *amap, const double* dutyToPWM);
    bool uninitialize();
    ImplementPWMControl(IPWMControlRaw *v);
    ~ImplementPWMControl();
    bool getNumberOfMotors(int *ax) override;
    bool setRefDutyCycle(int j, double v) override;
    bool setRefDutyCycles(const double *v) override;
    bool getRefDutyCycle(int j, double *v) override;
    bool getRefDutyCycles(double *v) override;
    bool getDutyCycle(int j, double *v) override;
    bool getDutyCycles(double *v) override;

};

#endif // YARP_DEV_IMPLEMENTPWMCONTROL_H
