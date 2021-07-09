/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/TwoWayStream.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/ConnectionState.h>
#include <yarp/os/SystemClock.h>

#include <iostream>
#include <string>
#include <cstdio>

using namespace yarp::os;


class HumanStream : public TwoWayStream,
                    public InputStream,
                    public OutputStream
{
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

    void close() override {
        std::cout << "Bye bye" << std::endl;
    }

    bool isOk() const override {
        return true;
    }

    void interrupt() override {
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
    yarp::conf::ssize_t read(Bytes& b) override;

    // OutputStream
    using yarp::os::OutputStream::write;
    void write(const Bytes& b) override;

    // TwoWayStream

    InputStream& getInputStream() override {
        return *this;
    }

    OutputStream& getOutputStream() override {
        return *this;
    }

    const yarp::os::Contact& getLocalAddress() const override {
        // left undefined
        return local;
    }

    const yarp::os::Contact& getRemoteAddress() const override {
        // left undefined
        return remote;
    }

    void reset() override {
        inputCache = outputCache = "";
        std::cout << "Stream reset" << std::endl;
    }

    void beginPacket() override {
        std::cout << "Packet begins" << std::endl;
        inputCache = "";
        outputCache = "";
    }

    void endPacket() override {
        std::cout << "Packet ends" << std::endl;
    }

private:
    yarp::os::Contact local, remote;
};
