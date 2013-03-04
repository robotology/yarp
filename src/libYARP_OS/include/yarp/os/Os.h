// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARPOSH__
#define __YARPOSH__

#include <yarp/os/api.h>

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
            YARP_SIGINT,
        };

        /**
        * Wrapper for the ACE_OS::signal signal.
        * Attach a function handler to a signal.
        * @param signum: signal number (e.g. SIGTERM/SIGINT) to handle
        * @param sighandler: handler function
        */
        typedef void(*YarpSignalHandler)(int);
        YARP_OS_API YarpSignalHandler signal(int signum, YarpSignalHandler sig);

        /**
        * Wrapper for ACE_OS::exit().
        */
        YARP_OS_API void exit(int v);

        /**
        * Wrapper for ACE_OS::getenv().
        * @param v: string that containt the environment variable name
        * @return the value corresponding to the envarionment variable v
        */
        YARP_OS_API const char *getenv(const char *v);

        /**
        * Wrapper for ACE_OS::mkdir(). Create a directory.
        * @param p: name of the new directory.
        */
        YARP_OS_API int mkdir(const char *p);

       /**
        * Wrapper for ACE_OS::rmdir(). Remove an empty directory.
        * @param p: name of the directory.
        */
        YARP_OS_API int rmdir(const char *p);

        /**
        * Wrapper for ACE_OS::stat() function.
        */
        YARP_OS_API int stat(const char *f);
    }
}

#endif
