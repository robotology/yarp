/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
                localization_status_not_yet_localized = yarp::os::createVocab32('l', 'o', 'c', 'n'),
                localization_status_localized_ok      = yarp::os::createVocab32('l', 'o', 'c', 'y'),
                localization_status_error             = yarp::os::createVocab32('e', 'r', 'r')
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

constexpr yarp::conf::vocab32_t VOCAB_INAVIGATION            = yarp::os::createVocab32('i','n','a','v');

constexpr yarp::conf::vocab32_t VOCAB_NAV_GOTOABS            = yarp::os::createVocab32('s','a','b','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GOTOREL            = yarp::os::createVocab32('s','r','e','l');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GOTOABS_AND_NAME   = yarp::os::createVocab32('s','a','b','n');
constexpr yarp::conf::vocab32_t VOCAB_NAV_VELOCITY_CMD       = yarp::os::createVocab32('v', 'e', 'l', 'c');
constexpr yarp::conf::vocab32_t VOCAB_NAV_RECOMPUTE_PATH     = yarp::os::createVocab32('r','c','m','p');

constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_X                 = yarp::os::createVocab32('g','e','t');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_LIST_X            = yarp::os::createVocab32('l','i','s','t');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_ABS_TARGET        = yarp::os::createVocab32('g','a','b','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_REL_TARGET        = yarp::os::createVocab32('g','r','e','l');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_NAME_TARGET       = yarp::os::createVocab32('g','n','a','m');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_ESTIMATED_ODOM    = yarp::os::createVocab32('g','o','d','m');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_CURRENT_POS       = yarp::os::createVocab32('g','p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_SET_INITIAL_POS       = yarp::os::createVocab32('i','p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_CURRENT_POSCOV    = yarp::os::createVocab32('g','c','o','v');
constexpr yarp::conf::vocab32_t VOCAB_NAV_SET_INITIAL_POSCOV    = yarp::os::createVocab32('i','c','o','v');
constexpr yarp::conf::vocab32_t VOCAB_NAV_LOCALIZATION_START    = yarp::os::createVocab32('l','c','g','o');
constexpr yarp::conf::vocab32_t VOCAB_NAV_LOCALIZATION_STOP     = yarp::os::createVocab32('l','c','s','t');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_NAVIGATION_STATUS = yarp::os::createVocab32('n','s','t','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_LOCALIZER_STATUS  = yarp::os::createVocab32('l','s','t','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_LOCALIZER_POSES   = yarp::os::createVocab32('l','p','s','s');
constexpr yarp::conf::vocab32_t VOCAB_NAV_CLEARALL_X               = yarp::os::createVocab32('c','l','r');
constexpr yarp::conf::vocab32_t VOCAB_NAV_DELETE_X              = yarp::os::createVocab32('d','e','l');
constexpr yarp::conf::vocab32_t VOCAB_NAV_RENAME_X              = yarp::os::createVocab32('r','e','n','m');
constexpr yarp::conf::vocab32_t VOCAB_NAV_STORE_X               = yarp::os::createVocab32('s','t','o','r');
constexpr yarp::conf::vocab32_t VOCAB_NAV_AREA                  = yarp::os::createVocab32('a','r','e','a');
constexpr yarp::conf::vocab32_t VOCAB_NAV_LOCATION              = yarp::os::createVocab32('l','o','c');
constexpr yarp::conf::vocab32_t VOCAB_NAV_PATH                  = yarp::os::createVocab32('p','a','t','h');
constexpr yarp::conf::vocab32_t VOCAB_NAV_TEMPORARY_FLAGS       = yarp::os::createVocab32('t','f','l','g');


#endif // YARP_DEV_ILOCALIZATION2D_H
