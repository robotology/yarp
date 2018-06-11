/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_FAKEFACE_H
#define YARP_OS_IMPL_FAKEFACE_H

#include <yarp/os/Face.h>

namespace yarp {
    namespace os {
        namespace impl {
            class FakeFace;
        }
    }
}

/**
 * A dummy Face for testing purposes.
 */
class YARP_OS_impl_API yarp::os::impl::FakeFace : public yarp::os::Face
{
public:
    virtual bool open(const Contact& address) override;
    virtual void close() override;
    virtual InputProtocol *read() override;
    virtual OutputProtocol *write(const Contact& address) override;
private:
};

#endif // YARP_OS_IMPL_FAKEFACE_H
