/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_BYTES_H
#define YARP_OS_BYTES_H

#include <yarp/os/api.h>

#include <cstddef>


namespace yarp {
namespace os {

/**
 * \brief A simple abstraction for a block of bytes.
 *
 * This class is not responsible for allocating or destroying those bytes, just
 * recording their location.
 */
class YARP_os_API Bytes
{
public:
    /**
     * Default constructor.  No data present.
     */
    explicit Bytes();

    /**
     * Constructor.
     * @param data address of data block
     * @param len length of data block
     */
    Bytes(char* data, size_t len);

    /**
     * @return length of data block
     */
    size_t length() const;

    /**
     * @return address of data block. Const version.
     */
    const char* get() const;

    /**
     * @return address of data block.
     */
    char* get();

private:
    char* data;
    size_t len;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_BYTES_H
