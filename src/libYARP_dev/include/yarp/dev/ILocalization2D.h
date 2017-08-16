/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
 * @ingroup dev_iface_motor
 *
 * ILocalization2D interface.
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

#define VOCAB_INAVIGATION           VOCAB4('i','n','a','v')

#define VOCAB_NAV_GOTOABS           VOCAB4('s','a','b','s')
#define VOCAB_NAV_GOTOREL           VOCAB4('s','r','e','l')

#define VOCAB_NAV_GET_LOCATION      VOCAB4('g','l','o','c')
#define VOCAB_NAV_GET_LOCATION_LIST VOCAB4('l','i','s','t')
#define VOCAB_NAV_GET_ABS_TARGET    VOCAB4('g','a','b','s')
#define VOCAB_NAV_GET_REL_TARGET    VOCAB4('g','r','e','l')
#define VOCAB_NAV_GET_NAME_TARGET   VOCAB4('g','n','a','m')
#define VOCAB_NAV_GET_CURRENT_POS   VOCAB4('g','p','o','s')
#define VOCAB_NAV_SET_INITIAL_POS   VOCAB4('i','p','o','s')
#define VOCAB_NAV_GET_STATUS        VOCAB4('g','s','t','s')
#define VOCAB_NAV_CLEAR             VOCAB4('c','l','r','l')
#define VOCAB_NAV_DELETE            VOCAB4('d','e','l','l')
#define VOCAB_NAV_STORE_ABS         VOCAB4('s','t','o','a')

#define VOCAB_NAV_STOP              VOCAB4('s','t','o','p')
#define VOCAB_NAV_SUSPEND           VOCAB4('s','u','s','p')
#define VOCAB_NAV_RESUME            VOCAB4('r','e','s','m')

#endif // YARP_DEV_ILOCALIZATION2D_H
