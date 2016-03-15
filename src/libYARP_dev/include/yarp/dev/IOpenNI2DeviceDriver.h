/*
 * Copyright (C) 2011 Duarte Aragao
 * Copyright (C) 2013 Konstantinos Theofilis, University of Hertfordshire, k.theofilis@herts.ac.uk
 * Authors: Duarte Aragao, Konstantinos Theofilis
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef IOPENNI2_DRIVER_H
#define IOPENNI2_DRIVER_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>

#ifdef OPENNI2_DRIVER_USES_NITE2
    #include <NiTE.h>
#else
namespace nite{
    typedef int SkeletonState;
}
#endif

//Constants from the OpenNI2SkeletonData
#define MAX_USERS 10
#define TOTAL_JOINTS 15
#define SENSOR_IMG_WIDTH 640
#define SENSOR_IMG_HEIGHT 480

namespace yarp {
    namespace dev {
        class IOpenNI2DeviceDriver;
    }
}
/**
 * @ingroup dev_iface_media
 *
 * Interface for the OpenNI2DeviceServer and OpenNI2DeviceClient.
 * See also http://wiki.icub.org/wiki/OpenNI2
 */
class yarp::dev::IOpenNI2DeviceDriver {
public:
    virtual ~IOpenNI2DeviceDriver() {}

    /**
     * Gets sensor's skeleton joints orientation quarternion
     *
     * @param vectrArray Array of vectors that is filled with the new orientation quartenions
     * @param confidence Array of floats that is filled with the confidence level of each quartenion
     * @param userID id of the user skeleton to be retrieved
     * @return false if the user skeleton is not being tracked
     */
    virtual bool getSkeletonOrientation(yarp::sig::Vector *vectorArray, float *confidence, int userID) = 0;
    /**
     * Gets sensor skeleton joints position 3D vector (in millimeters)
     *
     * @param vectorArray Array of vectors that is filled with the new position vector
     * @param confidence Array of floats that is filled with the confidence level of each vector
     * @param userID id of the user skeleton to be retrieved
     * @return false if the user skeleton is not being tracked
     */
    virtual bool getSkeletonPosition(yarp::sig::Vector *vectorArray, float *confidence, int userID) = 0;


    /**
     * Gets the user skeleton state
     *  - SKELETON_NONE
     *  - SKELETON_CALIBRATING
     *  - SKELETON_TRACKED
     *
     * @param userID id of the user skeleton state to be retrieved
     * @return nite::SkeletonState value of the user skeleton state
     */
    virtual nite::SkeletonState getSkeletonState(int userID) = 0;

    /**
     * Get RGB camera image
     *
     * @return camera image
     */
    virtual yarp::sig::ImageOf<yarp::sig::PixelRgb> getImageFrame() = 0;
    /**
     * Get depth camera image (in millimeters)
     *
     * @return camera image
     */
    virtual yarp::sig::ImageOf<yarp::sig::PixelMono16> getDepthFrame() = 0;
};
#endif
