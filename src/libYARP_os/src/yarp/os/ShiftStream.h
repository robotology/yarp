/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_SHIFTSTREAM_H
#define YARP_OS_SHIFTSTREAM_H

#include <yarp/os/TwoWayStream.h>

namespace yarp {
namespace os {

/**
 * A container for a stream, allowing the stream implementation to
 * be replaced when needed.  This is important in YARP since
 * connections "bootstrap" from an initial stream type to
 * an optimized stream type with user-preferred properties and
 * trade-offs.
 */
class YARP_os_API ShiftStream : public TwoWayStream
{
public:
    /**
     * Constructor.
     */
    ShiftStream();

    /**
     * Destructor.
     */
    virtual ~ShiftStream();

    /**
     * Perform maintenance actions, if needed.
     */
    virtual void check() const;

    InputStream& getInputStream() override;
    OutputStream& getOutputStream() override;

    const Contact& getLocalAddress() const override;
    const Contact& getRemoteAddress() const override;

    void close() override;

    /**
     * Wrap the supplied stream.  If a stream is already wrapped,
     * it will be closed and destroyed.
     * @param stream the stream to wrap.
     */
    virtual void takeStream(TwoWayStream* stream);

    /**
     * Removes the wrapped stream and returns it.
     * The caller will be responsible for closing the stream.
     * @return the wrapped stream (which after this call will be the
     * caller's responsibility).
     */
    virtual TwoWayStream* giveStream();

    /**
     * @return the wrapped stream (which after this call will remain
     * this container's responsibility - compare with giveStream).
     */
    virtual TwoWayStream* getStream() const;

    /**
     * @return true if there is no wrapped stream.
     */
    virtual bool isEmpty() const;

    bool isOk() const override;

    void reset() override;

    void beginPacket() override;

    void endPacket() override;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_SHIFTSTREAM_H
