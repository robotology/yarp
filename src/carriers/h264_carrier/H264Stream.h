/*
 * Copyright: (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * Author: Valentina Gaggero <valentina.gaggero@iit.it>
 * Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef H264STREAM_INC
#define H264STREAM_INC

#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>
#include "BlobNetworkHeader.h"
#include "H264Decoder.h"
#include <yarp/os/InputStream.h>


namespace yarp {
    namespace os {
        class H264Stream;
    }
}

class yarp::os::H264Stream : public yarp::os::impl::DgramTwoWayStream
{
private:

    DgramTwoWayStream *delegate;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> img;
    yarp::sig::ImageNetworkHeader imgHeader;
    BlobNetworkHeader blobHeader;
    int phase;
    char *cursor;
    int remaining;
    H264Decoder *decoder;
    int remotePort;
public:
    H264Stream( int remotePort) :
            phase(0),
            cursor(NULL),
            remaining(0),
            remotePort(remotePort)
    {;}

    virtual ~H264Stream()
    {
        if (decoder!=NULL)
        {
            delete decoder;
            decoder = NULL;
        }

        if (delegate!=NULL)
        {
            delete delegate;
            delegate = NULL;
        }
    }

    bool setStream(yarp::os::impl::DgramTwoWayStream *stream);

    void start (void);

    virtual InputStream& getInputStream() override;
    virtual OutputStream& getOutputStream() override;

    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b) override;

    using yarp::os::InputStream::read;
    virtual YARP_SSIZE_T read(const Bytes& b) override;

    virtual bool setReadEnvelopeCallback(InputStream::readEnvelopeCallbackType callback, void* data) override;

};

#endif
