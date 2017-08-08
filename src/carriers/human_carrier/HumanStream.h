/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/all.h>
#include <yarp/os/Carrier.h>

#include <iostream>
#include <string>
#include <cstdio>

using namespace yarp::os;


class HumanStream : public TwoWayStream, public InputStream, public OutputStream {
private:
    bool interrupting;
    bool needInterrupt;
    std::string inputCache;
    std::string outputCache;
public:
    HumanStream() {
        interrupting = false;
        needInterrupt = false;
        inputCache = outputCache = "";
    }

    virtual void close() override {
        std::cout << "Bye bye" << std::endl;
    }

    virtual bool isOk() override {
        return true;
    }

    virtual void interrupt() override {
        interrupting = true;
        while (needInterrupt) {
            std::cout << "*** INTERRUPT: Please hit enter ***" << std::endl;
            for (int i=0; i<10 && needInterrupt; i++) {
                SystemClock::delaySystem(0.1);
            }
        }
    }

    // InputStream
    using yarp::os::InputStream::read;
    virtual YARP_SSIZE_T read(const Bytes& b) override;

    // OutputStream
    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b) override;

    // TwoWayStream

    virtual InputStream& getInputStream() override {
        return *this;
    }

    virtual OutputStream& getOutputStream() override {
        return *this;
    }

    virtual const yarp::os::Contact& getLocalAddress() override {
        // left undefined
        return local;
    }

    virtual const yarp::os::Contact& getRemoteAddress() override {
        // left undefined
        return remote;
    }

    virtual void reset() override {
        inputCache = outputCache = "";
        std::cout << "Stream reset" << std::endl;
    }

    virtual void beginPacket() override {
        std::cout << "Packet begins" << std::endl;
        inputCache = "";
        outputCache = "";
    }

    virtual void endPacket() override {
        std::cout << "Packet ends" << std::endl;
    }

private:
    yarp::os::Contact local, remote;
};
