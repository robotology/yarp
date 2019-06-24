/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_SPLITSTRING_H
#define YARP_OS_IMPL_SPLITSTRING_H

#include <yarp/os/api.h>

#define MAX_ARG_CT (20)
#define MAX_ARG_LEN (256)

namespace yarp {
namespace os {
namespace impl {

/**
 * Split a string into pieces.
 * This class is a bit crufty, and should be replaced with a simple Bottle
 * (now that Bottle exists).
 */
class YARP_OS_impl_API SplitString
{
public:
    SplitString();

    SplitString(const char* command, const char splitter = ' ');

    int size();

    void set(int index, const char* txt);

    const char* get(int idx);

    const char** get();

    void apply(const char* command, char splitter = ' ');

private:
    int argc;
    const char* argv[MAX_ARG_CT];
    char buf[MAX_ARG_CT][MAX_ARG_LEN];
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_SPLITSTRING_H
