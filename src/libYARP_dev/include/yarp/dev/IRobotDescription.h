/*
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_IROBOTDESCRIPTION_H
#define YARP_DEV_IROBOTDESCRIPTION_H

#include <yarp/os/Vocab.h>
#include <yarp/os/Log.h>
#include <vector>
#include <string>

namespace yarp {
    namespace dev {
        class IRobotDescription;

        struct RobotDescription
        {
            std::string  device_name;
            std::string  device_type;
        };
      }
}

/**
 * @ingroup dev_iface_motor
 *
 * Control board, encoder interface.
 */
class yarp::dev::IRobotDescription
{
public:
    /**
     * Destructor.
     */
    virtual ~IRobotDescription() {}

    /**
     * Ask the complete list of all yarp device drivers registered by a robot description server.
     */
    virtual bool getAllDevices(std::vector<RobotDescription>& dev_list) = 0;

    /**
    * Ask a list of all yarp control board wrappers registered by a robot description server.
    */
    virtual bool getControlBoardWrapperDevices(std::vector<RobotDescription>& dev_list) = 0;

    /**
    * Register a new running yarp device into a robot description server.
    */
    virtual bool registerDevice(const RobotDescription& dev) = 0;
};

#define VOCAB_IROBOT_DESCRIPTION            VOCAB4('i','r','o','b')
#define VOCAB_IROBOT_GET                    VOCAB3('g','e','t')
#define VOCAB_IROBOT_SET                    VOCAB3('s','e','t')
#define VOCAB_IROBOT_ALL                    VOCAB3('a','l','l')
#define VOCAB_IROBOT_DEVICE                 VOCAB3('d','e','v')
#define VOCAB_IROBOT_CONTROLBOARD_WRAPPERS  VOCAB4('c','w','r','p')

#endif // YARP_DEV_IROBOTDESCRIPTION_H
