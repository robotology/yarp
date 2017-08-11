/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

int yarp::os::fork(void)
{
    pid_t pid = yarp::os::impl::fork();
    return pid;
}
