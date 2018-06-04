/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    virtual std::string expectText(int terminatingChar = '\n') override;
    virtual std::int8_t expectInt8() override;
    virtual std::int16_t expectInt16() override;
    virtual std::int32_t expectInt32() override;
    virtual std::int64_t expectInt64() override;
    virtual yarp::conf::float32_t expectFloat32() override;
    virtual yarp::conf::float64_t expectFloat64() override;
    virtual bool pushInt(int x) override;
    virtual bool isTextMode() const override;
    virtual bool convertTextMode() override;
    virtual size_t getSize() override;
    virtual ConnectionWriter *getWriter() override;
    virtual Bytes readEnvelope() override;
    virtual Portable *getReference() override;
    virtual Contact getRemoteContact() override;
    virtual Contact getLocalContact() override;
    virtual bool isValid() const override;
    virtual bool isActive() const override;
    virtual bool isError() override;
    virtual void requestDrop() override;
    virtual Searchable& getConnectionModifiers() override;
};


#endif // YARP_OS_NULLCONNECTIONREADER_H
