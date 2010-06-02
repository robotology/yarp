// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef MJPEGSTREAM_INC
#define MJPEGSTREAM_INC

#include <yarp/os/impl/InputStream.h>
#include <yarp/os/impl/OutputStream.h>
#include <yarp/os/impl/TwoWayStream.h>
#include <yarp/os/impl/StringInputStream.h>
#include <yarp/os/impl/StringOutputStream.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/PortCommand.h>

namespace yarp {
    namespace os {
        namespace impl {
            class MjpegStream;
        }
    }
}

class yarp::os::impl::MjpegStream : public TwoWayStream, 
                                    public InputStream,
                                    public OutputStream
{
private:
    TwoWayStream *delegate;
    StringInputStream sis;
    StringOutputStream sos;
    bool sender;
    bool firstRound;
public:
    MjpegStream(TwoWayStream *delegate, bool sender) : sender(sender) {
        this->delegate = delegate;
        firstRound = true;
        if (sender) {
            // add a stream ...
            BufferedConnectionWriter writer(false);
            PortCommand pc('r',"");
            pc.write(writer);
            sis.add(pc.toString());
        }
    }

    virtual ~MjpegStream() {
        if (delegate!=NULL) {
            delete delegate;
            delegate = NULL;
        }
    }

    virtual InputStream& getInputStream() { return *this; }
    virtual OutputStream& getOutputStream() { return *this; }


    virtual const Address& getLocalAddress() {
        return delegate->getLocalAddress();
    }

    virtual const Address& getRemoteAddress() {
        return delegate->getRemoteAddress();
    }

    virtual bool isOk() {
        return delegate->isOk();
    }

    virtual void reset() {
        delegate->reset();
    }

    virtual void close() {
        delegate->close();
    }

    virtual void beginPacket() {
        delegate->beginPacket();
    }

    virtual void endPacket() {
        delegate->endPacket();
    }

    virtual void write(const Bytes& b);

    virtual int read(const Bytes& b);

    virtual void interrupt() { 
        delegate->getInputStream().interrupt();
    }

};

#endif
