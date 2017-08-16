/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_PORTREADERBUFFERBASE_H
#define YARP_OS_PORTREADERBUFFERBASE_H

#include <yarp/os/PortReader.h>


// Defined in this file:
namespace yarp { namespace os { class PortReaderBufferBase; }}

// Other forward declarations:
namespace yarp { namespace os { class PortReaderBufferBaseCreator; }}


namespace yarp {
namespace os {

class YARP_OS_API PortReaderBufferBase : public yarp::os::PortReader
{
public:
    PortReaderBufferBase(unsigned int maxBuffer);
    virtual ~PortReaderBufferBase();

    void setCreator(PortReaderBufferBaseCreator* creator);

    void setReplier(yarp::os::PortReader& reader);

    void setPrune(bool flag = true);

    void setAllowReuse(bool flag = true);

    void setTargetPeriod(double period);

    yarp::os::ConstString getName() const;

    unsigned int getMaxBuffer();

    bool isClosed();

    void clear();

    virtual yarp::os::PortReader* create();

    void release(yarp::os::PortReader* completed);

    int check();

    virtual bool read(yarp::os::ConnectionReader& connection) override;

    yarp::os::PortReader* readBase(bool& missed, bool cleanup);

    void interrupt();

    void attachBase(yarp::os::Port& port);

    // direct writer-buffer to reader-buffer pointer sharing methods

    virtual bool acceptObjectBase(yarp::os::PortReader* obj,
                                  yarp::os::PortWriter* wrapper);

    virtual bool forgetObjectBase(yarp::os::PortReader* obj,
                                  yarp::os::PortWriter* wrapper);

    virtual bool getEnvelope(PortReader& envelope);

    // user takes control of the current read object
    void* acquire();

    // user gives back an object
    void release(void* key);

protected:
    void init();

    PortReaderBufferBaseCreator* creator;
    unsigned int maxBuffer;
    bool prune;
    bool allowReuse;
    void* implementation;
    yarp::os::PortReader* replier;
    double period;
    double last_recv;
};


} // namespace os
} // namespace yarp

#endif // YARP_OS_PORTREADERBUFFERBASE_H
