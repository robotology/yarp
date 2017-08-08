/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
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
    virtual void appendBlock(const char *data, size_t len) YARP_OVERRIDE;
    virtual void appendInt(int data) YARP_OVERRIDE;
    virtual void appendInt64(const YARP_INT64& data) YARP_OVERRIDE;
    virtual void appendDouble(double data) YARP_OVERRIDE;
    virtual void appendString(const char *str, int terminate = '\n') YARP_OVERRIDE;
    virtual void appendExternalBlock(const char *data, size_t len) YARP_OVERRIDE;
    virtual bool isTextMode() YARP_OVERRIDE;
    virtual bool isBareMode() YARP_OVERRIDE;
    virtual void declareSizes(int argc, int *argv) YARP_OVERRIDE;
    virtual void setReplyHandler(PortReader& reader) YARP_OVERRIDE;
    virtual void setReference(Portable *obj) YARP_OVERRIDE;
    virtual bool convertTextMode() YARP_OVERRIDE;
    virtual bool isValid() YARP_OVERRIDE;
    virtual bool isActive() YARP_OVERRIDE;
    virtual bool isError() YARP_OVERRIDE;
    virtual void requestDrop() YARP_OVERRIDE;
    virtual bool isNull() const YARP_OVERRIDE;
    virtual SizedWriter *getBuffer() YARP_OVERRIDE;
};

#endif // YARP_OS_NULLCONNECTIONWRITER_H
