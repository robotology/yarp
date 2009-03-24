// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_STAMP_
#define _YARP2_STAMP_

#include <yarp/os/NetInt32.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/Portable.h>

namespace yarp {
    namespace os {
        class Stamp;
        class Stamped;
    }
}

/**
 * An abstraction for a time stamp and/or sequence number.
 */
class yarp::os::Stamp : public Portable {
private:
    NetInt32 sequenceNumber;
    NetFloat64 timeStamp, timeZero;
public:
    /**
     * Constuct an invalid Stamp.
     */
    Stamp() {
        sequenceNumber = -1;
        timeStamp = 0;
        timeZero = 0;
    }

    /**
     * Constuct a Stamp with a given sequence number and time.
     * @param count the sequence number
     * @param time the time stamp (in seconds, relative to an arbitrary
     * zero time)
     */
    Stamp(int count, double time) :
        sequenceNumber(count),
        timeStamp(time) {
        timeZero = 0;
    }

    /**
     * @return the sequence number
     */
    int getCount() {
        return sequenceNumber;
    }

    /**
     * @return the time stamp
     */
    double getTime() {
        return timeStamp;
    }

    /**
     * @return true if this is a valid Stamp
     */
    bool isValid() {
        return sequenceNumber>=0;
    }


    /**
     * @returns the maximum sequence number, after which
     * an incrementing sequence should return to zero.
     */
    int getMaxCount();


    /**
     * This method sets the timestamp to the current time,
     * and increments the sequence number (wrapping to 0
     * if the sequence number exceeds Stamp::getMaxCount)
     */
    void update();
 
    /**
     * This method set the timestamp to a given time,
     * and increments the sequence number (wrapping to 0
     * if the sequence exceeds Stamp::getMaxCount)
     */
    void update(double time);

    virtual bool read(ConnectionReader& connection);

    virtual bool write(ConnectionWriter& connection);
};


/**
 * A base class for objects with time stamps and/or sequence numbers.
 */
class yarp::os::Stamped {
public:
    virtual ~Stamped() {}

    /**
     * @return the Stamp associated with this object (time stamp,
     * sequence number).
     */
    virtual Stamp getStamp() const = 0;
};


#endif
