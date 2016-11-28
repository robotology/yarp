/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP2_SPLITSTRING
#define YARP2_SPLITSTRING

#include <yarp/os/api.h>

#define MAX_ARG_CT (20)
#define MAX_ARG_LEN (256)

namespace yarp {
    namespace os {
        namespace impl {
            class SplitString;
        }
    }
}


/**
 * Split a string into pieces.
 * This class is a bit crufty, and should be replaced with a simple Bottle
 * (now that Bottle exists).
 */
class YARP_OS_impl_API yarp::os::impl::SplitString {
public:
    SplitString();

    SplitString(const char *command, const char splitter = ' ');

    int size();

    void set(int index, const char *txt);

    const char *get(int idx);

    const char **get();

    void apply(const char *command, char splitter=' ');

private:
    int argc;
    const char *argv[MAX_ARG_CT];
    char buf[MAX_ARG_CT][MAX_ARG_LEN];
};

#endif

