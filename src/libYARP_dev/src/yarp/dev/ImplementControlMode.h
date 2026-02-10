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

template <typename T>
class VectorBufferManager;

} // namespace yarp::dev::impl

class YARP_dev_API yarp::dev::ImplementControlMode: public IControlMode
{
private:
    void *helper = nullptr;
    yarp::dev::IControlModeRaw *raw = nullptr;

    std::mutex                                         m_imp_mutex;
    std::vector<int>                                   m_vectorInt_tmp;
    std::vector<yarp::dev::ControlModeEnum>            m_vectorCM_tmp;
    std::vector<yarp::dev::SelectableControlModeEnum>  m_vectorSCM_tmp;

public:
    bool initialize(int k, const int *amap);
    bool uninitialize();
    ImplementControlMode(IControlModeRaw *v);
    ~ImplementControlMode();
    yarp::dev::ReturnValue getAvailableControlModes(int j, std::vector<yarp::dev::SelectableControlModeEnum>& avail) override;
    yarp::dev::ReturnValue getControlMode(int j, yarp::dev::ControlModeEnum& mode) override;
    yarp::dev::ReturnValue getControlModes(std::vector<int> joints, std::vector<yarp::dev::ControlModeEnum>& modes) override;
    yarp::dev::ReturnValue getControlModes(std::vector<yarp::dev::ControlModeEnum>& modes) override;
    yarp::dev::ReturnValue setControlMode(int j, yarp::dev::SelectableControlModeEnum mode) override;
    yarp::dev::ReturnValue setControlModes(std::vector<int> joints, std::vector<yarp::dev::SelectableControlModeEnum> modes) override;
    yarp::dev::ReturnValue setControlModes(std::vector<yarp::dev::SelectableControlModeEnum> modes) override;

};

#endif // YARP_DEV_IMPLEMENTCONTROLMODE_H
