/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    virtual void appendBlock(const char *data, size_t len) override;
    virtual void appendInt(int data) override;
    virtual void appendInt64(const YARP_INT64& data) override;
    virtual void appendDouble(double data) override;
    virtual void appendString(const char *str, int terminate = '\n') override;
    virtual void appendExternalBlock(const char *data, size_t len) override;
    virtual bool isTextMode() override;
    virtual bool isBareMode() override;
    virtual void declareSizes(int argc, int *argv) override;
    virtual void setReplyHandler(PortReader& reader) override;
    virtual void setReference(Portable *obj) override;
    virtual bool convertTextMode() override;
    virtual bool isValid() override;
    virtual bool isActive() override;
    virtual bool isError() override;
    virtual void requestDrop() override;
    virtual bool isNull() const override;
    virtual SizedWriter *getBuffer() override;
};

#endif // YARP_OS_NULLCONNECTIONWRITER_H
