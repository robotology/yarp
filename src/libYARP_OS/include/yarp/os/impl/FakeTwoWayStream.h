/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_FAKETWOWAYSTREAM_H
#define YARP_OS_IMPL_FAKETWOWAYSTREAM_H

#include <yarp/os/StringInputStream.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/TwoWayStream.h>

namespace yarp {
namespace os {
namespace impl {

/**
 * A dummy two way stream for testing purposes.
 */
class FakeTwoWayStream :
        public TwoWayStream
{
public:
    FakeTwoWayStream(StringInputStream* target = nullptr) :
            TwoWayStream()
    {
        this->out.owner = this;
        this->target = target;
    }

    void setTarget(StringInputStream& target)
    {
        this->target = &target;
    }

    InputStream& getInputStream() override
    {
        return in;
    }

    virtual StringInputStream& getStringInputStream()
    {
        return in;
    }

    OutputStream& getOutputStream() override
    {
        return out;
    }

    const Contact& getLocalAddress() const override
    {
        return local;
    }

    const Contact& getRemoteAddress() const override
    {
        return remote;
    }

    void close() override
    {
        in.close();
        out.close();
    }

    virtual void apply(const Bytes& b)
    {
        if (target != nullptr) {
            target->add(b);
        }
    }

    void addInputText(const std::string& str)
    {
        in.add(str);
    }

    std::string getOutputText() const
    {
        return out.toString();
    }

    std::string getInputText() const
    {
        return in.toString();
    }

    bool isOk() const override
    {
        return true;
    }

    void reset() override
    {
    }

    void beginPacket() override
    {
    }

    void endPacket() override
    {
    }

private:
    class ActiveStringOutputStream : public StringOutputStream
    {
    public:
        ActiveStringOutputStream() :
                owner(nullptr)
        {
        }

        using yarp::os::OutputStream::write;
        void write(const Bytes& b) override
        {
            StringOutputStream::write(b);
            if (owner) {
                owner->apply(b);
            }
        }

        FakeTwoWayStream* owner;
    };

    StringInputStream in;
    ActiveStringOutputStream out;
    Contact local;
    Contact remote;
    StringInputStream* target;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_FAKETWOWAYSTREAM_H
