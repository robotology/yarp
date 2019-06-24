/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_LOGIMPL_H
#define YARP_OS_IMPL_LOGIMPL_H

#include <yarp/os/Log.h>

class yarp::os::impl::LogImpl
{
public:
    LogImpl(const char* file,
            const unsigned int line,
            const char* func);

    static void print_callback(yarp::os::Log::LogType t,
                               const char* msg,
                               const char* file,
                               const unsigned int line,
                               const char* func);

    static void forward_callback(yarp::os::Log::LogType t,
                                 const char* msg,
                                 const char* file,
                                 const unsigned int line,
                                 const char* func);

    const char* file;
    const unsigned int line;
    const char* func;

    static std::ofstream ftrc; /// Used by yTrace()
    static std::ofstream fout; /// Used by yDebug() and yInfo()
    static std::ofstream ferr; /// Used by yWarning(), yError() and yFatal()

    static bool colored_output;
    static bool verbose_output;
    static bool trace_output;
    static bool debug_output;
    static bool forward_output;
};


#endif // YARP_OS_IMPL_LOGIMPL_H
