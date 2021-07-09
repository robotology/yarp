/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TCPROSSTREAM_INC
#define TCPROSSTREAM_INC

#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/ManagedBytes.h>

#include <yarp/wire_rep_utils/BlobNetworkHeader.h>
#include <yarp/wire_rep_utils/WireTwiddler.h>

#include <string>
#include <map>

class TcpRosStream :
        public yarp::os::TwoWayStream,
        public yarp::os::InputStream,
        public yarp::os::OutputStream
{
private:
    yarp::os::TwoWayStream *delegate;
    int raw;
    yarp::wire_rep_utils::BlobNetworkHeader header;
    yarp::os::ManagedBytes scan;
    char *cursor;
    int remaining;
    int phase;
    bool expectTwiddle;
    yarp::wire_rep_utils::WireTwiddler twiddler;
    std::string kind;
    yarp::wire_rep_utils::WireTwiddlerReader twiddlerReader;
    bool initiative;
    bool setInitiative;
public:
    TcpRosStream(TwoWayStream *delegate,
                 bool sender,
                 bool reply,
                 bool service,
                 int raw,
                 const char *kind) :
            delegate(delegate),
            raw(raw),
            header(yarp::wire_rep_utils::BlobNetworkHeader{0,0,0}),
            cursor(nullptr),
            remaining(0),
            phase(0),
            expectTwiddle(service && sender),
            kind(kind),
            twiddlerReader(delegate->getInputStream(), twiddler),
            initiative(false),
            setInitiative(false)
    {
        updateKind(kind,sender,reply);
    }

    virtual ~TcpRosStream() {
        if (delegate!=NULL) {
            delete delegate;
            delegate = NULL;
        }
    }

    yarp::os::InputStream& getInputStream() override { return *this; }
    yarp::os::OutputStream& getOutputStream() override { return *this; }


    const yarp::os::Contact& getLocalAddress() const override {
        return delegate->getLocalAddress();
    }

    const yarp::os::Contact& getRemoteAddress() const override {
        return delegate->getRemoteAddress();
    }

    bool isOk() const override {
        return delegate->isOk();
    }

    void reset() override {
        delegate->reset();
    }

    void close() override {
        delegate->close();
    }

    void beginPacket() override {
        twiddlerReader.reset();
        delegate->beginPacket();
    }

    void endPacket() override {
        delegate->endPacket();
    }

    using yarp::os::OutputStream::write;
    void write(const yarp::os::Bytes& b) override;

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& b) override;

    void interrupt() override {
        delegate->getInputStream().interrupt();
    }

    void updateKind(const char *kind, bool sender, bool reply);

    static std::map<std::string, std::string> rosToKind();
    static std::string rosToKind(const char *rosname);
    static bool configureTwiddler(yarp::wire_rep_utils::WireTwiddler& twiddler, const char *txt, const char *prompt, bool sender, bool reply);
};

#endif
