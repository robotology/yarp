/*
 *  Copyright: (C) 2010 RobotCub Consortium
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_SYSTEMINFOSERIALIZER_H
#define YARP_OS_SYSTEMINFOSERIALIZER_H

#include <yarp/os/Portable.h>
#include <yarp/os/SystemInfo.h>

namespace yarp {
  namespace os {
        class SystemInfoSerializer;
  }
}

/**
 * \ingroup key_class
 * @brief A helper class to pass the SystemInfo object around the YARP network
 */
class YARP_OS_API yarp::os::SystemInfoSerializer: public yarp::os::Portable
{
public:
    /**
     * @brief SystemInfoSerializer constructor
     */
    SystemInfoSerializer();

    /**
     * @brief ~SystemInfoSerializer deconstructor
     */
    virtual ~SystemInfoSerializer();

    /**
     * @brief reads from a ConnectionReader and fill into the SystemInfo structs.
     * @param connection a ConnectionReader
     * @return true/false upon success or failure
     */
    virtual bool read(yarp::os::ConnectionReader& connection);

    /**
     * @brief write the SystemInfo structs using a ConnectionWriter.
     * @param connection a ConnectionWriter
     * @return true/false upon success or failure
     */
    virtual bool write(yarp::os::ConnectionWriter& connection);

public:
    /**
     * @brief system memory information
     */
    yarp::os::SystemInfo::MemoryInfo memory;

    /**
     * @brief system storage information
     */
    yarp::os::SystemInfo::StorageInfo storage;

    /**
     * @brief system processor type information
     */
    yarp::os::SystemInfo::ProcessorInfo processor;

    /**
     * @brief operating system information
     */
    yarp::os::SystemInfo::PlatformInfo platform;

    /**
     * @brief current cpu load information
     */
    yarp::os::SystemInfo::LoadInfo load;

    /**
     * @brief current user information
     */
    yarp::os::SystemInfo::UserInfo user;

    //yarp::os::SystemInfo::NetworkInfo network;
};

#endif // YARP_OS_SYSTEMINFOSERIALIZER_H
