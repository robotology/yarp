/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_FAKETWOWAYSTREAM_H
#define YARP_OS_IMPL_FAKETWOWAYSTREAM_H

#include <yarp/os/TwoWayStream.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/StringOutputStream.h>

namespace yarp {
    namespace os {
        namespace impl {
            class FakeTwoWayStream;
        }
    }
}


/**
 * A dummy two way stream for testing purposes.
 */
class yarp::os::impl::FakeTwoWayStream : public TwoWayStream
{
public:
    FakeTwoWayStream(StringInputStream *target = nullptr) :
            TwoWayStream()
    {
        this->out.owner = this;
        this->target = target;
    }

    void setTarget(StringInputStream& target)
    {
        this->target = &target;
    }

    virtual InputStream& getInputStream() override
    {
        return in;
    }

    virtual StringInputStream& getStringInputStream()
    {
        return in;
    }

    virtual OutputStream& getOutputStream() override
    {
        return out;
    }

    virtual const Contact& getLocalAddress() override
    {
        return local;
    }

    virtual const Contact& getRemoteAddress() override
    {
        return remote;
    }

    virtual void close() override
    {
        in.close();
        out.close();
    }

    virtual void apply(const Bytes& b)
    {
        if (target!=nullptr) {
            target->add(b);
        }
    }

    void addInputText(const std::string& str)
    {
        in.add(str);
    }

    std::string getOutputText()
    {
        return out.toString();
    }

    std::string getInputText()
    {
        return in.toString();
    }

    virtual bool isOk() const override
    {
        return true;
    }

    virtual void reset() override
    {
    }

    virtual void beginPacket() override { }

    virtual void endPacket() override { }

private:

    class ActiveStringOutputStream : public StringOutputStream
    {
    public:
        ActiveStringOutputStream() :
                owner(nullptr)
        {
        }

        using yarp::os::OutputStream::write;
        virtual void write(const Bytes& b) override
        {
            StringOutputStream::write(b);
            if (owner) {
                owner->apply(b);
            }
        }

        FakeTwoWayStream *owner;
    };

    StringInputStream in;
    ActiveStringOutputStream out;
    Contact local;
    Contact remote;
    StringInputStream *target;
};

#endif // YARP_OS_IMPL_FAKETWOWAYSTREAM_H
