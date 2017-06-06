/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
class yarp::os::impl::FakeFace : public yarp::os::Face
{
public:
    virtual bool open(const Contact& address) override;
    virtual void close() override;
    virtual InputProtocol *read() override;
    virtual OutputProtocol *write(const Contact& address) override;
private:
};

#endif // YARP_OS_IMPL_FAKEFACE_H
