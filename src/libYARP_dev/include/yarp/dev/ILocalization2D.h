/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ILOCALIZATION2D_H
#define YARP_DEV_ILOCALIZATION2D_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/Map2DLocation.h>
#include <vector>

namespace yarp {
    namespace dev {
        class ILocalization2D;

        enum LocalizationStatusEnum
        {
            localization_status_not_yet_localized = yarp::os::createVocab('l', 'o', 'c', 'n'),
            localization_status_localized_ok      = yarp::os::createVocab('l', 'o', 'c', 'y'),
            localization_status_error             = yarp::os::createVocab('e', 'r', 'r')
        };
    }
}

/**
 * @ingroup dev_iface_navigation
 *
 * ILocalization2D interface. Provides methods to obtain the pose of the robot in a known map.
 */
class YARP_dev_API yarp::dev::ILocalization2D
{
public:
    /**
     * Destructor.
     */
    virtual ~ILocalization2D() {}

    /**
     * Gets the current status of the localization task.
     * @return true/false
     */
    virtual bool   getLocalizationStatus(LocalizationStatusEnum& status) = 0;

    /**
     * Gets a set of pose estimates computed by the localization algorithm.
     * @return true/false
     */
    virtual bool   getEstimatedPoses(std::vector<yarp::dev::Nav2D::Map2DLocation>& poses) = 0;

    /**
     * Gets the current position of the robot w.r.t world reference frame
     * @param loc the location of the robot
     * @return true/false
     */
    virtual bool   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation& loc) = 0;

    /**
     * Sets the initial pose for the localization algorithm which estimates the current position of the robot w.r.t world reference frame.
     * @param loc the location of the robot
     * @return true/false
     */
    virtual bool   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc) = 0;
};

constexpr yarp::conf::vocab32_t VOCAB_INAVIGATION            = yarp::os::createVocab('i','n','a','v');

constexpr yarp::conf::vocab32_t VOCAB_NAV_GOTOABS            = yarp::os::createVocab('s','a','b','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GOTOREL            = yarp::os::createVocab('s','r','e','l');
constexpr yarp::conf::vocab32_t VOCAB_NAV_VELOCITY_CMD       = yarp::os::createVocab('v', 'e', 'l', 'c');
constexpr yarp::conf::vocab32_t VOCAB_NAV_RECOMPUTE_PATH     = yarp::os::createVocab('r','c','m','p');

constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_X                 = yarp::os::createVocab('g','e','t');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_LIST_X            = yarp::os::createVocab('l','i','s','t');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_ABS_TARGET        = yarp::os::createVocab('g','a','b','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_REL_TARGET        = yarp::os::createVocab('g','r','e','l');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_NAME_TARGET       = yarp::os::createVocab('g','n','a','m');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_CURRENT_POS       = yarp::os::createVocab('g','p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_SET_INITIAL_POS       = yarp::os::createVocab('i','p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_NAVIGATION_STATUS = yarp::os::createVocab('n','s','t','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_LOCALIZER_STATUS  = yarp::os::createVocab('l','s','t','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_LOCALIZER_POSES   = yarp::os::createVocab('l','p','s','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_CLEAR_X               = yarp::os::createVocab('c','l','r');
constexpr yarp::conf::vocab32_t VOCAB_NAV_DELETE_X              = yarp::os::createVocab('d','e','l');
constexpr yarp::conf::vocab32_t VOCAB_NAV_RENAME_X              = yarp::os::createVocab('r','e','n','m');
constexpr yarp::conf::vocab32_t VOCAB_NAV_STORE_X               = yarp::os::createVocab('s','t','o','r');
constexpr yarp::conf::vocab32_t VOCAB_NAV_AREA                  = yarp::os::createVocab('a','r','e','a');
constexpr yarp::conf::vocab32_t VOCAB_NAV_LOCATION              = yarp::os::createVocab('l','o','c');
constexpr yarp::conf::vocab32_t VOCAB_NAV_PATH                  = yarp::os::createVocab('p','a','t','h');

#endif // YARP_DEV_ILOCALIZATION2D_H
