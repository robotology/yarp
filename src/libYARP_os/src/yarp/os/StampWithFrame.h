/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_STAMPWITHFRAME_H
#define YARP_OS_STAMPWITHFRAME_H

#include <yarp/os/Portable.h>
#include <yarp/conf/numeric.h>

#include <string>

namespace yarp::os {

/**
 * An abstraction for a sequence number, time stamp and/or frame id.
 *
 * This class is compatible with the Stamp class
 */
class YARP_os_API StampWithFrame :
        public Portable
{
public:
    using count_t = std::uint32_t;

    /**
     * Maximum sequence number, after which an incrementing sequence
     * should return to zero.
     */
    static constexpr count_t npos = static_cast<count_t>(-1);

    /**
     * Construct an invalid StampWithFrame.
     */
    explicit StampWithFrame();

    /**
     * Construct a StampWithFrame with a given sequence number and time.
     *
     * @param count the sequence number.
     * @param time the time stamp (in seconds, relative to an arbitrary zero time).
     * @param frameId the frame id.
     */
    StampWithFrame(count_t count, yarp::conf::float64_t time, std::string frameId = {});

    /**
     * @brief Copy constructor.
     *
     * @param rhs the StampWithFrame to copy
     */
    StampWithFrame(const StampWithFrame& rhs);

    /**
     * @brief Move constructor.
     *
     * @param rhs the StampWithFrame to be moved
     */
    StampWithFrame(StampWithFrame&& rhs) noexcept;

    /**
     * @brief Destructor.
     */
    ~StampWithFrame() override;

    /**
     * Copy assignment operator.
     *
     * @param rhs the StampWithFrame to copy
     * @return this object
     */
    StampWithFrame& operator=(const StampWithFrame& rhs);

    /**
     * @brief Move assignment operator.
     *
     * @param rhs the StampWithFrame to be moved
     * @return this object
     */
    StampWithFrame& operator=(StampWithFrame&& rhs) noexcept;

    /**
     * Get the sequence number.
     *
     * @return the sequence number.
     */
    count_t count() const;

    /**
     * Get the time stamp.
     *
     * @return the time stamp.
     */
    yarp::conf::float64_t timeStamp() const;

    /**
     * Get the frame id.
     *
     * @return the frame id.
     */
    std::string frameId() const;

    /**
     * Check if this StampWithFrame is valid.
     *
     * @return true if this is a valid StampWithFrame
     */
    bool isValid() const;

    /**
     * Set the timestamp to the current time, and increment the sequence number
     * (wrapping to 0 if the sequence number exceeds npos)
     */
    void update();

    /**
     * Set the timestamp to a given time, and increments the sequence number
     * (wrapping to 0 if the sequence exceeds npos)
     */
    void update(yarp::conf::float64_t time);

    /**
     * Set the frame id for this StampWithFrame
     *
     * @param frameId the new frame id for this StampWithFrame
     */
    void setFrameId(std::string frameId);


    // Documented in Portable
    bool read(ConnectionReader& connection) override;

    // Documented in Portable
    bool write(ConnectionWriter& connection) const override;

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};


} // namespace yarp::os

#endif // YARP_OS_STAMPWITHFRAME_H
