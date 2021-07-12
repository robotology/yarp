/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_WEBSOCKSTREAM_H
#define YARP_WEBSOCKSTREAM_H

#include "WebSocket/WebSocket.h"

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/TwoWayStream.h>


class WebSocketStream :
        public yarp::os::TwoWayStream,
        public yarp::os::InputStream,
        public yarp::os::OutputStream
{
public:
    WebSocketStream(TwoWayStream* delegate);

    ~WebSocketStream() override;

    InputStream& getInputStream() override;
    OutputStream& getOutputStream() override;

    const yarp::os::Contact& getLocalAddress() const override;
    const yarp::os::Contact& getRemoteAddress() const override;

    void interrupt() override;

    void close() override;

    /**
     * this override the read call to the stream,
     * it automatically filters the various headers and
     * not useful frames (like ping or pong).
     * @param bytesToRead the bytes that must be read from the socket
     * @return the number of bytes actually read
     */
    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& bytesToRead) override;

    /**
     * this override the write call to the stream,
     * it automatically makes the header for the frame
     * and sends it over websocket.
     * It makes a binary frame.
     * @param bytesToRead the bytes that must be written into the socket
     */
    using yarp::os::OutputStream::write;
    void write(const yarp::os::Bytes& bytesToWrite) override;

    bool isOk() const override;

    void reset() override;

    void beginPacket() override;
    void endPacket() override;

private:
    yarp::os::TwoWayStream* delegate;
    yarp::os::Contact local, remote;
    yarp::os::ManagedBytes buffer;
    size_t currentHead {0};

    /**
     * It reads from the delegate stream a websocket frame.
     * It automatically remove the header and unmasks the frame
     * @param payload the payload of the frame received
     * @return the type of the frame received
     */
    WebSocketFrameType getFrame(yarp::os::ManagedBytes& payload);

    /**
     * It creates a frame in the websocket format
     * @param frame_type a WebSocketFrameType for the frame that wants to be created
     *                   usually BINARY_FRAME or TEXT_FRAME
     * @param payload the payload of the frame
     * @param frame a pointer to the bytes where the frame must be creeated
     */
    static void makeFrame(WebSocketFrameType frame_type,
                   const yarp::os::Bytes& payload,
                   yarp::os::ManagedBytes& frame);
};

#endif // YARP_WEBSOCKSTREAM_H
