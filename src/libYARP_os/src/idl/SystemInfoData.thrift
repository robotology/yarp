/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.os

/**
 * @brief Holds the system memory information
 */
struct MemoryInfoData
{
  1: i32 totalSpace;
  2: i32 freeSpace;
} (
  yarp.api.include = "yarp/os/api.h"
  yarp.api.keyword = "YARP_os_API"
)

/**
 * @brief Holds the system storage information
 */
struct StorageInfoData
{
  1: i32 totalSpace;
  2: i32 freeSpace;
} (
  yarp.api.include = "yarp/os/api.h"
  yarp.api.keyword = "YARP_os_API"
)

/**
 * @brief Holds the processor information
 */
struct ProcessorInfoData
{
  1: string architecture;
  2: string model;
  3: string vendor;
  4: i32 family;
  5: i32 modelNumber;
  6: i32 cores;
  7: i32 siblings;
  8: double frequency;
} (
  yarp.api.include = "yarp/os/api.h"
  yarp.api.keyword = "YARP_os_API"
)

/**
 * @brief Holds the current cpu load information
 */
struct LoadInfoData
{
  1: double cpuLoad1;
  2: double cpuLoad5;
  3: double cpuLoad15;
  4: i32 cpuLoadInstant;
} (
  yarp.api.include = "yarp/os/api.h"
  yarp.api.keyword = "YARP_os_API"
)

/**
 * @brief Holds the operating system information
 */
struct PlatformInfoData
{
  1: string name;
  2: string distribution;
  3: string release;
  4: string codename;
  5: string kernel;
  6: string environmentVars;
} (
  yarp.api.include = "yarp/os/api.h"
  yarp.api.keyword = "YARP_os_API"
)

/**
 * @brief Holds the current user information
 */
struct UserInfoData
{
  1: string userName;
  2: string realName;
  3: string homeDir;
  4: i32 userID;
} (
  yarp.api.include = "yarp/os/api.h"
  yarp.api.keyword = "YARP_os_API"
)

/**
 * @brief Holds the operating system process information
 */
struct ProcessInfoData
{
  1: string name;
  2: string arguments;
  3: i32 schedPolicy;
  4: i32 schedPriority;
  5: i32 pid;
} (
  yarp.api.include = "yarp/os/api.h"
  yarp.api.keyword = "YARP_os_API"
)

/**
 * @brief Aggregates all the system (platform) status information,
 * such as available memory, storage, CPU load, etc.
 */
struct SystemInfoData
{
  1: MemoryInfoData memory;
  2: StorageInfoData storage;
  3: ProcessorInfoData processor;
  4: LoadInfoData load;
  5: PlatformInfoData platform;
  6: UserInfoData user;
  7: ProcessInfoData process;
} (
  yarp.api.include = "yarp/os/api.h"
  yarp.api.keyword = "YARP_os_API"
)
