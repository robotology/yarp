/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_SIZEDWRITER_H
#define YARP_OS_SIZEDWRITER_H

#include <yarp/conf/numeric.h>

#include <yarp/os/api.h>

#include <yarp/os/PortWriter.h>

namespace yarp {
namespace os {

class ConnectionWriter;
class OutputStream;
class PortReader;
class Portable;

/**
 * Minimal requirements for an efficient Writer.
 * Some protocols require knowing the size of a message up front.
 * In general, that requires generating the message before sending
 * it, but a user could do something more clever. The
 * SizedWriter class is referenced by the library instead of
 * BufferedConnectionWriter specifically to leave that possibility open.
 */
class YARP_os_API SizedWriter : public PortWriter
{
public:
    virtual ~SizedWriter();

    virtual size_t length() const = 0;

    virtual size_t headerLength() const = 0;

    virtual size_t length(size_t index) const = 0;

    virtual const char* data(size_t index) const = 0;

    virtual PortReader* getReplyHandler() = 0;

    virtual Portable* getReference() = 0;

    virtual void write(OutputStream& os);

    bool write(ConnectionWriter& connection) const override;

    virtual bool dropRequested() = 0;

    /**
     * Call when writing is about to begin.
     */
    virtual void startWrite() const = 0;

    /**
     * Call when all writing is finished.
     */
    virtual void stopWrite() const = 0;

    virtual void clear();
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_SIZEDWRITER_H
