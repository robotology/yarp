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


    BayerCarrier() :
        image_data_len(0),
        consumed(0),
        local(yarp::os::ConnectionReader::createConnectionReader(*this)),
        parent(YARP_NULLPTR),
        need_reset(true),
        have_result(false),
        happy(local!=0),
        half(false),
        warned(false),
        bayer_method_set(false),
        bayer_method(-1),
        goff(0),
        roff(1),
        dcformat(-1)
    {}

    ~BayerCarrier() {
        if (local) delete local;
    }

    virtual Carrier *create() override {
        return new BayerCarrier();
    }

    virtual ConstString getName() override {
        return "bayer";
    }

    virtual ConstString toString() override {
        return "bayer_carrier";
    }

    virtual yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader) override;


    ////////////////////////////////////////////////////////////////////////
    // ConnectionReader methods

    virtual bool expectBlock(const char *data, size_t len) override {
        return local->expectBlock(data,len);
    }

    virtual ConstString expectText(int terminatingChar = '\n') override {
        return local->expectText(terminatingChar);
    }

    virtual int expectInt() override {
        return local->expectInt();
    }

    virtual YARP_INT64 expectInt64() override {
        return local->expectInt64();
    }

    virtual bool pushInt(int x) override {
        return local->pushInt(x);
    }

    virtual double expectDouble() override {
        return local->expectDouble();
    }

    virtual bool isTextMode() override {
        return false;
    }

    virtual bool isBareMode() override {
        return false;
    }

    virtual bool convertTextMode() override {
        return true;
    }

    virtual size_t getSize() override {
        if (image_data_len) {
            processBuffered();
        }
        return sizeof(header)+image_data_len;
    }

    virtual ConnectionWriter *getWriter() override {
        return parent->getWriter();
    }

    virtual Bytes readEnvelope() override {
        return parent->readEnvelope();
    }

    virtual Portable *getReference() override {
        return parent->getReference();
    }

    virtual Contact getRemoteContact() override {
        return parent->getRemoteContact();
    }

    virtual Contact getLocalContact() override {
        return parent->getLocalContact();
    }

    virtual bool isValid() override {
        return true;
    }

    virtual bool isActive() override {
        return parent->isActive();
    }

    virtual bool isError() override {
        return parent->isError()||!happy;
    }

    virtual void requestDrop() override {
        parent->requestDrop();
    }

    virtual yarp::os::Searchable& getConnectionModifiers() override {
        return parent->getConnectionModifiers();
    }

    virtual bool setSize(size_t len) override {
        return parent->setSize(len);
    }

    ////////////////////////////////////////////////////////////////////////
    // InputStream methods

    using yarp::os::InputStream::read;
    virtual YARP_SSIZE_T read(const yarp::os::Bytes& b) override;

    virtual void close() override {
    }

    virtual bool isOk() override {
        return happy;
    }

    ////////////////////////////////////////////////////////////////////////
    // BayerCarrier methods

    void setParent(yarp::os::ConnectionReader& reader) {
        parent = &reader;
    }

    virtual bool debayerFull(yarp::sig::ImageOf<yarp::sig::PixelMono>& src,
                             yarp::sig::ImageOf<yarp::sig::PixelRgb>& dest);

    virtual bool debayerHalf(yarp::sig::ImageOf<yarp::sig::PixelMono>& src,
                             yarp::sig::ImageOf<yarp::sig::PixelRgb>& dest);

    virtual bool processBuffered();

    virtual bool processDirect(const yarp::os::Bytes& bytes);

};


#endif
