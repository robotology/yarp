/*
 * SPDX-FileCopyrightText: 2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define HEADER_SMOKE_TEST
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>

// We verify that the version tested is indeed the requested in the CMakeList.txt,
// and not some following version due to some transitive target_compile_features requirement
static_assert(__cplusplus <= 201703L, "This test should be compiled with C++17, or anyhow with the version required in its CMakeLists.txt");

int main()
{
    return 0;
}
