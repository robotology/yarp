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


#ifndef _SYSTEMINFO_H_
#define _SYSTEMINFO_H_

#include <yarp/os/ConstString.h>
#include <yarp/os/Property.h>


namespace yarp {
  namespace os {
      namespace impl{
        class SystemInfo;
      }
  }
}


typedef int capacity_t;


typedef struct _MemoryInfo {
    capacity_t totalSpace;
    capacity_t freeSpace;
} MemoryInfo;


typedef struct _StorageInfo {
    capacity_t totalSpace;
    capacity_t freeSpace;
} StorageInfo;


typedef struct _NetworkInfo {
    yarp::os::ConstString mac;
    yarp::os::ConstString ip4;
    yarp::os::ConstString ip6;
} NetworkInfo;


typedef struct _ProcessorInfo {
    yarp::os::ConstString architecture;
    yarp::os::ConstString model;
    yarp::os::ConstString vendor;
    int family;
    int modelNumber;
    int cores;
    int siblings;
    double frequency;
} ProcessorInfo;


typedef struct _LoadInfo {
    double cpuLoad1; 
    double cpuLoad5;
    double cpuLoad15;
    int cpuLoadInstant;
} LoadInfo;


typedef struct _PlatformInfo {
    yarp::os::ConstString name;
    yarp::os::ConstString distribution;
    yarp::os::ConstString release;
    yarp::os::ConstString codename;
    yarp::os::ConstString kernel;
    yarp::os::Property environmentVars;
} PlatformInfo;

typedef struct _UserInfo {
    yarp::os::ConstString userName;
    yarp::os::ConstString realName;
    yarp::os::ConstString homeDir;
    int userID;
} UserInfo;

/**
 * Class SystemInfo
 */
class YARP_OS_impl_API yarp::os::impl::SystemInfo 
{
public:   
    static MemoryInfo getMemoryInfo(void);
    static StorageInfo getStorageInfo(void);
    static NetworkInfo getNetworkInfo(void);
    static ProcessorInfo getProcessorInfo(void);
    static PlatformInfo getPlatformInfo(void);
    static LoadInfo getLoadInfo(void);
    static UserInfo getUserInfo(void);
#if defined(WIN32)
    static void enableCpuLoadCollector(void);
    static void disableCpuLoadCollector(void);    
#endif    
};

#endif //_SYSTEMINFO_H_

