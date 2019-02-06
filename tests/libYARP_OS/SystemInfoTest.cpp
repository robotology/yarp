/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/MessageStack.h>
#include <yarp/os/SystemInfo.h>
#include <yarp/os/impl/UnitTest.h>
#include <sstream>


using namespace yarp::os::impl;
using namespace yarp::os;

class SystemInfoTest : public UnitTest {
private:
    std::ostringstream str;

    void emptyAndClearString() {
        str.str(std::string());
        str.clear();
    }

public:
    virtual std::string getName() const override {
        return "SystemInfoTest";
    }

    void startCheck() {
        report(0, "Getting (system dependent) load info:");
        SystemInfo::LoadInfo ldinfo = SystemInfo::getLoadInfo();
        str << "Instant: " << ldinfo.cpuLoadInstant << ", CPU1: " << ldinfo.cpuLoad5 << ", CPU5: " << ldinfo.cpuLoad5 << ", CPU15: " << ldinfo.cpuLoad15 << ".";
        report(0, std::string(str.str()));

        emptyAndClearString();
        report(0, "Getting (system dependent) memory info:");
        SystemInfo::MemoryInfo meminfo = SystemInfo::getMemoryInfo();
        str << "Free memory space: " << meminfo.freeSpace << ", total memory space: " << meminfo.totalSpace << ".\n";
        report(0, std::string(str.str()));

        emptyAndClearString();
        report(0, "Getting (system dependent) platform info:");
        SystemInfo::PlatformInfo pltinfo = SystemInfo::getPlatformInfo();
        str << "Codename: " << pltinfo.codename << ", distribution: " << pltinfo.distribution << ", kernel: " << pltinfo.kernel << ", name: " << pltinfo.name << ", release: " << pltinfo.release << ", environment variables: "<< pltinfo.environmentVars.toString() << ".\n";
        report(0, std::string(str.str()));

        emptyAndClearString();
        report(0, "Getting (system dependent) process info for process 1:");
        SystemInfo::ProcessInfo prctinfo = SystemInfo::getProcessInfo(1);
        str << "Name: " << prctinfo.name << ", arguments: " << prctinfo.arguments << ", PID: " << prctinfo.pid << ", scheduler policy: " << prctinfo.schedPolicy << ", scheduler priority: " << prctinfo.schedPriority << ".\n";
        report(0, std::string(str.str()));

        emptyAndClearString();
        report(0, "Getting (system dependent) process info for current process:");
        prctinfo = SystemInfo::getProcessInfo();
        str << "Name: " << prctinfo.name << ", arguments: " << prctinfo.arguments << ", PID: " << prctinfo.pid << ", scheduler policy: " << prctinfo.schedPolicy << ", scheduler priority: " << prctinfo.schedPriority << ".\n";
        report(0, std::string(str.str()));

        emptyAndClearString();
        report(0, "Getting (system dependent) processor info:");
        SystemInfo::ProcessorInfo prcsinfo = SystemInfo::getProcessorInfo();
        str << "Arch: " << prcsinfo.architecture << ", cores: " << prcsinfo.cores << ", family: " << prcsinfo.family << ", frequency: " << prcsinfo.frequency << ", model: " << prcsinfo.model << ", model number: " << prcsinfo.modelNumber << ", siblings: " << prcsinfo.siblings << ", vendor: " << prcsinfo.vendor << ".\n";
        report(0, std::string(str.str()));

        emptyAndClearString();
        report(0, "Getting (system dependent) storage info:");
        SystemInfo::StorageInfo strginfo = SystemInfo::getStorageInfo();
        str << "Free storage space: " << strginfo.freeSpace << ", total storage space: " << strginfo.totalSpace << ".\n";
        report(0, std::string(str.str()));

        emptyAndClearString();
        report(0, "Getting (system dependent) user info:");
        SystemInfo::UserInfo usrinfo = SystemInfo::getUserInfo();
        str << "User ID: " << usrinfo.userID << ", username: " << usrinfo.userName << ", real name: " << usrinfo.realName << ", home directory: " << usrinfo.homeDir << ".\n";
        report(0, std::string(str.str()));
    }

    virtual void runTests() override {
        startCheck();
    }
};

static SystemInfoTest theTest;

UnitTest& getSystemInfoTest() {
    return theTest;
}

