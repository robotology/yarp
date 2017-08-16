/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_IOEXCEPTION_H
#define YARP_OS_IMPL_IOEXCEPTION_H

#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        namespace impl {
            class IOException;
        }
    }
}

/**
 * General communication error.
 */
class yarp::os::impl::IOException
{
public:
    IOException(const char *txt = nullptr)
    {
        if (txt!=nullptr) {
            desc = txt;
        }
    }

    ConstString toString() const
    {
        return desc;
    }
private:
    ConstString desc;
};

#endif // YARP_OS_IMPL_IOEXCEPTION_H
