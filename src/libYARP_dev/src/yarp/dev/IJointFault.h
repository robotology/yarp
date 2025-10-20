/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IJOINTFAULT_H
#define YARP_DEV_IJOINTFAULT_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {
class IJointFaultRaw;
class IJointFault;
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for getting info about the fault which may occur on a robot.
 */
class YARP_dev_API yarp::dev::IJointFault
{
public:
    virtual ~IJointFault(){}
    virtual yarp::dev::ReturnValue getLastJointFault(int j, int& fault, std::string& message)=0;
};

/**
 * @ingroup dev_iface_motor_raw
 *
 * Interface for getting info about the fault which may occur on a robot.
 */
class YARP_dev_API yarp::dev::IJointFaultRaw
{
public:
    virtual ~IJointFaultRaw(){}
    virtual yarp::dev::ReturnValue getLastJointFaultRaw(int j, int& mode, std::string& message)=0;
};

constexpr yarp::conf::vocab32_t  VOCAB_IJOINTFAULT             =    yarp::os::createVocab32('i','j','f','l');
constexpr yarp::conf::vocab32_t  VOCAB_JF_GET_JOINTFAULT       =    yarp::os::createVocab32('j','h','f','l');

#endif // YARP_DEV_IJOINTFAULT_H
