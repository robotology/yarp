/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_BINPORTABLE_H
#define YARP_OS_BINPORTABLE_H

#include <yarp/os/Portable.h>

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
    bool read(ConnectionReader& connection) override;

    // Documented in Portable
    bool write(ConnectionWriter& connection) const override;
};

} // namespace os
} // namespace yarp

#include <yarp/os/BinPortable-inl.h>

#endif // YARP_OS_BINPORTABLE_H
