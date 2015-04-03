// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Alex Bernardino & Carlos Beltran-Gonzalez
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef _YARP2_SERVERSERIAL_
#define _YARP2_SERVERSERIAL_

#include <stdio.h>
#include <stdlib.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/SerialInterfaces.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>


using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


namespace yarp
{
    namespace dev
    {
        class ServerSerial;
        class ImplementCallbackHelper2;
    }
}

/**
 * Callback implementation after buffered input.
 */
class yarp::dev::ImplementCallbackHelper2 : public TypedReaderCallback<Bottle> {
protected:
    ISerialDevice *ser;

public:
    /**
     * Constructor.
     * @param x is the instance of the container class using the callback.
     */
    ImplementCallbackHelper2();
    ImplementCallbackHelper2(yarp::dev::ServerSerial *x);

    /**
     * Callback function.
     * @param b is the Bottle being received.
     */
    virtual void onRead(Bottle& b);
};


/**
 * @ingroup dev_impl_wrapper
 *
 * Export a serial sensor.
 * The network interface is composed by two ports.
 * The input port accepts bottles and sends their contents as
 * text to the serial port.
 * The output port streams out whatever information it gets in the
 * serial port as text bottles.
 *
 * @author Alex Bernardino
 */
class YARP_dev_API yarp::dev::ServerSerial : public DeviceDriver,
                                             public ISerialDevice,
                                             private RateThread
{
private:
    bool verb;
    PolyDriver poly;
    Port toDevice;
    Port fromDevice;

    PortWriterBuffer<Bottle> reply_buffer;
    PortReaderBuffer<Bottle> command_buffer;

    ISerialDevice *serial;
    ImplementCallbackHelper2 callback_impl;

    bool closeMain();

public:
    ServerSerial();
    virtual ~ServerSerial();
    virtual bool send(const Bottle& msg);
    virtual bool send(char *msg, size_t size);
    virtual bool receive(Bottle& msg);
    virtual int receiveChar(char& c);
    virtual int flush ();
    virtual int receiveLine(char* line, const int MaxLineLength);

    /**
    * Default open() method.
    * @return always false since initialization requires certain parameters.
    */
    virtual bool open();

    /**
    * Close the device driver by deallocating all resources and closing ports.
    * @return true if successful or false otherwise.
    */
    virtual bool close();

    /**
     * Open the device driver.
     *
     * @param prop is a Searchable object that contains the parameters to use.
     * Allowed parameters are:
     * - verbose or v to print diagnostic information while running,
     * - name to specify the prefix of the port names
     * and all parameters requied by the wrapped device driver.
     * @return true iff the object could be configured.
     */
    virtual bool open(Searchable& prop);

    /**
     * Initialization method.
     */
    virtual bool threadInit();

    /**
     * Release method.
     */
    virtual void threadRelease();

    /**
     * The thread main loop deals with writing on ports here.
     */
    virtual void run();
};

#endif
