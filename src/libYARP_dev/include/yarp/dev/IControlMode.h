// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo and Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARP_CONTROL_MODE_INTERFACE__
#define __YARP_CONTROL_MODE_INTERFACE__

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

    /**
    * Set position mode, single axis.
    * @param j joint number
    * @return: true/false success failure.
    */
    virtual bool setPositionMode(int j)=0;

    /**
    * Set velocity mode, single axis.
    * @param j joint number
    * @return: true/false success failure.
    */
    virtual bool setVelocityMode(int j)=0;

    /**
    * Set torque mode, single axis.
    * @param j joint number
    * @return: true/false success failure.
    */
    virtual bool setTorqueMode(int j)=0;

    /**
    * Set impedance position mode, single axis.
    * @param j joint number
    * @return: true/false success failure.
    */
    virtual bool setImpedancePositionMode(int j)=0;

    /**
    * Set impedance velocity mode, single axis.
    * @param j joint number
    * @return: true/false success failure.
    */
    virtual bool setImpedanceVelocityMode(int j)=0;

    /**
    * Set open loop mode, single axis.
    * @param j joint number
    * @return: true/false success failure.
    */
    virtual bool setOpenLoopMode(int j)=0;

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

    virtual bool setPositionModeRaw(int j)=0;
    virtual bool setVelocityModeRaw(int j)=0;
    virtual bool setTorqueModeRaw(int j)=0;
    virtual bool setImpedancePositionModeRaw(int j)=0;
    virtual bool setImpedanceVelocityModeRaw(int j)=0;
    virtual bool setOpenLoopModeRaw(int j)=0;
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
#define VOCAB_CM_OPENLOOP           VOCAB4('o','p','e','n')
#define VOCAB_CM_IMPEDANCE_POS      VOCAB4('i','m','p','o')  // deprecated
#define VOCAB_CM_IMPEDANCE_VEL      VOCAB4('i','m','v','e')  // deprecated

// Read only (cannot be set from user)
#define VOCAB_CM_UNKNOWN            VOCAB4('u','n','k','w')

#endif


