/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SystemInfoSerializer.h>

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/SystemInfo.h>

using namespace yarp::os;


class SystemInfoSerializer::Private
{
public:
    Private(SystemInfoSerializer* parent) :
            parent(parent)
    {
    }

    void updateSystemInfo()
    {
        // updating system info
        parent->memory = SystemInfo::getMemoryInfo();
        parent->storage = SystemInfo::getStorageInfo();
        //parent->network = SystemInfo::getNetworkInfo();
        parent->processor = SystemInfo::getProcessorInfo();
        parent->platform = SystemInfo::getPlatformInfo();
        parent->load = SystemInfo::getLoadInfo();
        parent->user = SystemInfo::getUserInfo();
    }

    SystemInfoSerializer* const parent;
};

SystemInfoSerializer::SystemInfoSerializer() :
        memory(SystemInfo::MemoryInfo{0, 0}),
        storage(SystemInfo::StorageInfo{0, 0}),
        load(SystemInfo::LoadInfo{.0, .0, .0, 0}),
        mPriv(new Private(this))
{
}

SystemInfoSerializer::~SystemInfoSerializer()
{
    delete mPriv;
}

bool SystemInfoSerializer::read(yarp::os::ConnectionReader& connection)
{
    // reading memory
    memory.totalSpace = connection.expectInt32();
    memory.freeSpace = connection.expectInt32();

    // reading storage
    storage.totalSpace = connection.expectInt32();
    storage.freeSpace = connection.expectInt32();

    // reading network
    //network.mac = connection.expectText();
    //network.ip4 = connection.expectText();
    //network.ip6 = connection.expectText();

    // reading processor
    processor.architecture = connection.expectText();
    processor.model = connection.expectText();
    processor.vendor = connection.expectText();
    processor.family = connection.expectInt32();
    processor.modelNumber = connection.expectInt32();
    processor.cores = connection.expectInt32();
    processor.siblings = connection.expectInt32();
    processor.frequency = connection.expectFloat64();

    // reading load
    load.cpuLoad1 = connection.expectFloat64();
    load.cpuLoad5 = connection.expectFloat64();
    load.cpuLoad15 = connection.expectFloat64();
    load.cpuLoadInstant = connection.expectInt32();

    // reading platform
    platform.name = connection.expectText();
    platform.distribution = connection.expectText();
    platform.release = connection.expectText();
    platform.codename = connection.expectText();
    platform.kernel = connection.expectText();
    platform.environmentVars.fromString(connection.expectText());

    // reading user
    user.userName = connection.expectText();
    user.realName = connection.expectText();
    user.homeDir = connection.expectText();
    user.userID = connection.expectInt32();
    return true;
}


bool SystemInfoSerializer::write(yarp::os::ConnectionWriter& connection) const
{
    mPriv->updateSystemInfo();

    // serializing memory
    connection.appendInt32(memory.totalSpace);
    connection.appendInt32(memory.freeSpace);

    // serializing storage
    connection.appendInt32(storage.totalSpace);
    connection.appendInt32(storage.freeSpace);

    // serializing network
    //connection.appendText(network.mac);
    //connection.appendText(network.ip4);
    //connection.appendText(network.ip6);

    // serializing processor
    connection.appendText(processor.architecture);
    connection.appendText(processor.model);
    connection.appendText(processor.vendor);
    connection.appendInt32(processor.family);
    connection.appendInt32(processor.modelNumber);
    connection.appendInt32(processor.cores);
    connection.appendInt32(processor.siblings);
    connection.appendFloat64(processor.frequency);

    // serializing load
    connection.appendFloat64(load.cpuLoad1);
    connection.appendFloat64(load.cpuLoad5);
    connection.appendFloat64(load.cpuLoad15);
    connection.appendInt32(load.cpuLoadInstant);

    // serializing platform
    connection.appendText(platform.name);
    connection.appendText(platform.distribution);
    connection.appendText(platform.release);
    connection.appendText(platform.codename);
    connection.appendText(platform.kernel);
    connection.appendText(platform.environmentVars.toString());

    // serializing user
    connection.appendText(user.userName);
    connection.appendText(user.realName);
    connection.appendText(user.homeDir);
    connection.appendInt32(user.userID);

    return !connection.isError();
}
