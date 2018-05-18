/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ICONTROLMODE2_H
#define YARP_DEV_ICONTROLMODE2_H

#include <yarp/dev/IControlMode.h>


namespace yarp {
    namespace dev {
    class IControlMode2Raw;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0

/**
 * @ingroup dev_iface_motor
 *
 * Interface for setting control mode in control board. See IControlMode for
 * more documentation.
 */
class yarp::dev::IControlMode2Raw: public IControlModeRaw
{
public:
    virtual ~IControlMode2Raw(){}
    using IControlModeRaw::getControlModeRaw;
    using IControlModeRaw::getControlModesRaw;

    virtual bool getControlModesRaw(const int n_joint, const int *joints, int *modes)=0;
    virtual bool setControlModeRaw(const int j, const int mode)=0;
    virtual bool setControlModesRaw(const int n_joint, const int *joints, int *modes)=0;
    virtual bool setControlModesRaw(int *modes)=0;

};


YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlMode instead") IControlMode IControlMode2;
#endif
    }
}
#endif // YARP_DEV_ICONTROLMODE2_H
