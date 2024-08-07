/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Os.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/PlatformLimits.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/impl/PlatformSysStat.h>
#include <yarp/os/impl/PlatformUnistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef YARP_HAS_ACE
#    include <ace/ACE.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#endif

#if defined(__APPLE__)
#    include <yarp/os/impl/macos/MacOSAPI.h>
#endif

int yarp::os::mkdir(const char* p)
{
    return yarp::os::impl::mkdir(p, 0755);
}

int yarp::os::mkdir_p(const char* p, int ignoreLevels)
{
    std::string fileName(p);

    size_t index = fileName.rfind('/');
    if (index == std::string::npos) {
#if defined(_WIN32)
        index = fileName.rfind('\\');
        if (index == std::string::npos) {
            return 1;
        }
#else
        return 1;
#endif
    }
    std::string base = fileName.substr(0, index);
    if (yarp::os::stat(const_cast<char*>(base.c_str())) < 0) {
        int result = yarp::os::mkdir_p(base.c_str(), ignoreLevels - 1);
        if (result != 0) {
            return 1;
        }
    }
    if (ignoreLevels <= 0) {
        if (yarp::os::stat(fileName.c_str()) < 0) {
            if (yarp::os::mkdir(fileName.c_str()) >= 0) {
                return 0;
            }
            return 1;
        }
    }
    return 0;
}

int yarp::os::rmdir(const char* p)
{
    return yarp::os::impl::rmdir(p);
}

int yarp::os::rename(const char* oldname, const char* newname)
{
    return std::rename(oldname, newname);
}

int yarp::os::stat(const char* path)
{
    yarp::os::impl::YARP_stat dummy;
    return yarp::os::impl::stat(path, &dummy);
}

int yarp::os::getpid()
{
    pid_t pid = yarp::os::impl::getpid();
    return pid;
}

void yarp::os::gethostname(char* hostname, size_t size)
{
    yarp::os::impl::gethostname(hostname, size);
    if (std::strlen(hostname) == 0) {
        std::strncpy(hostname, "no_hostname", size);
    }
}

std::string yarp::os::gethostname()
{
    char hostname[HOST_NAME_MAX];
    yarp::os::gethostname(hostname, HOST_NAME_MAX);
    return {hostname};
}

char* yarp::os::getcwd(char* buf, size_t size)
{
    return yarp::os::impl::getcwd(buf, size);
}

void yarp::os::setEnergySavingModeState(bool enabled)
{
#if defined(__APPLE__)
    static void* handle = 0;
    if (!enabled && !handle) {
        handle = disableAppNap();
    } else {
        restoreAppNap(handle);
    }

#endif
}
