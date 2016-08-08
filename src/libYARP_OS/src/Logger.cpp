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

Logger *Logger::root = NULL;

Logger& Logger::get() {
    if (!root) root = new Logger("yarp");
    return *root;
}

void Logger::fini() {
    if (root) delete root;
    root = NULL;
}


void Logger::show(ACE_UINT32 level, const ConstString& txt) {
    ACE_UINT32 inLevel = level;
    //ACE_OS::fprintf(stderr,"level %d txt %s\n", level, txt.c_str());
    if (verbose>0) {
        level = 10000;
    }
    if (verbose<0) {
        level = 0;
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
        ConstString more(prefix);
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
