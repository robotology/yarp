// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARPOSH__
#define __YARPOSH__

#include <yarp/os/api.h>

#include <cstddef>

/*
* Copyright (C) 2009 RobotCub Consortium
* Author: Lorenzo Natale
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

namespace yarp {
    namespace os {
        enum
        {
            YARP_SIGTERM,
            YARP_SIGINT
        };

        typedef void(*YarpSignalHandler)(int);
        /**
        * Wrapper for the ACE_OS::signal signal.
        * Attach a function handler to a signal.
        * @param signum signal number (e.g. SIGTERM/SIGINT) to handle
        * @param sig handler function
        */
        YARP_OS_API YarpSignalHandler signal(int signum, YarpSignalHandler sig);

        /**
        * Wrapper for ACE_OS::exit().
        */
        YARP_OS_API void exit(int v);

        /**
        * Wrapper for ACE_OS::abort().
        */
        YARP_OS_API void abort(bool verbose=false);

        /**
        * Wrapper for ACE_OS::getenv().
        * @param v string that containt the environment variable name
        * @return the value corresponding to the envarionment variable v
        */
        YARP_OS_API const char *getenv(const char *v);

        /**
        * Wrapper for ACE_OS::getppid().
        * @return the process id (pid)
        */
        YARP_OS_API int getpid();

        /**
        * Wrapper for ACE_OS::setprogname().
        * @param progname the program name
        */
        YARP_OS_API void setprogname(const char *progname);

        /**
        * Wrapper for ACE_OS::getprogname().
        * @return the program name
        */
        YARP_OS_API void getprogname(char* progname, size_t size);

        /**
        * Wrapper for ACE_OS::hostname().
        * @return the system hostname
        */
        YARP_OS_API void gethostname(char* hostname, size_t size);

        /**
        * Wrapper for ACE_OS::mkdir(). Create a directory.
        * @param p name of the new directory.
        */
        YARP_OS_API int mkdir(const char *p);

        /**
        * Create a directory and all parent directories needed
        * @param p desired path
        * @param ignoreLevels components of name to ignore. Set to 1 if 
        * last element of path is a filename, for example.
        * @return 0 on success
        */
        YARP_OS_API int mkdir_p(const char *p, int ignoreLevels = 0);

       /**
        * Wrapper for ACE_OS::rmdir(). Remove an empty directory.
        * @param p name of the directory.
        */
        YARP_OS_API int rmdir(const char *p);

        /**
        * Wrapper for ACE_OS::stat() function.
        */
        YARP_OS_API int stat(const char *f);
    }
}

#endif
