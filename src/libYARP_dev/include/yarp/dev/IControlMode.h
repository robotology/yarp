/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ICONTROLMODE_H
#define YARP_DEV_ICONTROLMODE_H

#include <yarp/os/Vocab.h>

namespace yarp {
    namespace dev {
    class IControlModeRaw;
    class IControlMode;
    }
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for setting control mode in control board.
 */
class YARP_dev_API yarp::dev::IControlMode
{
public:
    virtual ~IControlMode(){}

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    /**
    * Set position mode, single axis.
    * @param j joint number
    * @return: true/false success failure.
    * @deprecated since YARP 2.3.70
    */
    YARP_DEPRECATED_MSG("Use IControlMode2::setControlMode(j, VOCAB_CM_POSITION) instead")
    virtual bool setPositionMode(int j) { return false; }
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    /**
    * Set velocity mode, single axis.
    * @param j joint number
    * @return: true/false success failure.
    * @deprecated since YARP 2.3.70
    */
    YARP_DEPRECATED_MSG("Use IControlMode2::setControlMode(j, VOCAB_CM_VELOCITY) instead")
    virtual bool setVelocityMode(int j) { return false; }
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    /**
    * Set torque mode, single axis.
    * @param j joint number
    * @return: true/false success failure.
    * @deprecated since YARP 2.3.70
    */
    YARP_DEPRECATED_MSG("Use IControlMode2::setControlMode(j, VOCAB_CM_TORQUE) instead")
    virtual bool setTorqueMode(int j) { return false; }
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    /**
    * Set impedance position mode, single axis.
    * @param j joint number
    * @return: true/false success failure.
    * @deprecated since YARP 2.3.70
    */
    YARP_DEPRECATED_MSG("Use IControlMode2::setControlMode(j, VOCAB_CM_IMPEDANCE_POS) instead")
    virtual bool setImpedancePositionMode(int j) { return false; }
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    /**
    * Set impedance velocity mode, single axis.
    * @param j joint number
    * @return: true/false success failure.
    * @deprecated since YARP 2.3.70
    */
    YARP_DEPRECATED_MSG("Use IControlMode2::setControlMode(j, VOCAB_CM_IMPEDANCE_VEL) instead")
    virtual bool setImpedanceVelocityMode(int j) { return false; }
#endif // YARP_NO_DEPRECATED

    /**
    * Get the current control mode.
    * @param j joint number
    * @param mode a vocab of the current control mode for joint j.
    * @return: true/false success failure.
    */
    virtual bool getControlMode(int j, int *mode)=0;

    /**
    * Get the current control mode (multiple joints).
    * @param modes a vector containing vocabs for the current control modes of the joints.
    * @return: true/false success failure.
    */
    virtual bool getControlModes(int *modes)=0;
};


/**
 * @ingroup dev_iface_motor
 *
 * Interface for setting control mode in control board. See IControlMode for
 * more documentation.
 */
class yarp::dev::IControlModeRaw
{
public:
    virtual ~IControlModeRaw(){}

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    YARP_DEPRECATED virtual bool setPositionModeRaw(int j) { return false; }
    YARP_DEPRECATED virtual bool setVelocityModeRaw(int j) { return false; }
    YARP_DEPRECATED virtual bool setTorqueModeRaw(int j) { return false; }
    YARP_DEPRECATED virtual bool setImpedancePositionModeRaw(int j) { return false; }
    YARP_DEPRECATED virtual bool setImpedanceVelocityModeRaw(int j) { return false; }
#endif // YARP_NO_DEPRECATED
    virtual bool getControlModeRaw(int j, int *mode)=0;
    virtual bool getControlModesRaw(int* modes)=0;
};


// new style VOCABS
// Interface
#define VOCAB_ICONTROLMODE          VOCAB4('i','c','m','d')

// Methods
#define VOCAB_CM_CONTROL_MODE           VOCAB4('c','m','o','d')
#define VOCAB_CM_CONTROL_MODE_GROUP     VOCAB4('c','m','o','g')
#define VOCAB_CM_CONTROL_MODES          VOCAB4('c','m','d','s')

// Values
// Read / Write
#define VOCAB_CM_IDLE               VOCAB3('i','d','l')
#define VOCAB_CM_TORQUE             VOCAB4('t','o','r','q')
#define VOCAB_CM_POSITION           VOCAB3('p','o','s')
#define VOCAB_CM_POSITION_DIRECT    VOCAB4('p','o','s','d')
#define VOCAB_CM_VELOCITY           VOCAB3('v','e','l')
#define VOCAB_CM_CURRENT            VOCAB4('i','c','u','r')
#define VOCAB_CM_PWM                VOCAB4('i','p','w','m')
#define VOCAB_CM_IMPEDANCE_POS      VOCAB4('i','m','p','o')  // deprecated
#define VOCAB_CM_IMPEDANCE_VEL      VOCAB4('i','m','v','e')  // deprecated

// Read only (cannot be set from user)
#define VOCAB_CM_UNKNOWN            VOCAB4('u','n','k','w')

#endif // YARP_DEV_ICONTROLMODE_H
