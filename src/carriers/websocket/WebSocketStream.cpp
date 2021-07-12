/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "WebSocketStream.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/NetType.h>
#include <yarp/os/NetInt64.h>

using namespace yarp::os;

YARP_LOG_COMPONENT(WEBSOCK_STREAM,
                   "yarp.stream.websocket",
                   yarp::os::Log::TraceType,
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)


WebSocketStream::WebSocketStream(yarp::os::TwoWayStream* delegate) :
        delegate(delegate)
{
}


WebSocketStream::~WebSocketStream()
{
    yCTrace(WEBSOCK_STREAM);
}


InputStream& WebSocketStream::getInputStream()
{
    yCTrace(WEBSOCK_STREAM);
    return *this;
}


OutputStream& WebSocketStream::getOutputStream()
{
    yCTrace(WEBSOCK_STREAM);
    return *this;
}


const Contact& WebSocketStream::getLocalAddress() const
{
    yCTrace(WEBSOCK_STREAM);
    return local;
}


const Contact& WebSocketStream::getRemoteAddress() const
{
    yCTrace(WEBSOCK_STREAM);
    return remote;
}


void WebSocketStream::interrupt()
{
    yCTrace(WEBSOCK_STREAM);
    close();
}


void WebSocketStream::close()
{
    yCTrace(WEBSOCK_STREAM);
    yarp::os::ManagedBytes frame;
    yarp::os::Bytes b;
    makeFrame(CLOSING_OPCODE, b, frame);
    yarp::os::Bytes toWrite(frame.get(), frame.length());
    return delegate->getOutputStream().write(toWrite);
}


yarp::conf::ssize_t WebSocketStream::read(Bytes& b)
{
    yCTrace(WEBSOCK_STREAM);
    size_t bytesRead = 0;
    while (bytesRead < b.length()) {
        // the buffer is empty
        if (buffer.length() == 0 || buffer.length() == currentHead) {
            WebSocketFrameType frameType;
            do {
                frameType = getFrame(buffer);
            } while (frameType != BINARY_FRAME && frameType != TEXT_FRAME);
            currentHead = 0;
        }
        // get the remaining bytes to read from the buffer
        size_t remainedFromBuffer = buffer.length() - currentHead;
        // if the buffer is enough then the size of the bytes is given,
        // otherwise the remaining bytes from the buffer is copied and is read again from the buffer
        size_t toAdd = (remainedFromBuffer >= b.length()) ? b.length() : remainedFromBuffer;
        memcpy(b.get(), buffer.get() + currentHead, toAdd);
        currentHead += toAdd;
        bytesRead += toAdd;
    }
    return static_cast<yarp::conf::ssize_t>(b.length());
}


//TODO FIXME STE can be selected the type of frame?
void WebSocketStream::write(const yarp::os::Bytes& b)
{
    yCTrace(WEBSOCK_STREAM);
    yarp::os::ManagedBytes frame;
    makeFrame(BINARY_FRAME, b, frame);
    yarp::os::Bytes toWrite(frame.get(), frame.length());
    return delegate->getOutputStream().write(toWrite);
}


bool WebSocketStream::isOk() const
{
    return true;
}


void WebSocketStream::reset()
{
}


void WebSocketStream::beginPacket()
{
}


void WebSocketStream::endPacket()
{
}


WebSocketFrameType WebSocketStream::getFrame(yarp::os::ManagedBytes& payload)
{
    yCTrace(WEBSOCK_STREAM);
    yarp::os::ManagedBytes header;
    yarp::os::ManagedBytes mask_bytes;
    header.allocate(2);
    delegate->getInputStream().read(header.bytes());
    unsigned char msg_opcode = header.get()[0] & 0x0F;
    unsigned char msg_masked = (header.get()[1] >> 7) & 0x01;
    if(msg_opcode == 0x9)
    {
        return PING_FRAME;
    }
    if(msg_opcode == 0xA) {
        return PONG_FRAME;
    }
    if (msg_opcode == CLOSING_OPCODE)
    {
        // this returns a quit command to the caller through the yarp protocol,
        // so the caller can quit the connection in a reasonable manner
        unsigned char toreturn[]  = "\0\0\0\0~\0\0\1q";
        payload.allocate(10);
        memcpy(payload.get(), toreturn, 10);
        return CLOSING_OPCODE;
    }
    yarp::os::NetInt64 payload_length = 0;
    yarp::os::NetInt32 length_field = header.get()[1] & (0x7F);

    if (length_field <= 125) {
        payload_length = length_field;
    } else {
        yarp::os::ManagedBytes additionalLength;
        int length_to_add = 0;
        if (length_field == 126) { //msglen is 16bit!
            length_to_add = 2;
        } else if (length_field == 127) { //msglen is 64bit!
            length_to_add = 8;
        }
        additionalLength.allocate(length_to_add);
        delegate->getInputStream().read(additionalLength.bytes());
        for (int i =0; i < length_to_add; i++) {
            memcpy(reinterpret_cast<unsigned char *>(&payload_length) + i,
                   reinterpret_cast<unsigned char *>(&additionalLength.get()[(length_to_add-1) - i]),
                   1);
        }
    }

    if (msg_masked) {
        // get the mask
        mask_bytes.allocate(4);
        delegate->getInputStream().read(mask_bytes.bytes());
    }

    payload.allocate(payload_length);
    delegate->getInputStream().read(payload.bytes());
    if (msg_masked) {
        // unmask data:
        for (int i = 0; i < payload_length; i++) {
            payload.get()[i] = payload.get()[i] ^ mask_bytes.get()[i % 4];
        }
    }

    if(msg_opcode == 0x0 || msg_opcode == 0x1)
    {
        return TEXT_FRAME;
    }
    if(msg_opcode == 0x2)
    {
        return BINARY_FRAME;
    }
    return ERROR_FRAME;
}


// TODO FIXME STE need to manage if frame is not passed
void WebSocketStream::makeFrame(WebSocketFrameType frame_type,
                                const yarp::os::Bytes& payload,
                                yarp::os::ManagedBytes& frame)
{
    yCTrace(WEBSOCK_STREAM);
    int pos = 0;
    size_t size = payload.length();

    if (size <= 125) {
        frame.allocate(2 + size);
    } else if (size <= 65535) {
        frame.allocate(4 + size);
    } else { // >2^16-1 (65535)
        frame.allocate(10 + size);
    }

    frame.get()[pos] = static_cast<char>(frame_type); // text frame
    pos++;


    if (size <= 125) {
        // this is a 7 bit size (the first bit is the mask
        // that must be set to 0)
        frame.get()[pos++] = size;
    } else if (size <= 65535) {
        frame.get()[pos++] = 126;                //16 bit length follows
        frame.get()[pos++] = (size >> 8) & 0xFF; // leftmost first
        frame.get()[pos++] = size & 0xFF;
    } else {                      // >2^16-1 (65535)
        frame.get()[pos++] = 127; //64 bit length follows
        // write the actual 64bit msg_length in the next 4 bytes
        for (int i = 7; i >= 0; i--) {
            frame.get()[pos++] = ((size >> 8 * i) & 0xFF);
        }
    }
    memcpy(reinterpret_cast<void*>(frame.get() + pos), payload.get(), size);
}
