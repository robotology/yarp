// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_BYTES_
#define _YARP2_BYTES_


namespace yarp {
    class Bytes;
}


/**
 * A simple abstraction for a block of bytes.  This class is not
 * responsible for allocating or destroying those bytes, just
 * recording their location.
 */
class yarp::Bytes {
public:
    Bytes() {
        data = 0;
        len = 0;
    }

    Bytes(char *data, int len) {
        this->data = data;
        this->len = len;
    }

    int length() const {
        return len;
    }

    char *get() const {
        return data;
    }

private:
    char *data;
    int len;
};

#endif

