/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PORTREADERBUFFERBASE_H
#define YARP_OS_PORTREADERBUFFERBASE_H

#include <yarp/os/PortReader.h>

#include <string>

namespace yarp {
namespace os {

class Port;
class PortReaderBufferBaseCreator;
class PortWriter;

class YARP_os_API PortReaderBufferBase :
        public yarp::os::PortReader
{
public:
    PortReaderBufferBase(unsigned int maxBuffer);
    virtual ~PortReaderBufferBase();

    void setCreator(PortReaderBufferBaseCreator* creator);

    void setReplier(yarp::os::PortReader& reader);

    void setPrune(bool flag = true);

    void setTargetPeriod(double period);

    std::string getName() const;

    unsigned int getMaxBuffer();

    bool isClosed();

    void clear();

    virtual yarp::os::PortReader* create();

    int check();

    bool read(yarp::os::ConnectionReader& connection) override;

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};


} // namespace os
} // namespace yarp

#endif // YARP_OS_PORTREADERBUFFERBASE_H
