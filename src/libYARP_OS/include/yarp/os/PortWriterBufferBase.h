/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PORTWRITERBUFFERBASE_H
#define YARP_OS_PORTWRITERBUFFERBASE_H

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


class YARP_OS_API PortWriterBufferBase
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
