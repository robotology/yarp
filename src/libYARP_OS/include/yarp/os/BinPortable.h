/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_BINPORTABLE_H
#define YARP_OS_BINPORTABLE_H

#include <yarp/os/Portable.h>

namespace yarp {
    namespace os {
        template <class T> class BinPortable;
    }
}

namespace yarp {
namespace os {

/**
 * @ingroup comm_class
 *
 * @brief Class for writing and reading simple binary structures.
 * Can be used in conjunction with the Port class to send data
 * across the network.
 * Don't use this for anything containing a pointer,
 * or which needs to be portable across different compilers,
 * languages, operating systems, or processor architectures.
 */
template <class T>
class BinPortable : public Portable
{
private:
    T t;

public:
    /**
     * Get the internal structure that will be read or written.
     *
     * @return the internal structure that will be read or written.
     */
    T& content();

    // Documented in Portable
    virtual bool read(ConnectionReader& connection) override;

    // Documented in Portable
    virtual bool write(ConnectionWriter& connection) override;
};

} // namespace os
} // namespace yarp

#include <yarp/os/BinPortable-inl.h>

#endif // YARP_OS_BINPORTABLE_H
