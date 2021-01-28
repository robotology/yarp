/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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

/**
 * A dummy Face for testing purposes.
 */
class YARP_os_impl_API FakeFace :
        public yarp::os::Face
{
public:
    bool open(const Contact& address) override;
    void close() override;
    InputProtocol* read() override;
    OutputProtocol* write(const Contact& address) override;

private:
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_FAKEFACE_H
