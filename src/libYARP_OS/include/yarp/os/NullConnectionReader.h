/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_NULLCONNECTIONREADER_H
#define YARP_OS_NULLCONNECTIONREADER_H

#include <yarp/os/api.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Value.h>

namespace yarp {
    namespace os {
        class NullConnectionReader;
    }
}

/**
 *
 * Dummy ConnectionReader that has no data.
 *
 */
class YARP_OS_API yarp::os::NullConnectionReader : public ConnectionReader {
private:
    Value blank;
public:
    virtual bool expectBlock(const char *data, size_t len) YARP_OVERRIDE;
    virtual ConstString expectText(int terminatingChar = '\n') YARP_OVERRIDE;
    virtual int expectInt() YARP_OVERRIDE;
    virtual bool pushInt(int x) YARP_OVERRIDE;
    virtual double expectDouble() YARP_OVERRIDE;
    virtual bool isTextMode() YARP_OVERRIDE;
    virtual bool convertTextMode() YARP_OVERRIDE;
    virtual size_t getSize() YARP_OVERRIDE;
    virtual ConnectionWriter *getWriter() YARP_OVERRIDE;
    virtual Bytes readEnvelope() YARP_OVERRIDE;
    virtual Portable *getReference() YARP_OVERRIDE;
    virtual Contact getRemoteContact() YARP_OVERRIDE;
    virtual Contact getLocalContact() YARP_OVERRIDE;
    virtual bool isValid() YARP_OVERRIDE;
    virtual bool isActive() YARP_OVERRIDE;
    virtual bool isError() YARP_OVERRIDE;
    virtual void requestDrop() YARP_OVERRIDE;
    virtual Searchable& getConnectionModifiers() YARP_OVERRIDE;
};


#endif // YARP_OS_NULLCONNECTIONREADER_H
