// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NULLCONNECTIONREADER_
#define _YARP2_NULLCONNECTIONREADER_

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
    virtual bool expectBlock(const char *data, size_t len) { return false; }
    virtual ConstString expectText(int terminatingChar = '\n') { return ""; }
    virtual int expectInt() { return 0; }
    virtual bool pushInt(int x) { return false; }
    virtual double expectDouble() { return 0.0; }
    virtual bool isTextMode() { return false; }
    virtual bool convertTextMode() { return false; }
    virtual size_t getSize() { return 0; }
    virtual ConnectionWriter *getWriter() { return 0/*NULL*/; }
    virtual Bytes readEnvelope() { return Bytes(0,0); }
    virtual Portable *getReference() { return 0/*NULL*/; }
    virtual Contact getRemoteContact() { return Contact(); }
    virtual Contact getLocalContact() { return Contact(); }
    virtual bool isValid() { return false; }
    virtual bool isActive() { return false; }
    virtual bool isError() { return true; }
    virtual void requestDrop() {}
    virtual Searchable& getConnectionModifiers() { return blank; }
};


#endif

