/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IREMOTEVARIABLES_H
#define YARP_DEV_IREMOTEVARIABLES_H

#include <string>

#include <yarp/os/Bottle.h>
#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class IRemoteVariablesRaw;
        class IRemoteVariables;
      }
}

/**
 * @ingroup dev_iface_motor
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

    virtual bool getRemoteVariableRaw(std::string key, yarp::os::Bottle& val) = 0;

    virtual bool setRemoteVariableRaw(std::string key, const yarp::os::Bottle& val) = 0;

    virtual bool getRemoteVariablesListRaw(yarp::os::Bottle* listOfKeys) = 0;
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

    virtual bool getRemoteVariable(std::string key, yarp::os::Bottle& val) = 0;

    virtual bool setRemoteVariable(std::string key, const yarp::os::Bottle& val) = 0;

    virtual bool getRemoteVariablesList(yarp::os::Bottle* listOfKeys) = 0;
};

constexpr yarp::conf::vocab32_t VOCAB_REMOTE_VARIABILE_INTERFACE   = yarp::os::createVocab('i','v','a','r');
constexpr yarp::conf::vocab32_t VOCAB_VARIABLE                     = yarp::os::createVocab('m','v','a','r');
constexpr yarp::conf::vocab32_t VOCAB_LIST_VARIABLES               = yarp::os::createVocab('l','v','a','r');


#endif // YARP_DEV_IREMOTEVARIABLES_H
