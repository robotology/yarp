/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PORTWRITERBUFFERBASE_H
#define YARP_OS_PORTWRITERBUFFERBASE_H

#include <yarp/os/PortWriter.h>

namespace yarp {
namespace os {

class Port;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class PortWriterBufferManager
{
public:
    virtual ~PortWriterBufferManager();

    virtual void onCompletion(void* tracker) = 0;
};

class PortWriterWrapper : public PortWriter
{
public:
    virtual PortWriter* getInternal() = 0;
};

#endif // DOXYGEN_SHOULD_SKIP_THIS


class YARP_os_API PortWriterBufferBase
{
public:
    PortWriterBufferBase();

    virtual ~PortWriterBufferBase();

    virtual PortWriterWrapper *create(PortWriterBufferManager& man, void *tracker) = 0;

    const void* getContent() const;

    bool releaseContent();

    int getCount();

    void attach(Port& port);

    void detach();

    void write(bool strict);

    void waitForWrite();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PORTWRITERBUFFERBASE_H
