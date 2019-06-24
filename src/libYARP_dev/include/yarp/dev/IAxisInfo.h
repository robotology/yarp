/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IAXISINFO_H
#define YARP_DEV_IAXISINFO_H

#include <string>

#include <yarp/os/Log.h>
#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>

/*! \file IAxisInfo.h define control board standard interfaces*/

namespace yarp
{
    namespace dev
    {
        class IAxisInfo;
        class IAxisInfoRaw;

        enum JointTypeEnum
        {
            VOCAB_JOINTTYPE_REVOLUTE  = yarp::os::createVocab('a', 't', 'r', 'v'),
            VOCAB_JOINTTYPE_PRISMATIC = yarp::os::createVocab('a', 't', 'p', 'r'),
            VOCAB_JOINTTYPE_UNKNOWN   = yarp::os::createVocab('u', 'n', 'k', 'n')
        };
    }
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for getting information about specific axes, if available.
 */
class YARP_dev_API yarp::dev::IAxisInfo
{
public:
    /**
     * Destructor.
     */
    virtual ~IAxisInfo() {}

    /* Get the name for a particular axis.
    * @param axis joint number
    * @param name the axis name
    * @return true if everything goes fine, false otherwise.
    */
    virtual bool getAxisName(int axis, std::string& name) = 0;

    /* Get the joint type (e.g. revolute/prismatic) for a particular axis.
    * @param axis joint number
    * @param type the joint type
    * @return true if everything goes fine, false otherwise.
    */
    virtual bool getJointType(int axis, yarp::dev::JointTypeEnum& type) { yFatal("getJointType() not implemented on your device, cannot proceed further. Please report the problem on yarp issue tracker"); return false; }
};

/**
* Interface for getting information about specific axes, if available.
*/
class YARP_dev_API yarp::dev::IAxisInfoRaw
{
public:
    /**
    * Destructor.
    */
    virtual ~IAxisInfoRaw() {}

    /* Get the name for a particular axis.
    * @param axis joint number
    * @param name the axis name
    * @return true if everything goes fine, false otherwise.
    */
    virtual bool getAxisNameRaw(int axis, std::string& name) = 0;

    /* Get the joint type (e.g. revolute/prismatic) for a particular axis.
    * @param axis joint number
    * @param type the joint type
    * @return true if everything goes fine, false otherwise.
    */
    virtual bool getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type)  { yFatal("getJointType() not implemented on your device, cannot proceed further. Please report the problem on yarp issue tracker"); return false; };
};

// interface IAxisInfo
constexpr yarp::conf::vocab32_t VOCAB_INFO_NAME            = yarp::os::createVocab('n','a','m','e');
constexpr yarp::conf::vocab32_t VOCAB_INFO_TYPE            = yarp::os::createVocab('t','y','p','e');

#endif // YARP_DEV_IAXISINFO_H
