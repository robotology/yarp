// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <yarp/os/all.h>
#include <yarp/os/Carrier.h>

#include <iostream>
#include <string>

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

    virtual void close() {
        std::cout << "Bye bye" << std::endl;
    }

    virtual bool isOk() {
        return true;
    }

    virtual void interrupt() {
        interrupting = true;
        while (needInterrupt) {
            std::cout << "*** INTERRUPT: Please hit enter ***" << std::endl;
            for (int i=0; i<10 && needInterrupt; i++) {
                Time::delay(0.1);
            }
        }
    }

    // InputStream

    virtual YARP_SSIZE_T read(const Bytes& b);

    // OutputStream

    virtual void write(const Bytes& b);

    // TwoWayStream

    virtual InputStream& getInputStream() {
        return *this;
    }

    virtual OutputStream& getOutputStream() {
        return *this;
    }

    virtual const yarp::os::Contact& getLocalAddress() {
        // left undefined
        return local;
    }

    virtual const yarp::os::Contact& getRemoteAddress() {
        // left undefined
        return remote;
    }

    virtual void reset() {
        inputCache = outputCache = "";
        std::cout << "Stream reset" << std::endl;
    }

    virtual void beginPacket() {
        std::cout << "Packet begins" << std::endl;
        inputCache = "";
        outputCache = "";
    }

    virtual void endPacket() {
        std::cout << "Packet ends" << std::endl;
    }

private:
    yarp::os::Contact local, remote;
};
