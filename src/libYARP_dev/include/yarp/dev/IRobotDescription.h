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

        struct DeviceDescription
        {
            std::string  device_name;
            std::string  device_type;
            bool operator ==(DeviceDescription const& b) { return this->device_name == b.device_name && this->device_type == b.device_type; }
        };
      }
}

/**
 * @ingroup dev_iface_motor
 *
 * This interface allows users to retrieve a list which contains the names and the types of the currently running devices.
 * This list is stored in a RobotDescriptionsServer class. The user can access to the data opening a RobotDescriptionsClient in its own module.
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
    virtual bool getAllDevices(std::vector<DeviceDescription>& dev_list) = 0;

    /**
    * Ask a list of all registered yarp device drivers whose type corresponds to the given param.
    */
    virtual bool getAllDevicesByType(const std::string &type, std::vector<DeviceDescription>& dev_list) = 0;

    /**
    * Register a new running yarp device into a robot description server.
    */
    virtual bool registerDevice(const DeviceDescription& dev) = 0;

    /**
    * Unregister a running yarp device from a robot description server.
    */
    virtual bool unregisterDevice(const std::string& device_name) = 0;
};

#define VOCAB_IROBOT_DESCRIPTION            VOCAB4('i','r','o','b')
#define VOCAB_IROBOT_GET                    VOCAB3('g','e','t')
#define VOCAB_IROBOT_SET                    VOCAB3('s','e','t')
#define VOCAB_IROBOT_DELETE                 VOCAB3('d','e','l')
#define VOCAB_IROBOT_ALL                    VOCAB3('a','l','l')
#define VOCAB_IROBOT_DEVICE                 VOCAB3('d','e','v')
#define VOCAB_IROBOT_BY_TYPE                VOCAB4('t','y','p','e')

#endif // YARP_DEV_IROBOTDESCRIPTION_H
