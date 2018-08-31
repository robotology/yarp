/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ILOCALIZATION2D_H
#define YARP_DEV_ILOCALIZATION2D_H

#include <yarp/os/Vocab.h>
#include <yarp/os/Log.h>
#include <yarp/dev/Map2DLocation.h>

namespace yarp {
    namespace dev {
        class ILocalization2D;
    }
}

/**
 * @ingroup dev_iface_navigation
 *
 * ILocalization2D interface. Provides methods to obtain the pose of the robot in a known map.
 */
class yarp::dev::ILocalization2D
{
public:
    /**
     * Destructor.
     */
    virtual ~ILocalization2D() {}

    /**
    * Gets the current position of the robot w.r.t world reference frame
    * @param loc the location of the robot
    * @return true/false
    */
    virtual bool   getCurrentPosition(yarp::dev::Map2DLocation& loc) = 0;

    /**
    * Sets the initial pose for the localization algorithm which estimates the current position of the robot w.r.t world reference frame.
    * @param loc the location of the robot
    * @return true/false
    */
    virtual bool   setInitialPose(yarp::dev::Map2DLocation& loc) = 0;
};

constexpr yarp::conf::vocab32_t VOCAB_INAVIGATION            = yarp::os::createVocab('i','n','a','v');

constexpr yarp::conf::vocab32_t VOCAB_NAV_GOTOABS            = yarp::os::createVocab('s','a','b','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GOTOREL            = yarp::os::createVocab('s','r','e','l');

constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_LOCATION       = yarp::os::createVocab('g','l','o','c');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_LOCATION_LIST  = yarp::os::createVocab('l','i','s','t');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_ABS_TARGET     = yarp::os::createVocab('g','a','b','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_REL_TARGET     = yarp::os::createVocab('g','r','e','l');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_NAME_TARGET    = yarp::os::createVocab('g','n','a','m');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_CURRENT_POS    = yarp::os::createVocab('g','p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_SET_INITIAL_POS    = yarp::os::createVocab('i','p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_STATUS         = yarp::os::createVocab('g','s','t','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_CLEAR              = yarp::os::createVocab('c','l','r','l');
constexpr yarp::conf::vocab32_t VOCAB_NAV_DELETE             = yarp::os::createVocab('d','e','l','l');
constexpr yarp::conf::vocab32_t VOCAB_NAV_STORE_ABS          = yarp::os::createVocab('s','t','o','a');

#endif // YARP_DEV_ILOCALIZATION2D_H
