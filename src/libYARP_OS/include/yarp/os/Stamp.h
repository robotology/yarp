/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_STAMP_H
#define YARP_OS_STAMP_H

#include <yarp/os/NetInt32.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/Portable.h>


namespace yarp {
namespace os {

/**
 * An abstraction for a time stamp and/or sequence number.
 */
class YARP_OS_API Stamp : public Portable
{
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
    virtual bool read(ConnectionReader& connection) override;

    // Documented in Portable
    virtual bool write(ConnectionWriter& connection) override;
};


/**
 * A base class for objects with time stamps and/or sequence numbers.
 */
class YARP_OS_DEPRECATED_API Stamped
{
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

} // namespace os
} // namespace yarp

#endif // YARP_OS_STAMP_H
