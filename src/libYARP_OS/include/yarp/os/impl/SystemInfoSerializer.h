/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2010 RobotCub Consortium
 *              Italian Institute of Technology (IIT)
 *              Via Morego 30, 16163, 
 *              Genova, Italy
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 */

#ifndef _SYSTEMINFOSERIALIZER_H_
#define _SYSTEMINFOSERIALIZER_H_

#include <yarp/os/Portable.h>
#include <yarp/os/api.h>
#include <yarp/os/impl/SystemInfo.h>

namespace yarp {
  namespace os {
      namespace impl{
        class SystemInfoSerializer;
      }
  }
}

/**
 * Class SystemInfoSerializer
 */
class YARP_OS_impl_API yarp::os::impl::SystemInfoSerializer: public yarp::os::Portable 
{
public:
    SystemInfoSerializer(){}
    virtual ~SystemInfoSerializer(){}

    virtual bool read(yarp::os::ConnectionReader& connection);
    virtual bool write(yarp::os::ConnectionWriter& connection);

public:
    MemoryInfo memory;
    StorageInfo storage;
    NetworkInfo network;
    ProcessorInfo processor;
    PlatformInfo platform;
    LoadInfo load;
    UserInfo user;
};

#endif
