/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_SYSTEMINFO_H
#define YARP_OS_SYSTEMINFO_H

#include <yarp/os/Property.h>

#include <string>


namespace yarp {
namespace os {

/**
 * \ingroup key_class
 *
 * \brief A class to get the system (platform) status  such as
 * available memory, storage, CPU load and etc.
 */

class YARP_os_API SystemInfo
{
public:
    typedef int capacity_t;

    /**
     * @brief The MemoryInfo struct holds the system memory information
     */
    typedef struct MemoryInfo
    {
        capacity_t totalSpace;
        capacity_t freeSpace;
    } MemoryInfo;


    /**
     * @brief The StorageInfo struct holds the system storage information
     */
    typedef struct StorageInfo
    {
        capacity_t totalSpace;
        capacity_t freeSpace;
    } StorageInfo;

    /**
     * @brief The ProcessorInfo struct holds the processor information
     */
    typedef struct ProcessorInfo
    {
        std::string architecture;
        std::string model;
        std::string vendor;
        int family;
        int modelNumber;
        int cores;
        int siblings;
        double frequency;
    } ProcessorInfo;


    /**
     * @brief The LoadInfo struct holds the current cpu load information
     */
    typedef struct LoadInfo
    {
        double cpuLoad1;
        double cpuLoad5;
        double cpuLoad15;
        int cpuLoadInstant;
    } LoadInfo;


    /**
     * @brief The PlatformInfo struct holds the operating system information
     */
    typedef struct PlatformInfo
    {
        std::string name;
        std::string distribution;
        std::string release;
        std::string codename;
        std::string kernel;
        yarp::os::Property environmentVars;
    } PlatformInfo;

    /**
     * @brief The UserInfo struct holds the current user information
     */
    typedef struct UserInfo
    {
        std::string userName;
        std::string realName;
        std::string homeDir;
        int userID;
    } UserInfo;

    /*
    typedef struct NetworkInfo {
        std::string mac;
        std::string ip4;
        std::string ip6;
    } NetworkInfo;
    */

    /**
     * @brief The ProcessInfo struct provides the operating system process information.
     */
    typedef struct ProcessInfo
    {
        std::string name;
        std::string arguments;
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
     * @param pid the process (task) PID, or 0 for current process
     * @return ProcessInfo
     */
    static ProcessInfo getProcessInfo(int pid = 0);

    // static NetworkInfo getNetworkInfo();
};


} // namespace os
} // namespace yarp

#endif // YARP_OS_SYSTEMINFO_H
