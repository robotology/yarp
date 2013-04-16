// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformThread.h>
#include <yarp/os/Network.h>
#include <yarp/conf/system.h>

using namespace yarp::os::impl;
using namespace yarp::os;

Logger Logger::root("yarp");

Logger& Logger::get() {
    return root;
}

#ifdef YARP_HAS_EXECINFO
#include <execinfo.h>
void yarp_print_trace(FILE *out, const char *file, int line) {
    const size_t max_depth = 100;
    size_t stack_depth;
    void *stack_addrs[max_depth];
    char **stack_strings;
    stack_depth = backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);
    fprintf(out, "Assertion thrown at %s:%d by code called from:\n", file, line);
    for (size_t i = 1; i < stack_depth; i++) {
        fprintf(out, " --> %s\n", stack_strings[i]);
    }
    free(stack_strings); // malloc()ed by backtrace_symbols
    fflush(out);
}
#else
void yarp_print_trace(FILE *out, const char *file, int line) {
    // Not implemented on this platform
}
#endif

void Logger::show(int level, const String& txt) {
    int inLevel = level;
    //ACE_OS::fprintf(stderr,"level %d txt %s\n", level, txt.c_str());
    if (verbose>0) {
        level = 10000;
    }
    if (verbose<0) {
        level = -10000;
    }
    if (stream == NULL) {
        stream = stderr;
        if (NetworkBase::getEnvironment("YARP_LOGGER_STREAM") == "stdout") {
            stream = stdout;
        }
    }
    if (parent == NULL) {
        if (level>=low) {
            if (inLevel<=LM_DEBUG) {
                ACE_OS::fprintf(stream,"%s(%04x): %s\n",
                                prefix.c_str(),
                                (int)(long int)(PLATFORM_THREAD_SELF()),
                                txt.c_str());
            } else {
                ACE_OS::fprintf(stream,"%s: %s\n",prefix.c_str(),txt.c_str());
            }
            ACE_OS::fflush(stream);
        }
    } else {
        String more(prefix);
        more += ": ";
        more += txt;
        parent->show(inLevel,more);
    }
}


void Logger::exit(int level) {
    ACE_OS::exit(level);
}


void Logger::setPid() {
    pid = ACE_OS::getpid();
}


void __yarp_error(const char *str) {
    YARP_ERROR(Logger::get(),str);
}

void __yarp_warn(const char *str) {
    YARP_WARN(Logger::get(),str);
}

void __yarp_info(const char *str) {
    YARP_INFO(Logger::get(),str);
}

void __yarp_debug(const char *str) {
    YARP_DEBUG(Logger::get(),str);
}


bool yarp_show_error() {
    return Logger::get().shouldShowError();
}

bool yarp_show_warn() {
    return Logger::get().shouldShowError();
}

bool yarp_show_info() {
    return Logger::get().shouldShowInfo();
}

bool yarp_show_debug() {
    return Logger::get().shouldShowDebug();
}

