/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/InputStream.h>

#include <yarp/os/Bytes.h>
#include <yarp/os/ManagedBytes.h>

using namespace yarp::os;

InputStream::InputStream() = default;
InputStream::~InputStream() = default;

void InputStream::check()
{
}

int InputStream::read()
{
    unsigned char result;
    yarp::os::Bytes bytes(reinterpret_cast<char*>(&result), 1);
    yarp::conf::ssize_t ct = read(bytes);
    if (ct < 1) {
        return -1;
    }
    return static_cast<int>(result);
}

yarp::conf::ssize_t InputStream::read(Bytes& b, size_t offset, yarp::conf::ssize_t len)
{
    yarp::os::Bytes bytes(b.get() + offset, len);
    return read(bytes);
}

yarp::conf::ssize_t InputStream::partialRead(yarp::os::Bytes& b)
{
    return read(b);
}

void InputStream::interrupt()
{
}

bool InputStream::setReadTimeout(double timeout)
{
    YARP_UNUSED(timeout);
    return false;
}

// slow implementation - only relevant for textmode operation

std::string InputStream::readLine(const char terminal, bool* success)
{
    std::string buf;
    bool done = false;
    int esc = 0;
    if (success != nullptr) {
        *success = true;
    }
    while (!done) {
        int v = read();
        if (v < 0) {
            if (success != nullptr) {
                *success = false;
            }
            return {};
        }
        char ch = (char)v;
        if (v == '\\') {
            esc++;
        }
        if (v != 0 && v != '\r' && v != '\n') {
            if (v != '\\' || esc >= 2) {
                while (esc != 0) {
                    buf += '\\';
                    esc--;
                }
            }
            if (v != '\\') {
                buf += ch;
            }
        }
        if (ch == terminal) {
            if (esc == 0) {
                done = true;
            } else {
                esc = 0;
            }
        }
    }
    return buf;
}

yarp::conf::ssize_t InputStream::readFull(Bytes& b)
{
    yarp::conf::ssize_t off = 0;
    yarp::conf::ssize_t fullLen = b.length();
    yarp::conf::ssize_t remLen = fullLen;
    yarp::conf::ssize_t result = 1;
    while (result > 0 && remLen > 0) {
        result = read(b, off, remLen);
        if (result > 0) {
            remLen -= result;
            off += result;
        }
    }
    return (result <= 0) ? -1 : fullLen;
}

yarp::conf::ssize_t InputStream::readDiscard(size_t len)
{
    if (len < 100) {
        char buf[100];
        Bytes b(buf, len);
        return readFull(b);
    }
    ManagedBytes b(len);
    return readFull(b.bytes());
}

bool InputStream::setReadEnvelopeCallback(readEnvelopeCallbackType callback, void* data)
{
    YARP_UNUSED(callback);
    YARP_UNUSED(data);
    return false;
}
