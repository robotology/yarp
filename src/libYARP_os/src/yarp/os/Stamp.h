/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_STAMP_H
#define YARP_OS_STAMP_H

#include <yarp/os/NetFloat64.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/Portable.h>


namespace yarp {
namespace os {

/**
 * An abstraction for a time stamp and/or sequence number.
 */
class YARP_os_API Stamp : public Portable
{
private:
    NetInt32 sequenceNumber;
    NetFloat64 timeStamp;

public:
    /**
     * Construct an invalid Stamp.
     */
    explicit Stamp();

    /**
     * Construct a Stamp with a given sequence number and time.
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
    int getCount() const;

    /**
     * Get the time stamp.
     *
     * @return the time stamp
     */
    double getTime() const;

    /**
     * Check if this Stamp is valid.
     *
     * @return true if this is a valid Stamp
     */
    bool isValid() const;

    /**
     * Get the maximum sequence number, after which an incrementing sequence
     * should return to zero.
     *
     * @return the maximum sequence number.
     */
    int getMaxCount() const;

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
    bool read(ConnectionReader& connection) override;

    // Documented in Portable
    bool write(ConnectionWriter& connection) const override;
};


#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
/**
 * A base class for objects with time stamps and/or sequence numbers.
 * @deprecated since YARP 3.0.0
 */
class YARP_os_DEPRECATED_API Stamped
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
#endif // YARP_NO_DEPRECATED

} // namespace os
} // namespace yarp

#endif // YARP_OS_STAMP_H
