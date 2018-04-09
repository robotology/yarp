/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/Os.h>
#include <yarp/os/Network.h>
#include <yarp/conf/system.h>

using namespace yarp::os::impl;
using namespace yarp::os;

Logger& Logger::get() {
    static Logger instance("yarp");
    return instance;
}

void Logger::fini() {
}


void Logger::show(unsigned YARP_INT32 level, const ConstString& txt) {
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
                fprintf(stream, "%s(%04lx): %s\n",
                                prefix.c_str(),
                                ThreadImpl::getKeyOfCaller(),
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


void Logger::exit(int level) {
    std::exit(level);
}


void Logger::setPid() {
    pid = yarp::os::getpid();
}
