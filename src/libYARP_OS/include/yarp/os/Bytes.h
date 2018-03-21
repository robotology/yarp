/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_BYTES_H
#define YARP_OS_BYTES_H

#include <yarp/os/api.h>
#include <cstddef> //defines size_t

namespace yarp {
    namespace os {
        class Bytes;
    }
}


/**
 * \brief A simple abstraction for a block of bytes.
 *
 * This class is not responsible for allocating or destroying those bytes, just
 * recording their location.
 */
class YARP_OS_API yarp::os::Bytes {
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
    Bytes(char *data, size_t len);

    /**
     * @return length of data block
     */
    size_t length() const;

    /**
     * @return address of data block
     */
    char *get() const;

private:
    char *data;
    size_t len;
};

#endif // YARP_OS_BYTES_H
