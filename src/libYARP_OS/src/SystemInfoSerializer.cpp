/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/SystemInfo.h>
#include <yarp/os/SystemInfoSerializer.h>
#include <yarp/os/ConnectionReader.h>

using namespace yarp::os;


SystemInfoSerializer::SystemInfoSerializer() :
    memory(SystemInfo::MemoryInfo{0,0}),
    storage(SystemInfo::StorageInfo{0,0}),
    load(SystemInfo::LoadInfo{.0,.0,.0,0})
{
}

SystemInfoSerializer::~SystemInfoSerializer()
{
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


bool SystemInfoSerializer::write(yarp::os::ConnectionWriter& connection)
{
    // updating system info
    memory = SystemInfo::getMemoryInfo();
    storage = SystemInfo::getStorageInfo();
    //network = SystemInfo::getNetworkInfo();
    processor = SystemInfo::getProcessorInfo();
    platform = SystemInfo::getPlatformInfo();
    load = SystemInfo::getLoadInfo();
    user = SystemInfo::getUserInfo();

    // serializing memory
    connection.appendInt32(memory.totalSpace);
    connection.appendInt32(memory.freeSpace);

    // serializing storage
    connection.appendInt32(storage.totalSpace);
    connection.appendInt32(storage.freeSpace);

    // serializing network
    //connection.appendString(network.mac.c_str());
    //connection.appendString(network.ip4.c_str());
    //connection.appendString(network.ip6.c_str());

    // serializing processor
    connection.appendString(processor.architecture.c_str());
    connection.appendString(processor.model.c_str());
    connection.appendString(processor.vendor.c_str());
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
    connection.appendString(platform.name.c_str());
    connection.appendString(platform.distribution.c_str());
    connection.appendString(platform.release.c_str());
    connection.appendString(platform.codename.c_str());
    connection.appendString(platform.kernel.c_str());
    connection.appendString(platform.environmentVars.toString().c_str());

    // serializing user
    connection.appendString(user.userName.c_str());
    connection.appendString(user.realName.c_str());
    connection.appendString(user.homeDir.c_str());
    connection.appendInt32(user.userID);

    return !connection.isError();
}
