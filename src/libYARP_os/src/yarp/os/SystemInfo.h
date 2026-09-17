/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_SYSTEMINFO_H
#define YARP_OS_SYSTEMINFO_H

#include <yarp/os/SystemInfoData.h>

#include <string>


namespace yarp::os {

/**
 * \ingroup key_class
 *
 * \brief A class to get the system (platform) status  such as
 * available memory, storage, CPU load and etc.
 */

class YARP_os_API SystemInfo : public yarp::os::SystemInfoData
{
public:
    typedef int capacity_t;
    typedef yarp::os::ProcessInfoData    ProcessInfo;
    typedef yarp::os::MemoryInfoData     MemoryInfo;
    typedef yarp::os::StorageInfoData    StorageInfo;
    typedef yarp::os::ProcessorInfoData  ProcessorInfo;
    typedef yarp::os::LoadInfoData       LoadInfo;
    typedef yarp::os::PlatformInfoData   PlatformInfo;
    typedef yarp::os::UserInfoData       UserInfo;

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

    /**
     * @brief updates the system information by calling the static methods to
     * retrieve the latest data.
     */
    void updateSystemInfo();

    /**
     * @brief reads from a ConnectionReader and fill into the SystemInfo structs.
     * @param connection a ConnectionReader
     * @return true/false upon success or failure
     */
    bool read(yarp::os::ConnectionReader& connection) override;

    /**
     * @brief write the SystemInfo structs using a ConnectionWriter.
     * @param connection a ConnectionWriter
     * @return true/false upon success or failure
     */
    bool write(yarp::os::ConnectionWriter& connection) const override;

};


} // namespace yarp::os

#endif // YARP_OS_SYSTEMINFO_H
