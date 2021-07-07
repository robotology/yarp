/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_CALIBRATORTHREAD_H
#define YARP_ROBOTINTERFACE_CALIBRATORTHREAD_H

#include <yarp/robotinterface/api.h>

#include <yarp/os/Thread.h>

#include <yarp/robotinterface/Types.h>


namespace yarp {

namespace os {
class Semaphore;
} // namespace os

namespace dev {
class ICalibrator;
class DeviceDriver;
} // namespace dev

namespace robotinterface {
namespace impl {

class CalibratorThread : public yarp::os::Thread
{
public:
    enum Action
    {
        ActionCalibrate,
        ActionPark
    };

    CalibratorThread(yarp::dev::ICalibrator* calibrator,
                     const std::string& calibratorName,
                     yarp::dev::DeviceDriver* target,
                     const std::string& targetName,
                     yarp::robotinterface::impl::CalibratorThread::Action action);
    ~CalibratorThread() override;

    void run() override;
    void onStop() override;

private:
    class Private;
    Private* const mPriv;
};

} // namespace impl
} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_CALIBRATORTHREAD_H
