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


#ifndef _YARP2_SYSTEMINFO_
#define _YARP2_SYSTEMINFO_

#include <yarp/os/ConstString.h>
#include <yarp/os/Property.h>


namespace yarp {
  namespace os {
        class SystemInfo;
  }
}


/**
 * \ingroup key_class
 *
 * \brief A class to get the system (platform) status  such as
 * available memory, storage, CPU load and etc.
 */

class YARP_OS_API yarp::os::SystemInfo
{
public:
    typedef int capacity_t;

    /**
     * @brief The MemoryInfo struct holds the system memory information
     */
    typedef struct MemoryInfo {
        capacity_t totalSpace;
        capacity_t freeSpace;
    } MemoryInfo;


    /**
     * @brief The StorageInfo struct holds the system storage information
     */
    typedef struct StorageInfo {
        capacity_t totalSpace;
        capacity_t freeSpace;
    } StorageInfo;

    /**
     * @brief The ProcessorInfo struct holds the processor information
     */
    typedef struct ProcessorInfo {
        yarp::os::ConstString architecture;
        yarp::os::ConstString model;
        yarp::os::ConstString vendor;
        int family;
        int modelNumber;
        int cores;
        int siblings;
        double frequency;
    } ProcessorInfo;


    /**
     * @brief The LoadInfo stuct holda the current cpu load information
     */
    typedef struct LoadInfo {
        double cpuLoad1;
        double cpuLoad5;
        double cpuLoad15;
        int cpuLoadInstant;
    } LoadInfo;


    /**
     * @brief The PlatformInfo stuct holds the operating system information
     */
    typedef struct PlatformInfo {
        yarp::os::ConstString name;
        yarp::os::ConstString distribution;
        yarp::os::ConstString release;
        yarp::os::ConstString codename;
        yarp::os::ConstString kernel;
        yarp::os::Property environmentVars;
    } PlatformInfo;

    /**
     * @brief The UserInfo stuct holds the current user information
     */
    typedef struct UserInfo {
        yarp::os::ConstString userName;
        yarp::os::ConstString realName;
        yarp::os::ConstString homeDir;
        int userID;
    } UserInfo;

    /*
    typedef struct NetworkInfo {
        yarp::os::ConstString mac;
        yarp::os::ConstString ip4;
        yarp::os::ConstString ip6;
    } NetworkInfo;
    */

    /**
     * @brief The ProcessInfo stuct provides the operating system proccess information.
     */
    typedef struct ProcessInfo {
        yarp::os::ConstString name;
        yarp::os::ConstString arguments;
        int schedPolicy;
        int schedPriority;
        int pid;
    } ProcessInfo;

public:   
    /**
     * @brief getMemoryInfo
     * @return  information about the system memory
     */
    static MemoryInfo getMemoryInfo();

    /**
     * @brief getStorageInfo
     * @return information about the system storage
     */
    static StorageInfo getStorageInfo();

    /**
     * @brief getProcessorInfo
     * @return information about the processor type
     */
    static ProcessorInfo getProcessorInfo();

    /**
     * @brief getPlatformInfo
     * @return  information about the operating system type
     */
    static PlatformInfo getPlatformInfo();

    /**
     * @brief getLoadInfo
     * @return information about the current cpu usage (load)
     */
    static LoadInfo getLoadInfo();

    /**
     * @brief getUserInfo
     * @return information about the current user
     */
    static UserInfo getUserInfo();

    /**
     * @brief gets the operating system process information given by its PID.
     * If the information cannot be retrieved, ProcessInfo.pid is set to -1
     * otherwise, it is equal to the given PID as paramter.
     * @param pid the process (task) PID
     * @return ProcessInfo
     */
    static ProcessInfo getProcessInfo(int pid);

    //static NetworkInfo getNetworkInfo();    
};

#endif //_YARP2_SYSTEMINFO_

