/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006 Alexandre Bernardino
 * SPDX-FileCopyrightText: 2006 Carlos Beltran-Gonzalez
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ServerSerial.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(SERVERSERIAL, "yarp.devices.ServerSerial")
}

ServerSerial::~ServerSerial()
{
    closeMain();
}

bool ServerSerial::send(const Bottle& msg)
{
    if(verb)
        yCDebug(SERVERSERIAL, "std::string to send : %s", msg.toString().c_str());
    if(serial != nullptr) {
        serial->send(msg);
        return true;
    }
    else
        return false;
}

bool ServerSerial::send(char *msg, size_t size)
{
    if(verb)
        yCDebug(SERVERSERIAL, "std::string to send : %s", msg);
    if(serial != nullptr) {
        serial->send(msg, size);
        return true;
    }
    else
        return false;
}

bool ServerSerial::receive(Bottle& msg)
{
    if(serial != nullptr) {
        serial->receive(msg);
        return true;
    }
    else
        return false;
}

int ServerSerial::receiveChar(char& c)
{
    if(serial != nullptr) {
        return serial->receiveChar(c);
    }
    else
        return -1;
}

int ServerSerial::flush()
{
    if(serial != nullptr) {
        return serial->flush();
    }
    else
        return -1;
}

bool ServerSerial::setDTR(bool enable)
{
    if (serial != nullptr) {
        return serial->setDTR(enable);
    }
    else
        return false;
}

int ServerSerial::receiveLine(char* line, const int MaxLineLength)
{
    if(serial != nullptr) {
        return serial->receiveLine(line, MaxLineLength);
    }
    else
        return -1;
}

int ServerSerial::receiveBytes(unsigned char* bytes, const int size)
{
    if (serial != nullptr) {
        return serial->receiveBytes(bytes, size);
    }
    else
        return -1;
}

bool ServerSerial::open()  {
    return false;
}

bool ServerSerial::close() {
    return closeMain();
}

bool ServerSerial::open(Searchable& prop)
{
    verb = (prop.check("verbose",Value(0),"Specifies if the device is in verbose mode (0/1).").asInt32())>0;
    if (verb)
        yCInfo(SERVERSERIAL, "running with verbose output");

    Value *name;
    if (prop.check("subdevice",name,"name of specific control device to wrap")) {
        yCDebug(SERVERSERIAL, "Subdevice %s", name->toString().c_str());
        if (name->isString()) {
            // maybe user isn't doing nested configuration
            Property p;
            p.setMonitor(prop.getMonitor(),
                            "subdevice"); // pass on any monitoring
            p.fromString(prop.toString());
            p.put("device",name->toString());
            poly.open(p);
        } else {
            Bottle subdevice = prop.findGroup("subdevice").tail();
            poly.open(subdevice);
        }
        if (!poly.isValid()) {
            yCError(SERVERSERIAL, "cannot make <%s>", name->toString().c_str());
        }
    } else {
        yCError(SERVERSERIAL, "\"--subdevice <name>\" not set for serial");
        return false;
    }

    if (!poly.isValid()) {
        return false;
    }

    std::string rootName =
        prop.check("name",Value("/serial"),
                    "prefix for port names").asString();

    command_buffer.attach(toDevice);
    reply_buffer.attach(fromDevice);

    command_buffer.useCallback(callback_impl);

    toDevice.open(rootName+"/in");
    fromDevice.open(rootName+"/out");



    if (poly.isValid())
        poly.view(serial);

    if(serial != nullptr) {
        start();
        return true;
    }

    yCError(SERVERSERIAL, "subdevice <%s> doesn't look like a serial port (no appropriate interfaces were acquired)",
                    name->toString().c_str());

    return false;
}

void ServerSerial::run() {
    yCInfo(SERVERSERIAL, "Server Serial starting");
    //double before, now;
    while (!isStopping()) {
        //before = SystemClock::nowSystem();
        Bottle& b = reply_buffer.get();
        b.clear();
        receive( b );
        /*if(b.size() > 0)*/ /* this line was creating a memory leak !! */
        reply_buffer.write();
        //now = SystemClock::nowSystem();
        // give other threads the chance to run
        yarp::os::SystemClock::delaySystem(0.010);
    }
    yCInfo(SERVERSERIAL, "Server Serial stopping");
}


// ImplementCallbackHelper class.


ImplementCallbackHelper2::ImplementCallbackHelper2(ServerSerial *x)
{
    ser = dynamic_cast<yarp::dev::ISerialDevice *> (x);
    if (ser==nullptr) {
        yCError(SERVERSERIAL, "Could not get serial device");
        std::exit(1);
    }


}

void ImplementCallbackHelper2::onRead(Bottle &b)
{
    if (ser) {
        bool ok = ser->send(b);
        if (!ok)
            yCError(SERVERSERIAL, "Problems while trying to send data");
    }
}
