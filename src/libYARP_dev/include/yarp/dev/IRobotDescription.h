/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IROBOTDESCRIPTION_H
#define YARP_DEV_IROBOTDESCRIPTION_H

#include <yarp/dev/api.h>
#include <yarp/os/Vocab.h>
#include <vector>
#include <string>

namespace yarp {
    namespace dev {
        class IRobotDescription;

        struct DeviceDescription
        {
            std::string  device_name;
            std::string  device_type;
            bool operator ==(DeviceDescription const& b) const
            {
                return this->device_name == b.device_name && this->device_type == b.device_type;
            }
        };
      }
}

/**
 * @ingroup dev_iface_motor
 *
 * This interface allows users to retrieve a list which contains the names and the types of the currently running devices.
 * This list is stored in a RobotDescriptionsServer class. The user can access to the data opening a RobotDescriptionsClient in its own module.
 */
class YARP_dev_API yarp::dev::IRobotDescription
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

constexpr yarp::conf::vocab32_t VOCAB_IROBOT_DESCRIPTION            = yarp::os::createVocab('i','r','o','b');
constexpr yarp::conf::vocab32_t VOCAB_IROBOT_GET                    = yarp::os::createVocab('g','e','t');
constexpr yarp::conf::vocab32_t VOCAB_IROBOT_SET                    = yarp::os::createVocab('s','e','t');
constexpr yarp::conf::vocab32_t VOCAB_IROBOT_DELETE                 = yarp::os::createVocab('d','e','l');
constexpr yarp::conf::vocab32_t VOCAB_IROBOT_ALL                    = yarp::os::createVocab('a','l','l');
constexpr yarp::conf::vocab32_t VOCAB_IROBOT_DEVICE                 = yarp::os::createVocab('d','e','v');
constexpr yarp::conf::vocab32_t VOCAB_IROBOT_BY_TYPE                = yarp::os::createVocab('t','y','p','e');

#endif // YARP_DEV_IROBOTDESCRIPTION_H
