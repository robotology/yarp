// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Os.h>

#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/PlatformSignal.h>
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
    ACE_OS::exit(v);
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

int yarp::os::rmdir(const char *p)
{
#ifdef YARP_HAS_ACE
    return ACE_OS::rmdir(p);
#else
    return ::rmdir(p);
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
    ACE_OS::setprogname(::basename(progname));
#endif
}

void yarp::os::getprogname(char*progname)
{
    const char* tmp = ACE_OS::getprogname ();
    if (strlen(tmp)==0)
    {
        strcpy(progname,"no_progname");
    }
    else
    {
        strcpy(progname,tmp);
    }
}
void yarp::os::gethostname(char* hostname)
{
    char buff[50];
    ACE_OS::hostname(buff, sizeof(buff));
    if (strlen(buff)==0)
    {
        strcpy(hostname,"no_hostname");
    }
    else if (strlen(buff)>=50)
    {
        strcpy(hostname,"too_long_hostname");
    }
    else
    {
        strcpy(hostname,buff);
    }
}