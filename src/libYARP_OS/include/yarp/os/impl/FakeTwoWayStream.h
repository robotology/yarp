/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

    void addInputText(const ConstString& str)
    {
        in.add(str);
    }

    ConstString getOutputText()
    {
        return out.toString();
    }

    ConstString getInputText()
    {
        return in.toString();
    }

    virtual bool isOk() override
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
