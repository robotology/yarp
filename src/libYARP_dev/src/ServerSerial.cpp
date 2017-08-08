/*
 * Copyright (C) 2006 Alex Bernardino & Carlos Beltran-Gonzalez
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/dev/ServerSerial.h>
#include <yarp/os/Log.h>
#include <yarp/os/Os.h>

ServerSerial::ServerSerial() :
        verb(false),
        serial(NULL),
        callback_impl(this)
{
}


ServerSerial::~ServerSerial()
{
    closeMain();
}

bool ServerSerial::send(const Bottle& msg)
{
    if(verb)
        yDebug("ConstString to send : %s\n", msg.toString().c_str());
    if(serial != NULL) {
        serial->send(msg);
        return true;
    }
    else
        return false;
}

bool ServerSerial::send(char *msg, size_t size)
{
    if(verb)
        yDebug("ConstString to send : %s\n", msg);
    if(serial != NULL) {
        serial->send(msg, size);
        return true;
    }
    else
        return false;
}

bool ServerSerial::receive(Bottle& msg)
{
    if(serial != NULL) {
        serial->receive(msg);
        return true;
    }
    else
        return false;
}

int ServerSerial::receiveChar(char& c)
{
    if(serial != NULL) {
        return serial->receiveChar(c);
    }
    else
        return -1;
}

int ServerSerial::flush()
{
    if(serial != NULL) {
        return serial->flush();
    }
    else
        return -1;
}

bool ServerSerial::setDTR(bool enable)
{
    if (serial != NULL) {
        return serial->setDTR(enable);
    }
    else
        return false;
}

int ServerSerial::receiveLine(char* line, const int MaxLineLength)
{
    if(serial != NULL) {
        return serial->receiveLine(line, MaxLineLength);
    }
    else
        return -1;
}

int ServerSerial::receiveBytes(unsigned char* bytes, const int size)
{
    if (serial != NULL) {
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
    verb = (prop.check("verbose",Value(0),"Specifies if the device is in verbose mode (0/1).").asInt())>0;
    if (verb)
        yInfo("running with verbose output\n");

    Value *name;
    if (prop.check("subdevice",name,"name of specific control device to wrap")) {
        yDebug("Subdevice %s\n", name->toString().c_str());
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
            yError("cannot make <%s>\n", name->toString().c_str());
        }
    } else {
        yError("\"--subdevice <name>\" not set for server_serial\n");
        return false;
    }

    if (!poly.isValid()) {
        return false;
    }

    ConstString rootName =
        prop.check("name",Value("/serial"),
                    "prefix for port names").asString().c_str();

    command_buffer.attach(toDevice);
    reply_buffer.attach(fromDevice);

    command_buffer.useCallback(callback_impl);

    toDevice.open((rootName+"/in").c_str());
    fromDevice.open((rootName+"/out").c_str());



    if (poly.isValid())
        poly.view(serial);

    if(serial != NULL) {
        start();
        return true;
    }

    yError("subdevice <%s> doesn't look like a serial port (no appropriate interfaces were acquired)\n",
                    name->toString().c_str());

    return false;
}

void ServerSerial::run() {
    yInfo("Server Serial starting\n");
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
    yInfo("Server Serial stopping\n");
}


// ImplementCallbackHelper class.


yarp::dev::ImplementCallbackHelper2::ImplementCallbackHelper2(yarp::dev::ServerSerial *x) {
    ser = dynamic_cast<yarp::dev::ISerialDevice *> (x);
    //ACE_ASSERT (ser != 0);
    if (ser==0) {
        yError("Could not get serial device\n");
        std::exit(1);
    }


}

void yarp::dev::ImplementCallbackHelper2::onRead(Bottle &b) {
    //printf("Data received on the control channel of size: %d\n", v.body.size());
    //    int i;
   if (ser) {
        bool ok = ser->send(b);
        if (!ok)
            yError("Problems while trying to send data\n");
    }
}
