// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_SPLITSTRING
#define YARP2_SPLITSTRING

#include <yarp/os/impl/PlatformStdlib.h>

namespace yarp {
    namespace os {
        namespace impl {
            class SplitString;
        }
    }
}


#define MAX_ARG_CT (20)
#define MAX_ARG_LEN (256)


/**
 * Split a string into pieces.
 * This class is a bit crufty, and should be replaced with a simple Bottle
 * (now that Bottle exists).
 */
class yarp::os::impl::SplitString {
private:
    int argc;
    const char *argv[MAX_ARG_CT];
    char buf[MAX_ARG_CT][MAX_ARG_LEN];

public:
    SplitString() {
        argc = 0;
    }

    SplitString(const char *command, const char splitter = ' ') {
        apply(command,splitter);
    }

    int size() {
        return argc;
    }

    void set(int index, const char *txt) {
        if (index>=0&&index<size()) {
            ACE_OS::strncpy(buf[index],(char*)txt,MAX_ARG_LEN);
        }
    }

    const char *get(int idx) {
        return buf[idx];
    }

    const char **get() {
        return (const char **)argv;
    }

    void apply(const char *command, char splitter=' ') {
        int at = 0;
        int sub_at = 0;
        unsigned int i;
        for (i=0; i<strlen(command)+1; i++) {
            if (at<MAX_ARG_CT) {
                // yarpserver3 can deal with quoting.
                // old yarpserver just gets confused.
                char ch = command[i];
                if (ch=='\"') {
                    ch = ' ';
                }
                if (ch>=32||ch=='\0'||ch=='\n') {
                    if (ch==splitter||ch=='\n') {
                        ch = '\0';
                    }
                    if (sub_at<MAX_ARG_LEN) {
                        buf[at][sub_at] = ch;
                        sub_at++;
                    }
                }
                if (ch == '\0') {
                    if (sub_at>1) {
                        at++;
                    }
                    sub_at = 0;
                } 
            }
        }
        for (i=0; i<MAX_ARG_CT; i++) {
            argv[i] = buf[i];
            buf[i][MAX_ARG_LEN-1] = '\0';
        }

        argc = at;
    }
};

#endif

