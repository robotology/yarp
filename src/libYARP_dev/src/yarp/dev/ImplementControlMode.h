/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTCONTROLMODE_H
#define YARP_DEV_IMPLEMENTCONTROLMODE_H

#include <yarp/dev/IControlMode.h>
#include <yarp/dev/api.h>

namespace yarp::dev {
class ImplementControlMode;
}

namespace yarp::dev::impl {

template <typename T>
class FixedSizeBuffersManager;

} // namespace yarp::dev::impl

class YARP_dev_API yarp::dev::ImplementControlMode: public IControlMode
{
    void *helper;
    yarp::dev::IControlModeRaw *raw;
    yarp::dev::impl::FixedSizeBuffersManager<int> *buffManager;

public:
    bool initialize(int k, const int *amap);
    bool uninitialize();
    ImplementControlMode(IControlModeRaw *v);
    ~ImplementControlMode();
    yarp::dev::ReturnValue getAvailableControlModes(int j, std::vector<yarp::dev::SelectableControlModeEnum>& avail) override;
    yarp::dev::ReturnValue getControlMode(int j, int *f) override;
    yarp::dev::ReturnValue getControlModes(int *modes) override;
    yarp::dev::ReturnValue getControlModes(const int n_joint, const int *joints, int *modes) override;
    yarp::dev::ReturnValue setControlMode(const int j, const int mode) override;
    yarp::dev::ReturnValue setControlModes(const int n_joint, const int *joints, int *modes) override;
    yarp::dev::ReturnValue setControlModes(int *modes) override;
};

#endif // YARP_DEV_IMPLEMENTCONTROLMODE_H
