// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_STAMP
#define YARP2_STAMP

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
class YARP_OS_API yarp::os::Stamp : public Portable {
private:
    NetInt32 sequenceNumber;
    NetFloat64 timeStamp;
public:
    /**
     * Constuct an invalid Stamp.
     */
    explicit Stamp();

    /**
     * Constuct a Stamp with a given sequence number and time.
     *
     * @param count the sequence number.
     * @param time the time stamp (in seconds, relative to an arbitrary.
     * zero time)
     */
    Stamp(int count, double time);

    /**
     * Get the sequence number.
     *
     * @return the sequence number.
     */
    int getCount();

    /**
     * Get the time stamp.
     *
     * @return the time stamp
     */
    double getTime();

    /**
     * Check if this Stamp is valid.
     *
     * @return true if this is a valid Stamp
     */
    bool isValid();

    /**
     * Get the maximum sequence number, after which an incrementing sequence
     * should return to zero.
     *
     * @return the maximum sequence number.
     */
    int getMaxCount();

    /**
     * Set the timestamp to the current time, and increment the sequence number
     * (wrapping to 0 if the sequence number exceeds Stamp::getMaxCount())
     */
    void update();

    /**
     * Set the timestamp to a given time, and increments the sequence number
     * (wrapping to 0 if the sequence exceeds Stamp::getMaxCount())
     */
    void update(double time);

    // Documented in Portable
    virtual bool read(ConnectionReader& connection);

    // Documented in Portable
    virtual bool write(ConnectionWriter& connection);
};


/**
 * A base class for objects with time stamps and/or sequence numbers.
 */
class YARP_OS_API yarp::os::Stamped {
public:
    /**
     * Destructor
     */
    virtual ~Stamped();

    /**
     * @return the Stamp associated with this object (time stamp,
     * sequence number).
     */
    virtual Stamp getStamp() const = 0;
};


#endif
