// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <yarp/os/all.h>

#include <yarp/os/impl/Carrier.h>
#include <yarp/os/impl/Carriers.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/Protocol.h>

#include <iostream>
#include <string>

using namespace std;
using namespace yarp::os;
using namespace yarp::os::impl;


class HumanStream : public TwoWayStream, public InputStream, public OutputStream {
private:
    bool interrupting;
    bool needInterrupt;
    string inputCache;
    string outputCache;
public:
    HumanStream() {
        interrupting = false;
        needInterrupt = false;
        inputCache = outputCache = "";
    }

    virtual void close() {
        cout << "Bye bye" << endl;
    }

    virtual bool isOk() {
        return true;
    }

    virtual void interrupt() {
        interrupting = true;
        while (needInterrupt) {
            cout << "*** INTERRUPT: Please hit enter ***" << endl;
            for (int i=0; i<10 && needInterrupt; i++) {
                Time::delay(0.1);
            }
        }
    }

    // InputStream

    virtual ssize_t read(const Bytes& b);

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
        cout << "Stream reset" << endl;
    }

    virtual void beginPacket() {
        cout << "Packet begins" << endl;
        inputCache = "";
        outputCache = "";
    }

    virtual void endPacket() {
        cout << "Packet ends" << endl;
    }

private:
    yarp::os::Contact local, remote;
};
