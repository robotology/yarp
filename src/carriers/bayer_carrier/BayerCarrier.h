// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef BAYERCARRIER_INC
#define BAYERCARRIER_INC

#include <yarp/os/ModifyingCarrier.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>
#include <yarp/os/DummyConnector.h>

namespace yarp {
    namespace os {
        class BayerCarrier;
    }
}


/**
 *
 * Decode bayer images and serve them as regular rgb.
 * Affected by carrier modifiers.  Examples:
 *   tcp+recv.bayer
 *   tcp+recv.bayer+size.half
 *   tcp+recv.bayer+size.half+order.bggr
 *
 */
class yarp::os::BayerCarrier : public yarp::os::ModifyingCarrier,
                               public yarp::os::ConnectionReader,
                               public yarp::os::InputStream {
private:
    yarp::sig::ImageOf<yarp::sig::PixelMono> in;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> out;
    yarp::sig::ImageNetworkHeader header;
    yarp::sig::ImageNetworkHeader header_in;
    yarp::os::DummyConnector con;
    size_t image_data_len, consumed;

    yarp::os::ConnectionReader *local;
    yarp::os::ConnectionReader *parent;
    
    bool need_reset;
    bool have_result;
    bool happy;
    bool half;
    bool warned;
    bool bayer_method_set;

    int bayer_method;

    // format offsets
    int goff; // x offset to green on even rows
    int roff; // y offset to red on even columns
    int dcformat;

    bool setFormat(const char *fmt);
public:

    ////////////////////////////////////////////////////////////////////////
    // ModifyingCarrier methods


    BayerCarrier() {
        need_reset = true;
        have_result = false;
        image_data_len = 0;
        half = false;
        warned = false;
        goff = 0;
        roff = 1;
        bayer_method = -1;
        bayer_method_set = false;
        dcformat = -1;
        local = yarp::os::ConnectionReader::createConnectionReader(*this);
        happy = (local!=0);
    }

    ~BayerCarrier() {
        if (local) delete local;
    }

    virtual Carrier *create() {
        return new BayerCarrier();
    }

    virtual ConstString getName() {
        return "bayer";
    }

    virtual ConstString toString() {
        return "bayer_carrier";
    }

    virtual yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader);


    ////////////////////////////////////////////////////////////////////////
    // ConnectionReader methods

    void setParent(yarp::os::ConnectionReader& reader) {
        parent = &reader;
    }

    virtual bool debayerFull(yarp::sig::ImageOf<yarp::sig::PixelMono>& src,
                             yarp::sig::ImageOf<yarp::sig::PixelRgb>& dest);

    virtual bool debayerHalf(yarp::sig::ImageOf<yarp::sig::PixelMono>& src,
                             yarp::sig::ImageOf<yarp::sig::PixelRgb>& dest);

    virtual bool processBuffered();

    virtual bool processDirect(const yarp::os::Bytes& bytes);

    virtual bool expectBlock(const char *data, size_t len) {
        return local->expectBlock(data,len);
    }

    virtual ConstString expectText(int terminatingChar = '\n') {
        return local->expectText(terminatingChar);
    }

    virtual int expectInt() {
        return local->expectInt();
    }

    virtual YARP_INT64 expectInt64() {
        return local->expectInt64();
    }

    virtual bool pushInt(int x) {
        return local->pushInt(x);
    }

    virtual double expectDouble() {
        return local->expectDouble();
    }

    virtual bool isTextMode() {
        return false;
    }

    virtual bool isBareMode() {
        return false;
    }

    virtual bool convertTextMode() {
        return true;
    }

    virtual size_t getSize() {
        if (image_data_len) processBuffered();
        return sizeof(header)+image_data_len;
    }

    virtual ConnectionWriter *getWriter() {
        return parent->getWriter();
    }

    virtual Bytes readEnvelope() { 
        return parent->readEnvelope();
    }

    virtual Portable *getReference() {
        return parent->getReference();
    }

    virtual Contact getRemoteContact() {
        return parent->getRemoteContact();
    }

    virtual Contact getLocalContact() {
        return parent->getLocalContact();
    }

    virtual bool isValid() {
        return true;
    }

    virtual bool isActive() {
        return parent->isActive();
    }

    virtual bool isError() {
        return parent->isError()||!happy;
    }

    virtual void requestDrop() {
        parent->requestDrop();
    }

    virtual yarp::os::Searchable& getConnectionModifiers() {
        return parent->getConnectionModifiers();
    }

    virtual bool setSize(size_t len) {
        return parent->setSize(len);
    }

    ////////////////////////////////////////////////////////////////////////
    // InputStream methods

    using yarp::os::InputStream::read;
    virtual YARP_SSIZE_T read(const yarp::os::Bytes& b);

    virtual void close() {
    }

    virtual bool isOk() {
        return happy;
    }
};


#endif
