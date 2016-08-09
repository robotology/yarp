/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_LOGGER
#define YARP2_LOGGER

#include <stdio.h>
#include <yarp/conf/api.h>
#include <yarp/conf/system.h>
#include <yarp/os/ConstString.h>

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
class YARP_OS_impl_API yarp::os::impl::Logger : public yarp::os::Log
#ifdef YARP_HAS_ACE
                                              , public ACE_Log_Msg_Callback
#endif
{
public:
    enum Level {
        MAJOR_DEBUG=LM_INFO,
        DEFAULT_WARN=LM_INFO
    };

    Logger(const char *prefix, Logger *parent = NULL) {
        this->prefix = prefix;
        this->parent = parent;
        verbose = 0;
        low = DEFAULT_WARN;
        stream = NULL;
#ifdef YARP_HAS_ACE
        if (this==root) {
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
    static void fini();

#ifdef YARP_HAS_ACE
    virtual void log(ACE_Log_Record& log_record) {
        show(log_record.type(),log_record.msg_data());
    }
#endif

    void println(const ConstString& txt) {
        internal_debug(txt);
    }



    void internal_debug(const ConstString& txt) {
        show(LM_DEBUG,txt);
    }

    void internal_info(const ConstString& txt) {
        show(LM_INFO,txt);
    }

    void internal_warning(const ConstString& txt) {
        show(LM_WARNING,txt);
    }

    void internal_error(const ConstString& txt) {
        show(LM_ERROR,txt);
    }

    void internal_fail(const ConstString& txt) {
        show(LM_ERROR,txt);
        exit(1);
    }



    void internal_debug(const char *txt) {
        ConstString stxt(txt);
        show(LM_DEBUG,stxt);
    }

    void internal_info(const char *txt) {
        ConstString stxt(txt);
        show(LM_INFO,stxt);
    }

    void internal_warning(const char *txt) {
        ConstString stxt(txt);
        show(LM_WARNING,stxt);
    }

    void internal_error(const char *txt) {
        ConstString stxt(txt);
        show(LM_ERROR,stxt);
    }

    void internal_fail(const char *txt) {
        ConstString stxt(txt);
        show(LM_ERROR,stxt);
        exit(1);
    }


    void assertion(bool cond) {
        if (!cond) {
            internal_fail("assertion failure");
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
    void show(unsigned YARP_INT32 level, const ConstString& txt);
    void exit(int level);

    static Logger *root;
    ConstString prefix;
    Logger *parent;
    int verbose;
    unsigned YARP_INT32 low;
    long int pid;
    FILE *stream;
};

// compromise - use macros so that debugging can evaporate in optimized code.
// also, make a printf-style adaptor since c++ is a bit of a pain to
// build strings in.
#define YARP_ERROR(log,x) ((Logger*)&log)->internal_error(x)
#define YARP_WARN(log,x)  ((Logger*)&log)->internal_warning(x)
#define YARP_INFO(log,x)  ((Logger*)&log)->internal_info(x)
#ifndef NDEBUG
#  define YARP_DEBUG(log,x) ((Logger*)&log)->internal_debug(x)
#else
#  define YARP_DEBUG(log,x) YARP_UNUSED(log)
#endif
#define YARP_FAIL(log,x)  ((Logger*)&log)->internal_fail(x)

#define YARP_LONGEST_MESSAGE 1000
#define YARP_SPRINTF0(log,mode,msg)  { char _yarp_buf[YARP_LONGEST_MESSAGE]; ACE_OS::snprintf(_yarp_buf,YARP_LONGEST_MESSAGE,msg); log.internal_ ## mode(_yarp_buf); }
#define YARP_SPRINTF1(log,mode,msg,a)  { char _yarp_buf[YARP_LONGEST_MESSAGE]; ACE_OS::snprintf(_yarp_buf,YARP_LONGEST_MESSAGE,msg,a); log.internal_ ## mode(_yarp_buf); }
#define YARP_SPRINTF2(log,mode,msg,a,b)  { char _yarp_buf[YARP_LONGEST_MESSAGE]; ACE_OS::snprintf(_yarp_buf,YARP_LONGEST_MESSAGE,msg,a,b); log.internal_ ## mode(_yarp_buf); }
#define YARP_SPRINTF3(log,mode,msg,a,b,c)  { char _yarp_buf[YARP_LONGEST_MESSAGE]; ACE_OS::snprintf(_yarp_buf,YARP_LONGEST_MESSAGE,msg,a,b,c); log.internal_ ## mode(_yarp_buf); }
#define YARP_SPRINTF4(log,mode,msg,a,b,c,d)  { char _yarp_buf[YARP_LONGEST_MESSAGE]; ACE_OS::snprintf(_yarp_buf,YARP_LONGEST_MESSAGE,msg,a,b,c,d); log.internal_ ## mode(_yarp_buf); }
#define YARP_SPRINTF5(log,mode,msg,a,b,c,d,e)  { char _yarp_buf[YARP_LONGEST_MESSAGE]; ACE_OS::snprintf(_yarp_buf,YARP_LONGEST_MESSAGE,msg,a,b,c,d,e); log.internal_ ## mode(_yarp_buf); }



#endif
