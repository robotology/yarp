// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_WIRETWIDDLER
#define YARP2_WIRETWIDDLER

#include <yarp/conf/numeric.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/NetInt32.h>

#include <yarp/os/SizedWriter.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Bytes.h>

#include <wire_rep_utils_api.h>

#include <vector>
#include <string>

class WireTwiddlerGap {
public:
    int buffer_start;
    int buffer_length;
    int length;
    int unit_length;
    int wire_unit_length;
    char *byte_start;
    int byte_length;
    bool ignore_external;
    bool save_external;
    bool load_external;
    bool computing;
    yarp::os::ConstString origin;
    yarp::os::ConstString var_name;
    int flavor;
    WireTwiddlerGap() {
        buffer_start = 0;
        buffer_length = 0;
        length = 0;
        unit_length = 0;
        byte_start = 0/*NULL*/;
        byte_length = 0;
        ignore_external = false;
        wire_unit_length = -1;
        save_external = false;
        load_external = false;
        computing = false;
        flavor = 0;
    }

    char *getStart() const { return byte_start; }
    int getLength() const { return byte_length; }
    int getUnitLength() const { return unit_length; }
    int getExternalLength() const { return unit_length; }
    bool shouldIgnoreExternal() const { return ignore_external; }
    const yarp::os::ConstString& getOrigin() const { return origin; }
};


class YARP_wire_rep_utils_API WireTwiddler {
public:
    WireTwiddler() {
        buffer_start = 0;
        writer = 0 /*NULL*/;
    }

    virtual ~WireTwiddler() {
        if (writer) delete writer;
        writer = 0 /*NULL*/;
    }

    bool configure(const char *txt, const char *prompt);


    std::string fromTemplate(const yarp::os::Bottle& msg);

    /*
    bool setHeaderFormat(const char *txt) {
        headerFormat = txt;
    }
    */

private:

    int buffer_start;
    std::vector<yarp::os::NetInt32> buffer;
    std::vector<WireTwiddlerGap> gaps;
    yarp::os::ConnectionWriter *writer;
    yarp::os::ConstString prompt;

public:
    void show();
    int configure(yarp::os::Bottle& desc, int offset, bool& ignored,
                  const yarp::os::ConstString& vtag);

    int getGapCount() {
        return (int)gaps.size();
    }

    void clear() {
        buffer_start = 0;
        buffer.clear();
        gaps.clear();
    }

    const WireTwiddlerGap& getGap(int index) {
        return gaps[index];
    }

    yarp::os::ConstString toString() const;

    const yarp::os::ConstString& getPrompt() const {
        return prompt;
    }

    bool read(yarp::os::Bottle& bot, const yarp::os::Bytes& data);

    bool write(yarp::os::Bottle& bot, yarp::os::ManagedBytes& data);
};


/*

  State:
  index, sent

 */

class YARP_wire_rep_utils_API WireTwiddlerReader : public yarp::os::InputStream {
private:
    yarp::os::InputStream& is;
    WireTwiddler& twiddler;
    char *cursor;
    int sent;
    int consumed;
    int override_length;
    int index;
    bool recite;
    yarp::os::NetInt32 lengthBuffer;
    int pending_length;
    int pending_strings;
    int pending_string_length;
    int pending_string_data;
    yarp::os::ManagedBytes dump;
    yarp::os::Property prop;
public:
    WireTwiddlerReader(yarp::os::InputStream& is,
                       WireTwiddler& twiddler) : is(is),
                                                 twiddler(twiddler) {
        reset();
    }

    void reset() {
        recite = false;
        index = -1;
        sent = 0;
        consumed = 0;
        cursor = 0 /*NULL*/;
        pending_length = 0;
        pending_strings = 0;
        pending_string_length = 0;
        pending_string_data = 0;
        override_length = -1;
    }

    virtual ~WireTwiddlerReader() {}

    virtual YARP_SSIZE_T read(const yarp::os::Bytes& b);

    virtual void close() { is.close(); }

    virtual bool isOk() { return is.isOk(); }

    YARP_SSIZE_T readMapped(yarp::os::InputStream& is,
                            const yarp::os::Bytes& b,
                            const WireTwiddlerGap& gap);

    void compute(const WireTwiddlerGap& gap);
};


class YARP_wire_rep_utils_API WireTwiddlerSrc {
public:
    char *src;
    int len;
    int offset;

    WireTwiddlerSrc(char *src, int len, int offset) {
        this->src = src;
        this->len = len;
        this->offset = offset;
    }
};

class YARP_wire_rep_utils_API WireTwiddlerWriter : public yarp::os::SizedWriter {
private:
    yarp::os::SizedWriter *parent;
    WireTwiddler *twiddler;
    std::vector<WireTwiddlerSrc> srcs;
    int block;
    int lastBlock;
    int offset;
    const char *blockPtr;
    int blockLen;
    yarp::os::NetInt32 lengthBuffer;
    yarp::os::Bytes lengthBytes;
    yarp::os::ManagedBytes zeros;
    yarp::os::ManagedBytes scratch;
    int accumOffset;
    const char *activeEmit;
    const WireTwiddlerGap *activeGap;
    int activeEmitLength;
    int activeEmitOffset;
    const char *activeCheck;
    bool errorState;
    size_t scratchOffset;
    yarp::os::NetInt32 codeExpected;
    yarp::os::NetInt32 codeReceived;

    void showBrokenExpectation(const yarp::os::NetInt32& expected,
                               const yarp::os::NetInt32& received,
                               int evidence);
public:
    WireTwiddlerWriter(yarp::os::SizedWriter& parent,
                       WireTwiddler& twiddler) : parent(&parent),
                                                 twiddler(&twiddler) {
                                                     //update();
    }

    WireTwiddlerWriter() {
        parent = NULL;
        twiddler = NULL;
    }

    virtual ~WireTwiddlerWriter();

    void attach(yarp::os::SizedWriter& parent,
                WireTwiddler& twiddler) {
        this->parent = &parent;
        this->twiddler = &twiddler;
        //update();
    }

    bool update();

    virtual size_t length() {
        return srcs.size();
    }

    virtual size_t headerLength() {
        return 0;
    }

    virtual size_t length(size_t index)  {
        return srcs[index].len;
    }

    virtual const char *data(size_t index) {
        if (srcs[index].offset<0) return srcs[index].src;
        return scratch.get()+srcs[index].offset;
    }

    virtual yarp::os::PortReader *getReplyHandler() {
        return parent->getReplyHandler();
    }

    virtual yarp::os::Portable *getReference() {
        return parent->getReference();
    }

    bool skip(const char *start, int len);

    bool transform(const WireTwiddlerGap& gap);

    bool pass(int len);

    bool pad(size_t len);

    bool readLengthAndPass(int unitLength, const WireTwiddlerGap *gap = 0/*NULL*/);

    bool advance(int length, bool shouldEmit, bool shouldAccum=false,
                 bool shouldCheck=false);

    bool emit(const char *src, int len);

    int readLength();

    //virtual void write(OutputStream& os);
    virtual bool dropRequested() {
        return false;
    }

    virtual void startWrite() {
        parent->startWrite();
    }

    virtual void stopWrite() {
        parent->stopWrite();
    }
};

#endif

