/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_FAKETWOWAYSTREAM
#define YARP2_FAKETWOWAYSTREAM

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
class yarp::os::impl::FakeTwoWayStream : public TwoWayStream {
public:
    FakeTwoWayStream(StringInputStream *target = NULL) :
            TwoWayStream() {
        this->out.owner = this;
        this->target = target;
    }

    void setTarget(StringInputStream& target) {
        this->target = &target;
    }

    virtual InputStream& getInputStream() {
        return in;
    }

    virtual StringInputStream& getStringInputStream() {
        return in;
    }

    virtual OutputStream& getOutputStream() {
        return out;
    }

    virtual const Contact& getLocalAddress() {
        return local;
    }

    virtual const Contact& getRemoteAddress() {
        return remote;
    }

    virtual void close() {
        in.close();
        out.close();
    }

    virtual void apply(const Bytes& b) {
        if (target!=NULL) {
            target->add(b);
        }
    }

    void addInputText(const ConstString& str) {
        in.add(str);
    }

    ConstString getOutputText() {
        return out.toString();
    }

    ConstString getInputText() {
        return in.toString();
    }

    virtual bool isOk() {
        return true;
    }

    virtual void reset() {
    }

    virtual void beginPacket() { }

    virtual void endPacket() { }

private:

    class ActiveStringOutputStream : public StringOutputStream {
    public:
        ActiveStringOutputStream() : owner(NULL) {
        }

        using yarp::os::OutputStream::write;
        virtual void write(const Bytes& b) {
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

#endif
