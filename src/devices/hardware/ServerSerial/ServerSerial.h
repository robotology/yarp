/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006 Alexandre Bernardino
 * SPDX-FileCopyrightText: 2006 Carlos Beltran-Gonzalez
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_SERVERSERIAL_H
#define YARP_DEV_SERVERSERIAL_H

#include <cstdio>
#include <cstdlib>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ISerialDevice.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>


using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


class ServerSerial;

/**
 * Callback implementation after buffered input.
 */
class ImplementCallbackHelper2 :
        public yarp::os::TypedReaderCallback<yarp::os::Bottle>
{
protected:
    yarp::dev::ISerialDevice* ser{nullptr};

public:
    /**
     * Constructor.
     * @param x is the instance of the container class using the callback.
     */
    ImplementCallbackHelper2();
    ImplementCallbackHelper2(ServerSerial *x);

    using yarp::os::TypedReaderCallback<Bottle>::onRead;
    /**
     * Callback function.
     * @param b is the Bottle being received.
     */
    void onRead(Bottle& b) override;
};


/**
 * @ingroup dev_impl_wrapper
 *
 * \brief `serial`: Export a serial sensor.
 *
 * The network interface is composed by two ports.
 * The input port accepts bottles and sends their contents as
 * text to the serial port.
 * The output port streams out whatever information it gets in the
 * serial port as text bottles.
 */
class ServerSerial :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ISerialDevice,
        private yarp::os::Thread
{
private:
    bool verb{false};
    yarp::dev::PolyDriver poly;
    yarp::os::Port toDevice;
    yarp::os::Port fromDevice;

    yarp::os::PortWriterBuffer <yarp::os::Bottle> reply_buffer;
    yarp::os::PortReaderBuffer <yarp::os::Bottle> command_buffer;

    yarp::dev::ISerialDevice *serial{nullptr};
    ImplementCallbackHelper2 callback_impl{this};


    bool closeMain()
    {
        if (Thread::isRunning()) {
            Thread::stop();
        }
        //close the port connection here
        toDevice.close();
        fromDevice.close();
        poly.close();
        return true;
    }

public:
    ServerSerial() = default;
    ServerSerial(const ServerSerial&) = delete;
    ServerSerial(ServerSerial&&) = delete;
    ServerSerial& operator=(const ServerSerial&) = delete;
    ServerSerial& operator=(ServerSerial&&) = delete;
    ~ServerSerial() override;

    bool send(const Bottle& msg) override;
    bool send(char *msg, size_t size) override;
    bool receive(Bottle& msg) override;
    int receiveChar(char& c) override;
    int flush () override;
    int receiveLine(char* line, const int MaxLineLength) override;
    int receiveBytes(unsigned char* bytes, const int size) override;
    bool setDTR(bool enable) override;

    /**
     * Default open() method.
     * @return always false since initialization requires certain parameters.
     */
    virtual bool open();

    /**
     * Close the device driver by deallocating all resources and closing ports.
     * @return true if successful or false otherwise.
     */
    bool close() override;

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
    bool open(Searchable& prop) override;

    /**
     * The thread main loop deals with writing on ports here.
     */
    void run() override;
};

#endif // YARP_DEV_SERVERSERIAL_H
