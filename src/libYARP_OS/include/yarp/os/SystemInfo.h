/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_SYSTEMINFO_H
#define YARP_OS_SYSTEMINFO_H

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
     * @brief The LoadInfo stuct holds the current cpu load information
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
     * otherwise, it is equal to the given PID as parameter.
     * @param pid the process (task) PID
     * @return ProcessInfo
     */
    static ProcessInfo getProcessInfo(int pid);

    //static NetworkInfo getNetworkInfo();
};

#endif // YARP_OS_SYSTEMINFO_H

