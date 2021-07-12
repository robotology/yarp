/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef BAYERCARRIER_INC
#define BAYERCARRIER_INC

#include <yarp/os/ModifyingCarrier.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>
#include <yarp/os/DummyConnector.h>

/**
 *
 * Decode bayer images and serve them as regular rgb.
 * Affected by carrier modifiers.  Examples:
 *   tcp+recv.bayer
 *   tcp+recv.bayer+size.half
 *   tcp+recv.bayer+size.half+order.bggr
 *
 */
class BayerCarrier :
        public yarp::os::ModifyingCarrier,
        public yarp::os::ConnectionReader,
        public yarp::os::InputStream
{
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
        parent(nullptr),
        need_reset(true),
        have_result(false),
        happy(local!=0),
        half(false),
        bayer_method_set(false),
        bayer_method(-1),
        goff(0),
        roff(1),
        dcformat(-1)
    {}

    ~BayerCarrier() {
        if (local) delete local;
    }

    Carrier *create() const override {
        return new BayerCarrier();
    }

    std::string getName() const override {
        return "bayer";
    }

    std::string toString() const override {
        return "bayer_carrier";
    }

    yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader) override;


    ////////////////////////////////////////////////////////////////////////
    // ConnectionReader methods

    bool expectBlock(char *data, size_t len) override {
        return local->expectBlock(data,len);
    }

    std::string expectText(const char terminatingChar = '\n') override {
        return local->expectText(terminatingChar);
    }

    std::int8_t expectInt8() override {
        return local->expectInt8();
    }

    std::int16_t expectInt16() override {
        return local->expectInt64();
    }

    std::int32_t expectInt32() override {
        return local->expectInt32();
    }

    std::int64_t expectInt64() override {
        return local->expectInt64();
    }

    bool pushInt(int x) override {
        return local->pushInt(x);
    }

    yarp::conf::float32_t expectFloat32() override {
        return local->expectFloat32();
    }

    yarp::conf::float64_t expectFloat64() override {
        return local->expectFloat64();
    }

    bool isTextMode() const override {
        return false;
    }

    bool isBareMode() const override {
        return false;
    }

    bool convertTextMode() override {
        return true;
    }

    size_t getSize() const override {
        if (image_data_len) {
            processBuffered();
        }
        return sizeof(header)+image_data_len;
    }

    yarp::os::ConnectionWriter *getWriter() override {
        return parent->getWriter();
    }

    yarp::os::Bytes readEnvelope() override {
        return parent->readEnvelope();
    }

    yarp::os::Portable* getReference() const override {
        return parent->getReference();
    }

    yarp::os::Contact getRemoteContact() const override {
        return parent->getRemoteContact();
    }

    yarp::os::Contact getLocalContact() const override {
        return parent->getLocalContact();
    }

    bool isValid() const override {
        return true;
    }

    bool isActive() const override {
        return parent->isActive();
    }

    bool isError() const override {
        return parent->isError()||!happy;
    }

    void requestDrop() override {
        parent->requestDrop();
    }

    const yarp::os::Searchable& getConnectionModifiers() const override {
        return parent->getConnectionModifiers();
    }

    bool setSize(size_t len) override {
        return parent->setSize(len);
    }

    ////////////////////////////////////////////////////////////////////////
    // InputStream methods

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& b) override;

    void close() override {
    }

    bool isOk() const override {
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

    /*
     * The const version of the processBuffered() method performs a const_cast,
     * and calls the non-const version. This allows to call it in const methods.
     * Conceptually this is not completely wrong because it does not modify
     * the external state of the class, but just some internal representation.
     */
    virtual bool processBuffered() const;

    virtual bool processBuffered();

    virtual bool processDirect(yarp::os::Bytes& bytes);

};


#endif
