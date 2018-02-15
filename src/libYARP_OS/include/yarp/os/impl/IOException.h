/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
