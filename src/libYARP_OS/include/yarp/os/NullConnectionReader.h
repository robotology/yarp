// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_NULLCONNECTIONREADER
#define YARP2_NULLCONNECTIONREADER

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
    virtual bool expectBlock(const char *data, size_t len);
    virtual ConstString expectText(int terminatingChar = '\n');
    virtual int expectInt();
    virtual bool pushInt(int x);
    virtual double expectDouble();
    virtual bool isTextMode();
    virtual bool convertTextMode();
    virtual size_t getSize();
    virtual ConnectionWriter *getWriter();
    virtual Bytes readEnvelope();
    virtual Portable *getReference();
    virtual Contact getRemoteContact();
    virtual Contact getLocalContact();
    virtual bool isValid();
    virtual bool isActive();
    virtual bool isError();
    virtual void requestDrop();
    virtual Searchable& getConnectionModifiers();
};


#endif

