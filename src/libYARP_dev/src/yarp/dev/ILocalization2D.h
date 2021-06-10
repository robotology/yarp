/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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
#include <yarp/dev/OdometryData.h>
#include <yarp/sig/Matrix.h>
#include <vector>

namespace yarp {
    namespace dev {
        namespace Nav2D {
            class ILocalization2D;

            enum LocalizationStatusEnum
            {
                localization_status_not_yet_localized = yarp::os::createVocab('l', 'o', 'c', 'n'),
                localization_status_localized_ok      = yarp::os::createVocab('l', 'o', 'c', 'y'),
                localization_status_error             = yarp::os::createVocab('e', 'r', 'r')
            };
        }
    }
}

/**
 * @ingroup dev_iface_navigation
 *
 * ILocalization2D interface. Provides methods to obtain the pose of the robot in a known map.
 */
class YARP_dev_API yarp::dev::Nav2D::ILocalization2D
{
public:
    /**
     * Destructor.
     */
    virtual ~ILocalization2D() {}

    /**
    * Starts the localization service
    * @return true/false
    */
    virtual bool   startLocalizationService() = 0;

    /**
    * Stops the localization service
    * @return true/false
    */
    virtual bool   stopLocalizationService() = 0;

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
    * Gets the current position of the robot w.r.t world reference frame, plus the covariance
    * @param loc the location of the robot
    * @param cov the 3x3 covariance matrix
    * @return true/false
    */
    virtual bool   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation& loc, yarp::sig::Matrix& cov) = 0;

    /**
    * Gets the estimated odometry the robot, including its velocity expressed in the world and in the local reference frame.
    * @param loc the estimated odometry.
    * @return true/false
    */
    virtual bool   getEstimatedOdometry(yarp::dev::OdometryData& odom) = 0;


    /**
     * Sets the initial pose for the localization algorithm which estimates the current position of the robot w.r.t world reference frame.
     * @param loc the location of the robot
     * @return true/false
     */
    virtual bool   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc) = 0;

    /**
    * Sets the initial pose for the localization algorithm which estimates the current position of the robot w.r.t world reference frame.
    * @param loc the location of the robot
    * @param cov the 3x3 covariance matrix
    * @return true/false
    */
    virtual bool   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc, const yarp::sig::Matrix& cov) = 0;
};

constexpr yarp::conf::vocab32_t VOCAB_INAVIGATION            = yarp::os::createVocab('i','n','a','v');

constexpr yarp::conf::vocab32_t VOCAB_NAV_GOTOABS            = yarp::os::createVocab('s','a','b','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GOTOREL            = yarp::os::createVocab('s','r','e','l');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GOTOABS_AND_NAME   = yarp::os::createVocab('s','a','b','n');
constexpr yarp::conf::vocab32_t VOCAB_NAV_VELOCITY_CMD       = yarp::os::createVocab('v', 'e', 'l', 'c');
constexpr yarp::conf::vocab32_t VOCAB_NAV_RECOMPUTE_PATH     = yarp::os::createVocab('r','c','m','p');

constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_X                 = yarp::os::createVocab('g','e','t');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_LIST_X            = yarp::os::createVocab('l','i','s','t');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_ABS_TARGET        = yarp::os::createVocab('g','a','b','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_REL_TARGET        = yarp::os::createVocab('g','r','e','l');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_NAME_TARGET       = yarp::os::createVocab('g','n','a','m');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_ESTIMATED_ODOM    = yarp::os::createVocab('g','o','d','m');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_CURRENT_POS       = yarp::os::createVocab('g','p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_SET_INITIAL_POS       = yarp::os::createVocab('i','p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_CURRENT_POSCOV    = yarp::os::createVocab('g','c','o','v');
constexpr yarp::conf::vocab32_t VOCAB_NAV_SET_INITIAL_POSCOV    = yarp::os::createVocab('i','c','o','v');
constexpr yarp::conf::vocab32_t VOCAB_NAV_LOCALIZATION_START    = yarp::os::createVocab('l','c','g','o');
constexpr yarp::conf::vocab32_t VOCAB_NAV_LOCALIZATION_STOP     = yarp::os::createVocab('l','c','s','t');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_NAVIGATION_STATUS = yarp::os::createVocab('n','s','t','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_LOCALIZER_STATUS  = yarp::os::createVocab('l','s','t','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_LOCALIZER_POSES   = yarp::os::createVocab('l','p','s','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_CLEARALL_X               = yarp::os::createVocab('c','l','r');
constexpr yarp::conf::vocab32_t VOCAB_NAV_DELETE_X              = yarp::os::createVocab('d','e','l');
constexpr yarp::conf::vocab32_t VOCAB_NAV_RENAME_X              = yarp::os::createVocab('r','e','n','m');
constexpr yarp::conf::vocab32_t VOCAB_NAV_STORE_X               = yarp::os::createVocab('s','t','o','r');
constexpr yarp::conf::vocab32_t VOCAB_NAV_AREA                  = yarp::os::createVocab('a','r','e','a');
constexpr yarp::conf::vocab32_t VOCAB_NAV_LOCATION              = yarp::os::createVocab('l','o','c');
constexpr yarp::conf::vocab32_t VOCAB_NAV_PATH                  = yarp::os::createVocab('p','a','t','h');
constexpr yarp::conf::vocab32_t VOCAB_NAV_TEMPORARY_FLAGS       = yarp::os::createVocab('t','f','l','g');


#endif // YARP_DEV_ILOCALIZATION2D_H
