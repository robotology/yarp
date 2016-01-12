/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_BYTES_H
#define YARP_OS_BYTES_H

#include <yarp/os/api.h>
#include <stddef.h> //defines size_t

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
