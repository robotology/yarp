/*
* Copyright(C) 2016 iCub Facility
* Authors: Francesco Romano, Andrea Ruzzenenti
* CopyPolicy : Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#ifndef YARP_DEV_IFRAMEPROVIDER_H
#define YARP_DEV_IFRAMEPROVIDER_H

#include <yarp/dev/api.h>

#include <string>
#include <vector>

namespace yarp
{
    namespace dev
    {
        class  IFrameProvider;
        struct FrameComplete;
        enum   IFrameProviderStatus
        {
            IFrameProviderStatusOK      = 0,
            IFrameProviderStatusError   = 1,
            IFrameProviderStatusNoData  = 1 << 1,
            IFrameProviderStatusTimeout = 1 << 2,
        };

    }

    namespace sig
    {
        class Vector;
    }

    namespace math
    {
        class FrameTransform;
    }
}

// 30/03/2017 andrea.ruzzenenti@iit.it : even though it is not so safe and a little unsatisfying,
// providing a bool information of which datum is valid an which not, without decupling the data (so keeping accelerations, velocities position and rotation together)
// was the best way found so far..
#define VALID_POSITION         1;  // 0b00000001
#define VALID_ROTATION         2;  // 0b00000010
#define VALID_LIN_VELOCITY     4;  // 0b00000100
#define VALID_LIN_ACCELERATION 8;  // 0b00001000
#define VALID_ANG_VELOCITY     16; // 0b00010000
#define VALID_ANG_ACCELERATION 32; // 0b00100000

struct yarp::dev::FrameComplete
{
    long                       isValid;
    yarp::math::FrameTransform frame;
    yarp::sig::Vector          linAccelerations;
    yarp::sig::Vector          linVelocities;
    yarp::sig::Vector          angAccelerations;
    yarp::sig::Vector          angVelocities;
};

/**
 * \since 2.3.69
 */

/**
 * Interface representing a provider of frames (and possibly their velocity and acceleration)
 * \since 2.3.69
 */
class yarp::dev::IFrameProvider
{
public:
    typedef std::vector<yarp::dev::FrameComplete> FrameVector;
    
    virtual ~IFrameProvider(){}

    virtual yarp::dev::IFrameProviderStatus getFrameCount(unsigned int& count) = 0;
    virtual yarp::dev::IFrameProviderStatus getFrameAtIndex(const unsigned& frameIndex, yarp::dev::FrameComplete& frame) = 0;
    virtual yarp::dev::IFrameProviderStatus getIndexForFrame(const std::pair<std::string, std::string>& frameKey, unsigned int& index) = 0;
    virtual yarp::dev::IFrameProviderStatus getFrames(FrameVector& frames) = 0;
    
};


#endif /* End of YARP_DEV_IFRAMEPROVIDER_H */
