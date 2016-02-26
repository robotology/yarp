/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <cstdio>

#include <yarp/os/Os.h>

#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/impl/NameConfig.h>
#ifdef YARP_HAS_ACE
#  include <ace/OS_NS_stdio.h>
#  include <ace/OS_NS_unistd.h>
#  include <ace/OS_NS_signal.h>
#  include <ace/OS_NS_stdlib.h>
#  include <ace/OS_NS_sys_stat.h>
#  include <ace/ACE.h>
#endif

yarp::os::YarpSignalHandler yarp::os::signal(int signum, yarp::os::YarpSignalHandler sighandler)
{
    switch (signum)
    {
        case yarp::os::YARP_SIGINT:
            return ACE_OS::signal(SIGINT, (ACE_SignalHandler) sighandler);
        case yarp::os::YARP_SIGTERM:
            return ACE_OS::signal(SIGTERM, (ACE_SignalHandler) sighandler);
        default:
            return 0; //signal not implemented yet
    }
}

void yarp::os::exit(int v)
{
    ACE_OS::exit(v); //may cause crash... exit is not recommended in processes with multi thread, see http://www.cplusplus.com/reference/cstdlib/exit/
    //::exit(v);     //...this seems to work(?)
}

void yarp::os::abort(bool verbose)
{
#if defined(WIN32)
    if (verbose==false)
    {
        //to suppress windows dialog message
        _set_abort_behavior(0, _WRITE_ABORT_MSG);
        _set_abort_behavior(0, _CALL_REPORTFAULT);
    }
#endif
    ACE_OS::abort();   // exit is not recommended in processes with multi thread, see http://www.cplusplus.com/reference/cstdlib/exit/ and http://www.cplusplus.com/reference/cstdlib/abort/
}

const char *yarp::os::getenv(const char *var)
{
    return ACE_OS::getenv(var);
}

int yarp::os::mkdir(const char *p)
{
#ifdef YARP_HAS_ACE
    return ACE_OS::mkdir(p);
#else
    return ::mkdir(p,0777);
#endif
}

int yarp::os::mkdir_p(const char *p, int ignoreLevels) {
    bool ok = yarp::os::impl::NameConfig::createPath(p,ignoreLevels);
    return ok?0:1;
}

int yarp::os::rmdir(const char *p)
{
#ifdef YARP_HAS_ACE
    return ACE_OS::rmdir(p);
#else
    return ::rmdir(p);
#endif
}

int yarp::os::rename(const char *oldname, const char *newname)
{
#ifdef YARP_HAS_ACE
    return ACE_OS::rename(oldname,newname);
#else
    return std::rename(oldname,newname);
#endif
}

int yarp::os::stat(const char *path)
{
    ACE_stat dummy;
    return ACE_OS::stat(path, &dummy);
}

int yarp::os::getpid()
{
    pid_t pid = ACE_OS::getpid();
    return pid;
}

void yarp::os::setprogname(const char *progname)
{
#ifdef YARP_HAS_ACE
    ACE_OS::setprogname(ACE::basename(progname));
#else
    // not available
#endif
}


void yarp::os::getprogname(char*progname, size_t size)
{
#ifdef YARP_HAS_ACE
    const char* tmp = ACE_OS::getprogname ();
    if (strlen(tmp)==0)
    {
        strncpy(progname,"no_progname",size);
    }
    else
    {
        strncpy(progname,tmp,size);
    }
#else
    // not available
    *progname = '\0';
#endif
}


void yarp::os::gethostname(char* hostname, size_t size)
{
#ifdef YARP_HAS_ACE
    ACE_OS::hostname(hostname, size);
#else
    ::gethostname(hostname, size);
#endif
    if (strlen(hostname)==0)
    {
        strncpy(hostname,"no_hostname",size);
    }
}
