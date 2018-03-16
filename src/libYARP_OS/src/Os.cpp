/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Os.h>

#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/PlatformUnistd.h>
#include <yarp/os/impl/PlatformSysStat.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/SystemInfo.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef YARP_HAS_ACE
# include <ace/ACE.h>
#endif


#ifndef YARP_NO_DEPRECATED // Since YARP 2.3.70
yarp::os::YarpSignalHandler yarp::os::signal(int signum, yarp::os::YarpSignalHandler sighandler)
{
    switch (signum) {
        case yarp::os::YARP_SIGINT:
            return yarp::os::impl::signal(SIGINT, sighandler);
        case yarp::os::YARP_SIGTERM:
            return yarp::os::impl::signal(SIGTERM, sighandler);
        default:
            return nullptr; //signal not implemented yet
    }
}
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // Since YARP 2.3.70
void yarp::os::exit(int exit_code)
{
    std::exit(exit_code); //may cause crash... exit is not recommended in processes with multi thread, see http://www.cplusplus.com/reference/cstdlib/exit/
}
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // Since YARP 2.3.70
void yarp::os::abort(bool verbose)
{
#if defined(_WIN32)
    if (verbose==false) {
        //to suppress windows dialog message
        _set_abort_behavior(0, _WRITE_ABORT_MSG);
        _set_abort_behavior(0, _CALL_REPORTFAULT);
    }
#else
    YARP_UNUSED(verbose);
#endif
    std::abort();   // exit is not recommended in processes with multi thread, see http://www.cplusplus.com/reference/cstdlib/exit/ and http://www.cplusplus.com/reference/cstdlib/abort/
}
#endif // YARP_NO_DEPRECATED

const char *yarp::os::getenv(const char *var)
{
    return yarp::os::impl::getenv(var);
}

int yarp::os::mkdir(const char *p)
{
    return yarp::os::impl::mkdir(p, 0755);
}

int yarp::os::mkdir_p(const char *p, int ignoreLevels) {
    bool ok = yarp::os::impl::NameConfig::createPath(p, ignoreLevels);
    return ok?0:1;
}

int yarp::os::rmdir(const char *p)
{
    return yarp::os::impl::rmdir(p);
}

int yarp::os::rename(const char *oldname, const char *newname)
{
    return std::rename(oldname, newname);
}

int yarp::os::stat(const char *path)
{
    yarp::os::impl::YARP_stat dummy;
    return yarp::os::impl::stat(path, &dummy);
}

int yarp::os::getpid()
{
    pid_t pid = yarp::os::impl::getpid();
    return pid;
}


void yarp::os::getprogname(char* progname, size_t size)
{
    int pid = yarp::os::getpid();
    yarp::os::SystemInfo::ProcessInfo info = yarp::os::SystemInfo::getProcessInfo(pid);
    std::strncpy(progname, info.name.c_str(), size);
}


void yarp::os::gethostname(char* hostname, size_t size)
{
    yarp::os::impl::gethostname(hostname, size);
    if (std::strlen(hostname)==0) {
        std::strncpy(hostname, "no_hostname", size);
    }
}

char* yarp::os::getcwd(char *buf, size_t size)
{
    return yarp::os::impl::getcwd(buf, size);
}

int yarp::os::fork()
{
    pid_t pid = yarp::os::impl::fork();
    return pid;
}


#if defined(__APPLE__)
#include "yarp/os/impl/MacOSAPI.h"
#endif


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
