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
    memory.totalSpace = connection.expectInt();
    memory.freeSpace = connection.expectInt();

    // reading storage
    storage.totalSpace = connection.expectInt();
    storage.freeSpace = connection.expectInt();

    // reading network
    //network.mac = connection.expectText();
    //network.ip4 = connection.expectText();
    //network.ip6 = connection.expectText();

    // reading processor
    processor.architecture = connection.expectText();
    processor.model = connection.expectText();
    processor.vendor = connection.expectText();
    processor.family = connection.expectInt();
    processor.modelNumber = connection.expectInt();
    processor.cores = connection.expectInt();
    processor.siblings = connection.expectInt();
    processor.frequency = connection.expectDouble();

    // reading load
    load.cpuLoad1 = connection.expectDouble();
    load.cpuLoad5 = connection.expectDouble();
    load.cpuLoad15 = connection.expectDouble();
    load.cpuLoadInstant = connection.expectInt();

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
    user.userID = connection.expectInt();
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
    connection.appendInt(memory.totalSpace);
    connection.appendInt(memory.freeSpace);

    // serializing storage
    connection.appendInt(storage.totalSpace);
    connection.appendInt(storage.freeSpace);

    // serializing network
    //connection.appendString(network.mac.c_str());
    //connection.appendString(network.ip4.c_str());
    //connection.appendString(network.ip6.c_str());

    // serializing processor
    connection.appendString(processor.architecture.c_str());
    connection.appendString(processor.model.c_str());
    connection.appendString(processor.vendor.c_str());
    connection.appendInt(processor.family);
    connection.appendInt(processor.modelNumber);
    connection.appendInt(processor.cores);
    connection.appendInt(processor.siblings);
    connection.appendDouble(processor.frequency);

    // serializing load
    connection.appendDouble(load.cpuLoad1);
    connection.appendDouble(load.cpuLoad5);
    connection.appendDouble(load.cpuLoad15);
    connection.appendInt(load.cpuLoadInstant);

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
    connection.appendInt(user.userID);

    return !connection.isError();
}
