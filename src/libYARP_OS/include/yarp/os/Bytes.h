// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_BYTES_
#define _YARP2_BYTES_

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class Bytes;
    }
}


/**
 * A simple abstraction for a block of bytes.  This class is not
 * responsible for allocating or destroying those bytes, just
 * recording their location.
 */
class YARP_OS_API yarp::os::Bytes {
public:
    /**
     * Default constructor.  No data present.
     */
    Bytes() {
        data = 0;
        len = 0;
    }

    /**
     * Constructor.
     * @param data address of data block
     * @param len length of data block
     */
    Bytes(char *data, int len) {
        this->data = data;
        this->len = len;
    }

    /**
     * @return length of data block
     */
    int length() const {
        return len;
    }

    /**
     * @return address of data block
     */
    char *get() const {
        return data;
    }

private:
    char *data;
    int len;
};

#endif

