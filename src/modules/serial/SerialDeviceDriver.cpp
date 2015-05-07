// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alex Bernardino, Carlos Beltran-Gonzalez
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <SerialDeviceDriver.h>

#include <stdio.h>
#include <stdlib.h>

///#include <yarp/os/Time.h>
#include <yarp/os/Log.h>

using namespace yarp::os;
using namespace yarp::dev;

//inline SerialHandler& RES(void *res) { return *(SerialHandler *)res; }

SerialDeviceDriver::SerialDeviceDriver() :
line_terminator_char1('\r'), line_terminator_char2('\n'),
deviceOpened(false)//, stopCondition(conditionMutex),
//shouldStop(false), stopAck(false)
 {
    //system_resources = (SerialHandler*) new SerialHandler();
    verbose=false;
}

SerialDeviceDriver::~SerialDeviceDriver() {
    close();
}

bool SerialDeviceDriver::open(SerialDeviceDriverSettings& config)
{
    //if(RES(system_resources).initialize(config.CommChannel, config.SerialParams) < 0)
    //    return false;
    //RES(system_resources).setCommandSender(this);
    //yTrace("SerialHandler::initialize");
    yInfo("Starting Serial Port in %s \n", config.CommChannel);

    // Initialize serial port
    if(_serialConnector.connect(_serial_dev, ACE_DEV_Addr(config.CommChannel),
                                0, ACE_Addr::sap_any, 0, O_NONBLOCK) == -1)
    {
        yError("Invalid communications port in %s \n", config.CommChannel);
        return false;
    }

    if(_serialConnector.connect(_send_serial_dev, ACE_DEV_Addr(config.CommChannel)) == -1)
    {
        yError("Invalid communications port in %s \n", config.CommChannel);
        return false;
    }

    // Set TTY_IO parameter into the ACE_TTY_IO device(_serial_dev)
    if (_serial_dev.control (ACE_TTY_IO::SETPARAMS, &config.SerialParams) == -1)
    {
        yError("Can not control communications port %s \n", config.CommChannel);
        yError("Can not control communications port %s \n", config.CommChannel);
        return false;
    }

    // Set TTY_IO parameter into the ACE_TTY_IO device(_serial_dev)
    if (_send_serial_dev.control (ACE_TTY_IO::SETPARAMS, &config.SerialParams) == -1)
    {
        yError("Can not control communications port %s \n", config.CommChannel);
        return false;
    }

    deviceOpened = true;
    return true;
}


bool SerialDeviceDriver::open(yarp::os::Searchable& config) {
    SerialDeviceDriverSettings config2;
    strcpy(config2.CommChannel, config.check("comport",Value("COM3"),"name of the serial channel").asString().c_str());
    this->verbose = (config.check("verbose",Value(1),"Specifies if the device is in verbose mode (0/1).").asInt())>0;
    config2.SerialParams.baudrate = config.check("baudrate",Value(9600),"Specifies the baudrate at which the communication port operates.").asInt();
    config2.SerialParams.xonlim = config.check("xonlim",Value(0),"Specifies the minimum number of bytes in input buffer before XON char is sent. Negative value indicates that default value should be used (Win32)").asInt();
    config2.SerialParams.xofflim = config.check("xofflim",Value(0),"Specifies the maximum number of bytes in input buffer before XOFF char is sent. Negative value indicates that default value should be used (Win32). ").asInt();
    config2.SerialParams.readmincharacters = config.check("readmincharacters",Value(1),"Specifies the minimum number of characters for non-canonical read (POSIX).").asInt();
    config2.SerialParams.readtimeoutmsec = config.check("readtimeoutmsec",Value(100),"Specifies the time to wait before returning from read. Negative value means infinite timeout.").asInt();
    // config2.SerialParams.parityenb = config.check("parityenb",Value(0),"Enable/disable parity checking.").asInt();
    yarp::os::ConstString temp = config.check("paritymode",Value("EVEN"),"Specifies the parity mode (EVEN, ODD, NONE). POSIX supports even and odd parity. Additionally Win32 supports mark and space parity modes.").asString().c_str();
    config2.SerialParams.paritymode = temp.c_str();
    config2.SerialParams.ctsenb = config.check("ctsenb",Value(0),"Enable & set CTS mode. Note that RTS & CTS are enabled/disabled together on some systems (RTS/CTS is enabled if either <code>ctsenb</code> or <code>rtsenb</code> is set).").asInt();
    config2.SerialParams.rtsenb = config.check("rtsenb",Value(0),"Enable & set RTS mode. Note that RTS & CTS are enabled/disabled together on some systems (RTS/CTS is enabled if either <code>ctsenb</code> or <code>rtsenb</code> is set).\n- 0 = Disable RTS.\n- 1 = Enable RTS.\n- 2 = Enable RTS flow-control handshaking (Win32).\n- 3 = Specifies that RTS line will be high if bytes are available for transmission.\nAfter transmission RTS will be low (Win32).").asInt();
    config2.SerialParams.xinenb = config.check("xinenb",Value(0),"Enable/disable software flow control on input.").asInt();
    config2.SerialParams.xoutenb = config.check("xoutenb",Value(0),"Enable/disable software flow control on output.").asInt();
    config2.SerialParams.modem = config.check("modem",Value(0),"Specifies if device is a modem (POSIX). If not set modem status lines are ignored. ").asInt();
    config2.SerialParams.rcvenb = config.check("rcvenb",Value(0),"Enable/disable receiver (POSIX).").asInt();
    config2.SerialParams.dsrenb = config.check("dsrenb",Value(0),"Controls whether DSR is disabled or enabled (Win32).").asInt();
    config2.SerialParams.dtrdisable = config.check("dtrdisable",Value(0),"Controls whether DTR is disabled or enabled.").asInt();
    config2.SerialParams.databits = config.check("databits",Value(7),"Data bits. Valid values 5, 6, 7 and 8 data bits. Additionally Win32 supports 4 data bits.").asInt();
    config2.SerialParams.stopbits = config.check("stopbits",Value(1),"Stop bits. Valid values are 1 and 2.").asInt();

    if (config.check("line_terminator_char1", "line terminator character for receiveLine(), default '\r'"))
        line_terminator_char1 = config.find("line_terminator_char1").asInt();

    if (config.check("line_terminator_char2", "line terminator character for receiveLine(), default '\n'"))
        line_terminator_char2 = config.find("line_terminator_char2").asInt();

    return open(config2);
}

bool SerialDeviceDriver::close(void) {
    if (!deviceOpened) return true;    
    // stopAck = false;
    
    // conditionMutex.lock();
    // shouldStop = true;
    // while(!stopAck) {
    //     stopCondition.wait();
    // }
    // conditionMutex.release();

    _serial_dev.close();
    _send_serial_dev.close();

    deviceOpened = false;
    return true;
}

bool SerialDeviceDriver::send(const Bottle& msg)
{
    if (msg.size() > 0) {
        int message_size = msg.get(0).asString().length();

        if (message_size > 0) {
            if (verbose) {
                yDebug("Sending string: %s", msg.get(0).asString().c_str());
                if (msg.get(0).asString().c_str()[message_size - 1] != '\n') {    // Add \n only if reply does not contain it already
                    ACE_OS::printf("\n");
                }
            }

            // Write message to the serial device
            ssize_t bytes_written = _send_serial_dev.send_n((void *) msg.get(0).asString().c_str(), message_size);

            if (bytes_written == -1) {
                ACE_ERROR((LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("send")));
                return false;
            }
        } else {
           if (verbose) yDebug("The input command bottle contains an empty string. \n");
           return false;
        }
    } else {
        if (verbose) yDebug("The input command bottle is empty. \n");
        return false;
    }

    return true;
}

bool SerialDeviceDriver::send(char *msg, size_t size)
{
    if (size > 0) {
        if (verbose)
        {
            yDebug("Sending string: %s", msg);
        }

        // Write message in the serial device
        ssize_t bytes_written = _send_serial_dev.send_n((void *)msg, size);

        if (bytes_written == -1) {
            ACE_ERROR((LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("send")));
            return false;
        }
    } else {
        if (verbose) yDebug("The input message is empty. \n");
        return false;
    }

    return true;
}

int SerialDeviceDriver::receiveChar(char& c)
{
    char chr;

    //this function call blocks
    ssize_t bytes_read = _serial_dev.recv ((void *) &chr, 1);

    if (bytes_read == -1)
    {
        ACE_ERROR((LM_ERROR, ACE_TEXT ("Error in SerialDeviceDriver::receive(). \n")));
        return 0;
    }

    if (bytes_read == 0)
    {
        return 0;
    }

    c=chr;
    return 1;
}

int  SerialDeviceDriver::flush()
{
    char chr;
    int count=0;
    ssize_t bytes_read=0;
    do
    {
        bytes_read = _serial_dev.recv ((void *) &chr, 1);
        count+=bytes_read;
    }
    while (bytes_read>0);
    return count;
}

int SerialDeviceDriver::receiveLine(char* buffer, const int MaxLineLength)
{
    int i;
    for (i = 0; i < MaxLineLength -1; ++i)
    {
        char recv_ch;
        int n = receiveChar(recv_ch);
        if (n <= 0)
        {
            return 0;
        }
        if ((recv_ch == line_terminator_char1) || (recv_ch == line_terminator_char2))
        {
            buffer[i] = recv_ch;
            i++;
            break;
        }
        buffer[i] = recv_ch;
    }
    buffer[i] = '\0';
    return i;
}

bool SerialDeviceDriver::receive(Bottle& msg)
{
    const int msgSize = 1001;
    char message[1001];

    //reading from socket.
    ssize_t bytes_read = _serial_dev.recv ((void *) message, msgSize - 1);
    
   /* conditionMutex.lock();
>>>>>>> 72fe732... Worked on the non-blocking solution
    if (shouldStop) {
        stopAck = true;
        stopCondition.signal();
    }
    conditionMutex.release();
<<<<<<< HEAD
    if (shouldStop) return true;

    if (bytes_read == -1 && errno != ETIME) {
=======
    if (shouldStop) return true;*/

    if (bytes_read == -1) {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return true;

        ACE_ERROR((LM_ERROR, ACE_TEXT ("Error in SerialDeviceDriver::receive(). \n")));
        return false;
    }

    if (bytes_read == 0)  //nothing there
        return true;

    message[bytes_read] = 0;

    if (verbose) {
        yDebug("Data received from serial device: %s",message);
        if (message[bytes_read - 1] != '\n') {    // Add \n only if reply does not contain it already
            ACE_OS::printf("\n");
        }
    }


    // Put message in the bottle
    msg.addString(message);

    return true;
}
