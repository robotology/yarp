/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/Logger.h>

#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/ThreadImpl.h>

#include <cstdio>

using namespace yarp::os::impl;
using namespace yarp::os;


Logger::Logger(const char* prefix, Logger* parent) :
        prefix(prefix),
        parent(parent),
        verbose(0),
        low(DEFAULT_WARN),
        pid(yarp::os::getpid()),
        stream(nullptr)
{
}


Logger::Logger(const char* prefix, Logger& parent) :
        prefix(prefix),
        parent(&parent),
        verbose(0),
        low(DEFAULT_WARN),
        pid(yarp::os::getpid()),
        stream(nullptr)
{
}


Logger& Logger::get()
{
    static Logger instance("yarp");
    return instance;
}

void Logger::println(const std::string& txt)
{
    internal_debug(txt);
}


void Logger::internal_debug(const std::string& txt)
{
    show(YARP_LM_DEBUG, txt);
}


void Logger::internal_info(const std::string& txt)
{
    show(YARP_LM_INFO, txt);
}


void Logger::internal_warning(const std::string& txt)
{
    show(YARP_LM_WARNING, txt);
}


void Logger::internal_error(const std::string& txt)
{
    show(YARP_LM_ERROR, txt);
}


void Logger::internal_fail(const std::string& txt)
{
    show(YARP_LM_ERROR, txt);
    std::exit(1);
}


void Logger::internal_debug(const char* txt)
{
    std::string stxt(txt);
    show(YARP_LM_DEBUG, stxt);
}


void Logger::internal_info(const char* txt)
{
    std::string stxt(txt);
    show(YARP_LM_INFO, stxt);
}


void Logger::internal_warning(const char* txt)
{
    std::string stxt(txt);
    show(YARP_LM_WARNING, stxt);
}


void Logger::internal_error(const char* txt)
{
    std::string stxt(txt);
    show(YARP_LM_ERROR, stxt);
}


void Logger::internal_fail(const char* txt)
{
    std::string stxt(txt);
    show(YARP_LM_ERROR, stxt);
    std::exit(1);
}


void Logger::assertion(bool cond)
{
    if (!cond) {
        internal_fail("assertion failure");
    }
}


void Logger::setVerbosity(int verbose)
{
    this->verbose = verbose;
}


void Logger::setPrefix(const char* prefix)
{
    this->prefix = prefix;
}


void Logger::setPid()
{
    pid = yarp::os::getpid();
}


long int Logger::getPid()
{
    return pid;
}


bool Logger::shouldShowInfo()
{
    return (verbose >= 0);
}


bool Logger::shouldShowError()
{
    return true;
}


bool Logger::shouldShowDebug()
{
    return (verbose > 0);
}


void Logger::show(std::uint32_t level, const std::string& txt)
{
    std::uint32_t inLevel = level;
    //fprintf(stderr, "level %d txt %s\n", level, txt.c_str());
    if (verbose > 0) {
        level = 10000;
    }
    if (verbose < 0) {
        level = 0;
    }
    if (stream == nullptr) {
        stream = stderr;
        if (NetworkBase::getEnvironment("YARP_LOGGER_STREAM") == "stdout") {
            stream = stdout;
        }
    }
    if (parent == nullptr) {
        if (level >= low) {
            if (inLevel <= YARP_LM_DEBUG) {
                fprintf(stream, "%s(%04lx): %s\n", prefix.c_str(), ThreadImpl::getKeyOfCaller(), txt.c_str());
            } else {
                fprintf(stream, "%s: %s\n", prefix.c_str(), txt.c_str());
            }
            fflush(stream);
        }
    } else {
        std::string more(prefix);
        more += ": ";
        more += txt;
        parent->show(inLevel, more);
    }
}
