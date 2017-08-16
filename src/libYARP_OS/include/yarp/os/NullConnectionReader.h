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
    virtual bool expectBlock(const char *data, size_t len) override;
    virtual ConstString expectText(int terminatingChar = '\n') override;
    virtual int expectInt() override;
    virtual bool pushInt(int x) override;
    virtual double expectDouble() override;
    virtual bool isTextMode() override;
    virtual bool convertTextMode() override;
    virtual size_t getSize() override;
    virtual ConnectionWriter *getWriter() override;
    virtual Bytes readEnvelope() override;
    virtual Portable *getReference() override;
    virtual Contact getRemoteContact() override;
    virtual Contact getLocalContact() override;
    virtual bool isValid() override;
    virtual bool isActive() override;
    virtual bool isError() override;
    virtual void requestDrop() override;
    virtual Searchable& getConnectionModifiers() override;
};


#endif // YARP_OS_NULLCONNECTIONREADER_H
