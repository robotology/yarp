// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alex Bernardino, Carlos Beltran-Gonzalez
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/SerialDeviceDriver.h>

#include <stdio.h>
#include <stdlib.h>

///#include <yarp/os/Time.h>

using namespace yarp::os;
using namespace yarp::dev;

//inline SerialHandler& RES(void *res) { return *(SerialHandler *)res; }

SerialDeviceDriver::SerialDeviceDriver() {
    //system_resources = (SerialHandler*) new SerialHandler();
}

SerialDeviceDriver::~SerialDeviceDriver() {
    close();
}

bool SerialDeviceDriver::open(SerialDeviceDriverSettings& config) 
{
    //if(RES(system_resources).initialize(config.CommChannel, config.SerialParams) < 0)
    //    return false;
    //RES(system_resources).setCommandSender(this);
    ACE_TRACE("SerialHandler::initialize");
    ACE_OS::printf("Starting Serial Port in %s \n", config.CommChannel);

    // Initialize serial port
    if(_serialConnector.connect(_serial_dev, ACE_DEV_Addr(config.CommChannel)) == -1)
    { 
        ACE_OS::printf("Invalid communications port in %s \n", config.CommChannel);
        return false;
    } 


    // Set TTY_IO parameter into the ACE_TTY_IO device(_serial_dev)
    if (_serial_dev.control (ACE_TTY_IO::SETPARAMS, &config.SerialParams) == -1)
    {
         ACE_OS::printf("Can not control communications port %s \n", config.CommChannel);
        return false;
    }

    return true;
}


bool SerialDeviceDriver::open(yarp::os::Searchable& config) {
    SerialDeviceDriverSettings config2;
    strcpy(config2.CommChannel, config.check("comport",Value("COM3"),"name of the serial channel").asString().c_str());
    config2.SerialParams.baudrate = config.check("baudrate",Value(9600),"Specifies the baudrate at which the communication port operates.").asInt();
    config2.SerialParams.xonlim = config.check("xonlim",Value(0),"Specifies the minimum number of bytes in input buffer before XON char is sent. Negative value indicates that default value should be used (Win32)").asInt();
    config2.SerialParams.xofflim = config.check("xofflim",Value(0),"Specifies the maximum number of bytes in input buffer before XOFF char is sent. Negative value indicates that default value should be used (Win32). ").asInt();
    config2.SerialParams.readmincharacters = config.check("readmincharacters",Value(1),"Specifies the minimum number of characters for non-canonical read (POSIX).").asInt();
    config2.SerialParams.readtimeoutmsec = config.check("readtimeoutmsec",Value(100),"Specifies the time to wait before returning from read. Negative value means infinite timeout.").asInt();
    config2.SerialParams.parityenb = config.check("parityenb",Value(0),"Enable/disable parity checking.").asInt();
    config2.SerialParams.paritymode = config.check("paritymode",Value("EVEN"),"Specifies the parity mode. POSIX supports even and odd parity. Additionally Win32 supports mark and space parity modes.").asString().c_str();
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
    return open(config2);
}

bool SerialDeviceDriver::close(void) {
    return true;
}

bool SerialDeviceDriver::send(const Bottle& msg)
{
    ACE_OS::printf("Received string: %s\n", msg.get(0).asString().c_str());
    int message_size = msg.get(0).asString().length();
    // Write message in the serial device
    ssize_t bytes_written = _serial_dev.send_n((void *)msg.get(0).asString().c_str(), message_size);

    if (bytes_written == -1)
        ACE_ERROR((LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("send")));
    return true;
} 

bool SerialDeviceDriver::send(char *msg, size_t size)
{
    ACE_OS::printf("Received string: %s\n", msg);
    // Write message in the serial device
    ssize_t bytes_written = _serial_dev.send_n((void *)msg, size);

    if (bytes_written == -1)
        ACE_ERROR((LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("send")));
    return true;
}

bool SerialDeviceDriver::receive(Bottle& msg)
{
    char message[1001];

    //this function call blocks
    ssize_t bytes_read = _serial_dev.recv ((void *) message, 1000);

    if (bytes_read == -1)
        ACE_ERROR((LM_ERROR, ACE_TEXT ("Error on SerialDeviceDriver : receive \n")));

    if (bytes_read == 0)  //nothing there
        return true;
        
    message[bytes_read] = 0;

    ACE_OS::printf("Datareceived in Serial DeviceDriver receive:#%s#\n",message);

    // Put message in the bottle
    msg.addString(message);

    return true;
}
