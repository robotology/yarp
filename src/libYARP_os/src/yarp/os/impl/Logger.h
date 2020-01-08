/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_LOGGER_H
#define YARP_OS_IMPL_LOGGER_H

#include <yarp/conf/api.h>
#include <yarp/conf/system.h>

#include <yarp/os/Log.h>
#include <yarp/os/impl/PlatformStdio.h>

#include <string>

#define YARP_LM_DEBUG      04
#define YARP_LM_INFO      010
#define YARP_LM_WARNING   040
#define YARP_LM_ERROR    0200


namespace yarp {
namespace os {
namespace impl {

/**
 * This is a wrapper for message logging.
 */
class YARP_os_impl_API Logger : public yarp::os::Log
{
public:
    enum Level
    {
        MAJOR_DEBUG = YARP_LM_INFO,
        DEFAULT_WARN = YARP_LM_INFO
    };

    Logger(const char* prefix, Logger* parent = nullptr);
    Logger(const char* prefix, Logger& parent);

    static Logger& get();

    void println(const std::string& txt);
    void internal_debug(const std::string& txt);
    void internal_info(const std::string& txt);
    void internal_warning(const std::string& txt);
    void internal_error(const std::string& txt);
    YARP_NORETURN void internal_fail(const std::string& txt);
    void internal_debug(const char* txt);
    void internal_info(const char* txt);
    void internal_warning(const char* txt);
    void internal_error(const char* txt);
    YARP_NORETURN void internal_fail(const char* txt);
    void assertion(bool cond);
    void setVerbosity(int verbose = 0);
    void setPrefix(const char* prefix);
    void setPid();
    long int getPid();
    bool shouldShowInfo();
    bool shouldShowError();
    bool shouldShowDebug();

private:
    void show(std::uint32_t level, const std::string& txt);

    std::string prefix;
    Logger* parent;
    int verbose;
    std::uint32_t low;
    long int pid;
    FILE* stream;
};

} // namespace impl
} // namespace os
} // namespace yarp


// compromise - use macros so that debugging can evaporate in optimized code.
// also, make a printf-style adaptor since c++ is a bit of a pain to
// build strings in.
#define YARP_ERROR(log, x) ((yarp::os::impl::Logger*)&(log))->internal_error(x)
#define YARP_WARN(log, x) ((yarp::os::impl::Logger*)&(log))->internal_warning(x)
#define YARP_INFO(log, x) ((yarp::os::impl::Logger*)&(log))->internal_info(x)
#ifndef NDEBUG
#    define YARP_DEBUG(log, x) ((yarp::os::impl::Logger*)&(log))->internal_debug(x)
#else
#    define YARP_DEBUG(log, x) YARP_UNUSED(log)
#endif
#define YARP_FAIL(log, x) ((yarp::os::impl::Logger*)&(log))->internal_fail(x)

#define YARP_LONGEST_MESSAGE 1000
#define YARP_SPRINTF0(log, mode, msg)                         \
    {                                                         \
        char _yarp_buf[YARP_LONGEST_MESSAGE];                 \
        snprintf(&(_yarp_buf[0]), YARP_LONGEST_MESSAGE, msg); \
        (log).internal_##mode(&(_yarp_buf[0]));               \
    }
#define YARP_SPRINTF1(log, mode, msg, a)                         \
    {                                                            \
        char _yarp_buf[YARP_LONGEST_MESSAGE];                    \
        snprintf(&(_yarp_buf[0]), YARP_LONGEST_MESSAGE, msg, a); \
        (log).internal_##mode(&(_yarp_buf[0]));                  \
    }
#define YARP_SPRINTF2(log, mode, msg, a, b)                         \
    {                                                               \
        char _yarp_buf[YARP_LONGEST_MESSAGE];                       \
        snprintf(&(_yarp_buf[0]), YARP_LONGEST_MESSAGE, msg, a, b); \
        (log).internal_##mode(&(_yarp_buf[0]));                     \
    }
#define YARP_SPRINTF3(log, mode, msg, a, b, c)                         \
    {                                                                  \
        char _yarp_buf[YARP_LONGEST_MESSAGE];                          \
        snprintf(&(_yarp_buf[0]), YARP_LONGEST_MESSAGE, msg, a, b, c); \
        (log).internal_##mode(&(_yarp_buf[0]));                        \
    }
#define YARP_SPRINTF4(log, mode, msg, a, b, c, d)                         \
    {                                                                     \
        char _yarp_buf[YARP_LONGEST_MESSAGE];                             \
        snprintf(&(_yarp_buf[0]), YARP_LONGEST_MESSAGE, msg, a, b, c, d); \
        (log).internal_##mode(&(_yarp_buf[0]));                           \
    }
#define YARP_SPRINTF5(log, mode, msg, a, b, c, d, e)                         \
    {                                                                        \
        char _yarp_buf[YARP_LONGEST_MESSAGE];                                \
        snprintf(&(_yarp_buf[0]), YARP_LONGEST_MESSAGE, msg, a, b, c, d, e); \
        (log).internal_##mode(&(_yarp_buf[0]));                              \
    }


#endif // YARP_OS_IMPL_LOGGER_H
