/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_LOG_H
#define YARP_OS_LOG_H

#include <yarp/os/api.h>

#include <iosfwd>

#if defined(__GNUC__)
#    define __YFUNCTION__ __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#    define __YFUNCTION__ __FUNCSIG__
#elif (__cplusplus <= 199711)
#    define __YFUNCTION__ __func__
#else
#    define __YFUNCTION__ "(unknown function)"
#endif // __GNUC__

// check arguments of the c-style debug functions to make sure that the
// arguments supplied have types appropriate to the format string
// specified, and that the conversions specified in the format string
// make sense. On gcc the warning is enabled by -Wformat.
#if defined(__GNUC__)
#    define YARP_ATTRIBUTE_FORMAT(style, fmt, args) __attribute__((format(printf, (fmt), (args))))
#else
#    define YARP_ATTRIBUTE_FORMAT(style, fmt, args)
#endif


// Forward declarations
namespace yarp {
namespace os {

class LogStream;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace impl {
class LogImpl;
} // namespace impl
#endif // DOXYGEN_SHOULD_SKIP_THIS

class YARP_os_API Log
{
public:
    Log(const char* file,
        const unsigned int line,
        const char* func);
    Log();
    virtual ~Log();

    enum LogType
    {
        TraceType,
        DebugType,
        InfoType,
        WarningType,
        ErrorType,
        FatalType
    };

    void trace(const char* msg, ...) const YARP_ATTRIBUTE_FORMAT(printf, 2, 3);
    void debug(const char* msg, ...) const YARP_ATTRIBUTE_FORMAT(printf, 2, 3);
    void info(const char* msg, ...) const YARP_ATTRIBUTE_FORMAT(printf, 2, 3);
    void warning(const char* msg, ...) const YARP_ATTRIBUTE_FORMAT(printf, 2, 3);
    void error(const char* msg, ...) const YARP_ATTRIBUTE_FORMAT(printf, 2, 3);
    YARP_NORETURN void fatal(const char* msg, ...) const YARP_ATTRIBUTE_FORMAT(printf, 2, 3);

    LogStream trace() const;
    LogStream debug() const;
    LogStream info() const;
    LogStream warning() const;
    LogStream error() const;
    LogStream fatal() const;

    typedef void (*LogCallback)(yarp::os::Log::LogType,
                                const char*,
                                const char*,
                                const unsigned int,
                                const char*);

    static void setLogCallback(LogCallback);

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    yarp::os::impl::LogImpl* const mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS

    static LogCallback print_callback;
    static LogCallback forward_callback;

    friend class LogStream;
}; // class Log

} // namespace os
} // namespace yarp


#define yTrace   yarp::os::Log(__FILE__, __LINE__, __YFUNCTION__).trace
#define yDebug   yarp::os::Log(__FILE__, __LINE__, __YFUNCTION__).debug
#define yInfo    yarp::os::Log(__FILE__, __LINE__, __YFUNCTION__).info
#define yWarning yarp::os::Log(__FILE__, __LINE__, __YFUNCTION__).warning
#define yError   yarp::os::Log(__FILE__, __LINE__, __YFUNCTION__).error
#define yFatal   yarp::os::Log(__FILE__, __LINE__, __YFUNCTION__).fatal

#ifndef NDEBUG
#  define yAssert(x)                                                       \
    if (!(x)) {                                                            \
        yFatal("Assertion failure at %s:%d (%s)", __FILE__, __LINE__, #x); \
    }
#else
#  define yAssert(x)
#endif

#define YARP_FIXME_NOTIMPLEMENTED(what) yWarning("FIXME: %s not yet implemented", what);


/**
 * Low level function for printing a stack trace, if implemented (ACE or gcc/Linux).
 */
YARP_os_API void yarp_print_trace(FILE* out, const char* file, int line);


#endif // YARP_OS_LOG_H
