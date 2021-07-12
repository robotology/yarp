/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SystemInfo.h>

#include <sstream>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("os::SystemInfoTest", "[yarp::os]")
{
    std::ostringstream str;

    SECTION("Getting (system dependent) load info:")
    {
        SystemInfo::LoadInfo ldinfo = SystemInfo::getLoadInfo();
        str << "Instant: " << ldinfo.cpuLoadInstant
            << ", CPU1: " << ldinfo.cpuLoad1
            << ", CPU5: " << ldinfo.cpuLoad5
            << ", CPU15: " << ldinfo.cpuLoad15
            << ".\n";
        INFO(str.str());
    }

    SECTION("Getting (system dependent) memory info:")
    {
        SystemInfo::MemoryInfo meminfo = SystemInfo::getMemoryInfo();
        str << "Free memory space: " << meminfo.freeSpace
            << ", total memory space: " << meminfo.totalSpace
            << ".\n";
        INFO(str.str());
    }

    SECTION("Getting (system dependent) platform info:")
    {
        SystemInfo::PlatformInfo pltinfo = SystemInfo::getPlatformInfo();
        str << "Codename: " << pltinfo.codename
            << ", distribution: " << pltinfo.distribution
            << ", kernel: " << pltinfo.kernel
            << ", name: " << pltinfo.name
            << ", release: " << pltinfo.release
            << ", environment variables: "<< pltinfo.environmentVars.toString()
            << ".\n";
        INFO(str.str());
    }

    SECTION("Getting (system dependent) process info for process 1:")
    {
        SystemInfo::ProcessInfo prctinfo = SystemInfo::getProcessInfo(1);
        str << "Name: " << prctinfo.name
            << ", arguments: " << prctinfo.arguments
            << ", PID: " << prctinfo.pid
            << ", scheduler policy: " << prctinfo.schedPolicy
            << ", scheduler priority: " << prctinfo.schedPriority
            << ".\n";
        INFO(str.str());
    }

    SECTION("Getting (system dependent) process info for current process:")
    {
        SystemInfo::ProcessInfo prctinfo = SystemInfo::getProcessInfo();
        str << "Name: " << prctinfo.name
            << ", arguments: " << prctinfo.arguments
            << ", PID: " << prctinfo.pid
            << ", scheduler policy: " << prctinfo.schedPolicy
            << ", scheduler priority: " << prctinfo.schedPriority
            << ".\n";
        INFO(str.str());
    }

    SECTION("Getting (system dependent) processor info:")
    {
        SystemInfo::ProcessorInfo prcsinfo = SystemInfo::getProcessorInfo();
        str << "Arch: " << prcsinfo.architecture
            << ", cores: " << prcsinfo.cores
            << ", family: " << prcsinfo.family
            << ", frequency: " << prcsinfo.frequency
            << ", model: " << prcsinfo.model
            << ", model number: " << prcsinfo.modelNumber
            << ", siblings: " << prcsinfo.siblings
            << ", vendor: " << prcsinfo.vendor
            << ".\n";
        INFO(str.str());
    }

    SECTION("Getting (system dependent) storage info:")
    {
        SystemInfo::StorageInfo strginfo = SystemInfo::getStorageInfo();
        str << "Free storage space: " << strginfo.freeSpace
            << ", total storage space: " << strginfo.totalSpace
            << ".\n";
        INFO(str.str());
    }

    SECTION("Getting (system dependent) user info:")
    {
        SystemInfo::UserInfo usrinfo = SystemInfo::getUserInfo();
        str << "User ID: " << usrinfo.userID
            << ", username: " << usrinfo.userName
            << ", real name: " << usrinfo.realName
            << ", home directory: " << usrinfo.homeDir
            << ".\n";
        INFO(str.str());
    }
}
