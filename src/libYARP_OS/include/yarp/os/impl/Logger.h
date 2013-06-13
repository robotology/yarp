// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_LOGGER_
#define _YARP2_LOGGER_

#include <stdio.h>
#include <yarp/os/impl/String.h>

#ifdef YARP_HAS_ACE
#  include <ace/Log_Msg.h>
#  include <ace/Log_Record.h>
#  include <ace/Log_Msg_Callback.h>
#else
#  define LM_DEBUG -1
#  define LM_WARNING 1
#  define LM_INFO 2
#  define LM_ERROR 3
#endif
#include <yarp/os/impl/PlatformStdio.h>

#include <yarp/os/Log.h>

namespace yarp {
    namespace os {
        namespace impl {
            class Logger;
        }
    }
}

/**
 * This is a wrapper for message logging.
 * This is currently a sad mixture of the java yarp logging mechanism
 * and ACE.
 *
 */
class YARP_OS_impl_API yarp::os::impl::Logger
#ifdef YARP_HAS_ACE
    : public ACE_Log_Msg_Callback
#endif
{
public:
    enum Level {
        MAJOR_DEBUG=LM_INFO,
        DEFAULT_WARN=LM_INFO,
    };

    Logger(const char *prefix, Logger *parent = NULL) {
        this->prefix = prefix;
        this->parent = parent;
        verbose = 0;
        low = DEFAULT_WARN;
        stream = NULL;
#ifdef YARP_HAS_ACE
        if (this==&root) {
            ACE_Log_Msg *acer = ACE_Log_Msg::instance();
            acer->set_flags(8);
            acer->clr_flags(1);
            acer->msg_callback(this);
        }
#endif
    }

    Logger(const char *prefix, Logger& parent) {
        this->prefix = prefix;
        this->parent = &parent;
        verbose = 0;
        stream = NULL;
        low = DEFAULT_WARN;
    }

    static Logger& get();

#ifdef YARP_HAS_ACE
    virtual void log(ACE_Log_Record& log_record) {
        show(log_record.type(),log_record.msg_data());
    }
#endif

    void debug(const String& txt) {
        show(LM_DEBUG,txt);
    }

    void println(const String& txt) {
        debug(txt);
    }

    void info(const String& txt) {
        show(LM_INFO,txt);
    }

    void warning(const String& txt) {
        show(LM_WARNING,txt);
    }

    void error(const String& txt) {
        show(LM_ERROR,txt);
    }

    void fail(const String& txt) {
        show(LM_ERROR,txt);
        exit(1);
    }

    void assertion(bool cond) {
        if (!cond) {
            fail("assertion failure");
        }
    }

    void setVerbosity(int verbose = 0) {
        this->verbose = verbose;
    }

    void setPrefix(const char *prefix) {
        this->prefix = prefix;
    }

    void setPid();

    long int getPid() {
        return pid;
    }

    bool shouldShowInfo() {
        return (verbose>=0);
    }

    bool shouldShowError() {
        return true;
    }

    bool shouldShowDebug() {
        return (verbose>0);
    }

private:
    void show(int level, const String& txt);
    void exit(int result);

    static Logger root;
    String prefix;
    Logger *parent;
    int verbose;
    int low;
    long int pid;
    FILE *stream;
};

// compromise - use macros so that debugging can evaporate in optimized code.
// also, make a printf-style adaptor since c++ is a bit of a pain to
// build strings in.
#define YARP_ERROR(log,x) log.error(x)
#define YARP_WARN(log,x)  log.warning(x)
#define YARP_INFO(log,x)  log.info(x)
#define YARP_DEBUG(log,x) log.debug(x)
#define YARP_FAIL(log,x)  log.fail(x)

#define YARP_LONGEST_MESSAGE 1000
#define YARP_SPRINTF0(log,mode,msg)  { char _yarp_buf[YARP_LONGEST_MESSAGE]; ACE_OS::snprintf(_yarp_buf,YARP_LONGEST_MESSAGE,msg); log.mode(_yarp_buf); }
#define YARP_SPRINTF1(log,mode,msg,a)  { char _yarp_buf[YARP_LONGEST_MESSAGE]; ACE_OS::snprintf(_yarp_buf,YARP_LONGEST_MESSAGE,msg,a); log.mode(_yarp_buf); }
#define YARP_SPRINTF2(log,mode,msg,a,b)  { char _yarp_buf[YARP_LONGEST_MESSAGE]; ACE_OS::snprintf(_yarp_buf,YARP_LONGEST_MESSAGE,msg,a,b); log.mode(_yarp_buf); }
#define YARP_SPRINTF3(log,mode,msg,a,b,c)  { char _yarp_buf[YARP_LONGEST_MESSAGE]; ACE_OS::snprintf(_yarp_buf,YARP_LONGEST_MESSAGE,msg,a,b,c); log.mode(_yarp_buf); }
#define YARP_SPRINTF4(log,mode,msg,a,b,c,d)  { char _yarp_buf[YARP_LONGEST_MESSAGE]; ACE_OS::snprintf(_yarp_buf,YARP_LONGEST_MESSAGE,msg,a,b,c,d); log.mode(_yarp_buf); }
#define YARP_SPRINTF5(log,mode,msg,a,b,c,d,e)  { char _yarp_buf[YARP_LONGEST_MESSAGE]; ACE_OS::snprintf(_yarp_buf,YARP_LONGEST_MESSAGE,msg,a,b,c,d,e); log.mode(_yarp_buf); }



#endif
