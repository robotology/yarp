/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "Types.h"

#include <yarp/os/Thread.h>

namespace yarp { namespace os { class Semaphore; } }
namespace yarp { namespace dev { class ICalibrator; } }
namespace yarp { namespace dev { class DeviceDriver; } }

namespace RobotInterface {

class CalibratorThread: public yarp::os::Thread
{
public:
    enum Action {
        ActionCalibrate,
        ActionPark
    };

    CalibratorThread(yarp::dev::ICalibrator *calibrator,
                     yarp::dev::DeviceDriver *target,
                     RobotInterface::CalibratorThread::Action action,
                     RobotInterface::ThreadList *threadList,
                     yarp::os::Semaphore *threadListSemaphore);
    virtual ~CalibratorThread();

    virtual bool threadInit();
    virtual void run();
    virtual void onStop();

private:
    class Private;
    Private * const mPriv;
};

} // namespace RobotInterface
