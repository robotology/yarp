/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformThread.h>
#include <yarp/os/Os.h>
#include <yarp/os/Network.h>

#include <cstdio>

#ifdef YARP_HAS_ACE
# include <ace/Log_Msg.h>
# include <ace/Log_Record.h>
#endif

using namespace yarp::os::impl;
using namespace yarp::os;


Logger::Logger(const char *prefix, Logger *parent) :
        prefix(prefix),
        parent(parent),
        verbose(0),
        low(DEFAULT_WARN),
        pid(yarp::os::getpid()),
        stream(nullptr)
{
#ifdef YARP_HAS_ACE
    if (parent == nullptr) {
        ACE_Log_Msg *acer = ACE_Log_Msg::instance();
        acer->set_flags(8);
        acer->clr_flags(1);
        acer->msg_callback(this);
    }
#endif
}


Logger::Logger(const char *prefix, Logger& parent) :
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


#ifdef YARP_HAS_ACE
void Logger::log(ACE_Log_Record& log_record)
{
    show(log_record.type(), log_record.msg_data());
}
#endif


void Logger::println(const ConstString& txt)
{
    internal_debug(txt);
}


void Logger::internal_debug(const ConstString& txt)
{
    show(LM_DEBUG, txt);
}


void Logger::internal_info(const ConstString& txt)
{
    show(LM_INFO, txt);
}


void Logger::internal_warning(const ConstString& txt)
{
    show(LM_WARNING, txt);
}


void Logger::internal_error(const ConstString& txt)
{
    show(LM_ERROR, txt);
}


void Logger::internal_fail(const ConstString& txt)
{
    show(LM_ERROR, txt);
    std::exit(1);
}


void Logger::internal_debug(const char *txt)
{
    ConstString stxt(txt);
    show(LM_DEBUG, stxt);
}


void Logger::internal_info(const char *txt)
{
    ConstString stxt(txt);
    show(LM_INFO, stxt);
}


void Logger::internal_warning(const char *txt)
{
    ConstString stxt(txt);
    show(LM_WARNING, stxt);
}


void Logger::internal_error(const char *txt)
{
    ConstString stxt(txt);
    show(LM_ERROR, stxt);
}


void Logger::internal_fail(const char *txt)
{
    ConstString stxt(txt);
    show(LM_ERROR, stxt);
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


void Logger::setPrefix(const char *prefix)
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
    return (verbose>=0);
}


bool Logger::shouldShowError()
{
    return true;
}


bool Logger::shouldShowDebug()
{
    return (verbose>0);
}


void Logger::show(unsigned YARP_INT32 level, const ConstString& txt)
{
    unsigned YARP_INT32 inLevel = level;
    //fprintf(stderr, "level %d txt %s\n", level, txt.c_str());
    if (verbose>0) {
        level = 10000;
    }
    if (verbose<0) {
        level = 0;
    }
    if (stream == nullptr) {
        stream = stderr;
        if (NetworkBase::getEnvironment("YARP_LOGGER_STREAM") == "stdout") {
            stream = stdout;
        }
    }
    if (parent == nullptr) {
        if (level>=low) {
            if (inLevel<=LM_DEBUG) {
                fprintf(stream, "%s(%04x): %s\n",
                                prefix.c_str(),
                                (int)(long int)(PLATFORM_THREAD_SELF()),
                                txt.c_str());
            } else {
                fprintf(stream, "%s: %s\n", prefix.c_str(), txt.c_str());
            }
            fflush(stream);
        }
    } else {
        ConstString more(prefix);
        more += ": ";
        more += txt;
        parent->show(inLevel, more);
    }
}
