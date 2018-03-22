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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef YARP_HAS_ACE
# include <ace/ACE.h>
#endif

#if defined(__APPLE__)
#include <yarp/os/impl/MacOSAPI.h>
#endif

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
    return ok ? 0 : 1;
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

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
void yarp::os::setprogname(const char *progname)
{
#ifdef YARP_HAS_ACE
    ACE_OS::setprogname(ACE::basename(progname));
#else
    // not available
    YARP_UNUSED(progname);
#endif
}


void yarp::os::getprogname(char* progname, size_t size)
{
#ifdef YARP_HAS_ACE
    const char* tmp = ACE_OS::getprogname();
    if (std::strlen(tmp)==0) {
        std::strncpy(progname, "no_progname", size);
    } else {
        std::strncpy(progname, tmp, size);
    }
#else
    // not available
    *progname = '\0';
    YARP_UNUSED(size);
#endif
}
#endif // YARP_NO_DEPRECATED


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

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
int yarp::os::fork()
{
#if defined(YARP_HAS_ACE)
    pid_t pid = ACE_OS::fork();
#elif defined(__unix__)
    pid_t pid = ::fork();
#else
YARP_COMPILER_ERROR(Cannot implement fork on this platform)
#endif
    return pid;
}
#endif // YARP_NO_DEPRECATED
