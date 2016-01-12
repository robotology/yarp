/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_NULLCONNECTIONWRITER_H
#define YARP_OS_NULLCONNECTIONWRITER_H

#include <yarp/os/api.h>
#include <yarp/os/ConnectionWriter.h>

namespace yarp {
    namespace os {
        class NullConnectionWriter;
    }
}

/**
 *
 * A dummy ConnectionWriter that consumes data without effect.
 *
 */
class YARP_OS_API yarp::os::NullConnectionWriter : public ConnectionWriter {
public:
    virtual void appendBlock(const char *data, size_t len);
    virtual void appendInt(int data);
    virtual void appendInt64(const YARP_INT64& data);
    virtual void appendDouble(double data);
    virtual void appendString(const char *str, int terminate = '\n');
    virtual void appendExternalBlock(const char *data, size_t len);
    virtual bool isTextMode();
    virtual bool isBareMode();
    virtual void declareSizes(int argc, int *argv);
    virtual void setReplyHandler(PortReader& reader);
    virtual void setReference(Portable *obj);
    virtual bool convertTextMode();
    virtual bool isValid();
    virtual bool isActive();
    virtual bool isError();
    virtual void requestDrop();
    virtual bool isNull() const;
    virtual SizedWriter *getBuffer();
};

#endif // YARP_OS_NULLCONNECTIONWRITER_H
