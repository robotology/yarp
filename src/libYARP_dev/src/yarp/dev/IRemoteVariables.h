/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IREMOTEVARIABLES_H
#define YARP_DEV_IREMOTEVARIABLES_H

#include <string>

#include <yarp/os/Bottle.h>
#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {
class IRemoteVariablesRaw;
class IRemoteVariables;
}

/**
 * @ingroup dev_iface_motor_raw
 *
 * IRemoteVariablesRaw interface.
 */
class YARP_dev_API yarp::dev::IRemoteVariablesRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IRemoteVariablesRaw() {}

    virtual yarp::dev::ReturnValue getRemoteVariableRaw(std::string key, yarp::os::Bottle& val) = 0;

    virtual yarp::dev::ReturnValue setRemoteVariableRaw(std::string key, const yarp::os::Bottle& val) = 0;

    virtual yarp::dev::ReturnValue getRemoteVariablesListRaw(yarp::os::Bottle* listOfKeys) = 0;
};

/**
 * @ingroup dev_iface_motor
 *
 * IRemoteVariables interface.
 */
class YARP_dev_API yarp::dev::IRemoteVariables
{
public:
    /**
     * Destructor.
     */
    virtual ~IRemoteVariables() {}

    virtual yarp::dev::ReturnValue getRemoteVariable(std::string key, yarp::os::Bottle& val) = 0;

    virtual yarp::dev::ReturnValue setRemoteVariable(std::string key, const yarp::os::Bottle& val) = 0;

    virtual yarp::dev::ReturnValue getRemoteVariablesList(yarp::os::Bottle* listOfKeys) = 0;
};

constexpr yarp::conf::vocab32_t VOCAB_REMOTE_VARIABILE_INTERFACE   = yarp::os::createVocab32('i','v','a','r');
constexpr yarp::conf::vocab32_t VOCAB_VARIABLE                     = yarp::os::createVocab32('m','v','a','r');
constexpr yarp::conf::vocab32_t VOCAB_LIST_VARIABLES               = yarp::os::createVocab32('l','v','a','r');


#endif // YARP_DEV_IREMOTEVARIABLES_H
