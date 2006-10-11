// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_STAMP_
#define _YARP2_STAMP_

#include <yarp/os/NetInt32.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/Portable.h>

namespace yarp {
    namespace os {
        class Stamp;
    }
}

class yarp::os::Stamp {
private:
    NetInt32 sequenceNumber;
    NetFloat64 timeStamp;
public:
    Stamp() {
        sequenceNumber = -1;
        timeStamp = 0;
    }

    Stamp(int sequenceNumber, double timeStamp) :
        sequenceNumber(sequenceNumber),
        timeStamp(timeStamp) {
    }

    int getCount() {
        return sequenceNumber;
    }

    double getTime() {
        return timeStamp;
    }

    bool isValid() {
        return sequenceNumber>=0;
    }

    virtual bool read(ConnectionReader& connection);

    virtual bool write(ConnectionWriter& connection);
};

#endif
