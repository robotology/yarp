/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2007 Alexandre Bernardino
 * SPDX-FileCopyrightText: 2007 Carlos Beltran-Gonzalez
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SerialDeviceDriver.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>

#include <cstdio>
#include <cstdlib>

using namespace yarp::os;
using namespace yarp::dev;

#define MAX_FLUSHED_BYTES 10000

//inline SerialHandler& RES(void *res) { return *(SerialHandler *)res; }

namespace {
YARP_LOG_COMPONENT(SERIALPORT, "yarp.device.serialport")
}

SerialDeviceDriver::SerialDeviceDriver() {
    //system_resources = (SerialHandler*) new SerialHandler();
    verbose=false;
    line_terminator_char1 = '\r';
    line_terminator_char2 = '\n';
}

SerialDeviceDriver::~SerialDeviceDriver() {
    close();
}

bool SerialDeviceDriver::open(SerialDeviceDriverSettings& config)
{
    //if(RES(system_resources).initialize(config.CommChannel, config.SerialParams) < 0)
    //    return false;
    //RES(system_resources).setCommandSender(this);
    //yCTrace(SERIALPORT, "SerialHandler::initialize");
    yCInfo(SERIALPORT, "Starting Serial Port in %s \n", config.CommChannel);

    // Initialize serial port
    if(_serialConnector.connect(_serial_dev, ACE_DEV_Addr(config.CommChannel)) == -1)
    {
        yCError(SERIALPORT, "Invalid communications port in %s: %s\n", config.CommChannel, strerror(errno));
        return false;
    }


    // Set TTY_IO parameter into the ACE_TTY_IO device(_serial_dev)
    if (_serial_dev.control (ACE_TTY_IO::SETPARAMS, &config.SerialParams) == -1)
    {
        yCError(SERIALPORT, "Can not control communications port %s \n", config.CommChannel);
        return false;
    }

    return true;
}


bool SerialDeviceDriver::open(yarp::os::Searchable& config) {
    SerialDeviceDriverSettings config2;
    strcpy(config2.CommChannel, config.check("comport",Value("COM3"),"name of the serial channel").asString().c_str());
    this->verbose = (config.check("verbose",Value(1),"Specifies if the device is in verbose mode (0/1).").asInt32())>0;
    config2.SerialParams.baudrate = config.check("baudrate",Value(9600),"Specifies the baudrate at which the communication port operates.").asInt32();
    config2.SerialParams.xonlim = config.check("xonlim",Value(0),"Specifies the minimum number of bytes in input buffer before XON char is sent. Negative value indicates that default value should be used (Win32)").asInt32();
    config2.SerialParams.xofflim = config.check("xofflim",Value(0),"Specifies the maximum number of bytes in input buffer before XOFF char is sent. Negative value indicates that default value should be used (Win32). ").asInt32();
    //RANDAZ: as far as I undesrood, the exit condition for recv() function is NOT readmincharacters || readtimeoutmsec. It is readmincharacters && readtimeoutmsec.
    //On Linux. if readmincharacters params is set !=0, recv() may still block even if readtimeoutmsec is expired.
    //On Win32, for unknown reason, readmincharacters seems to be ignored, so recv () returns after readtimeoutmsec. Maybe readmincharacters is used if readtimeoutmsec is set to -1?
    config2.SerialParams.readmincharacters = config.check("readmincharacters",Value(1),"Specifies the minimum number of characters for non-canonical read (POSIX).").asInt32();
    config2.SerialParams.readtimeoutmsec = config.check("readtimeoutmsec",Value(100),"Specifies the time to wait before returning from read. Negative value means infinite timeout.").asInt32();
    // config2.SerialParams.parityenb = config.check("parityenb",Value(0),"Enable/disable parity checking.").asInt32();
    std::string temp = config.check("paritymode",Value("EVEN"),"Specifies the parity mode (EVEN, ODD, NONE). POSIX supports even and odd parity. Additionally Win32 supports mark and space parity modes.").asString();
    config2.SerialParams.paritymode = temp.c_str();
    config2.SerialParams.ctsenb = config.check("ctsenb",Value(0),"Enable & set CTS mode. Note that RTS & CTS are enabled/disabled together on some systems (RTS/CTS is enabled if either <code>ctsenb</code> or <code>rtsenb</code> is set).").asInt32();
    config2.SerialParams.rtsenb = config.check("rtsenb",Value(0),"Enable & set RTS mode. Note that RTS & CTS are enabled/disabled together on some systems (RTS/CTS is enabled if either <code>ctsenb</code> or <code>rtsenb</code> is set).\n- 0 = Disable RTS.\n- 1 = Enable RTS.\n- 2 = Enable RTS flow-control handshaking (Win32).\n- 3 = Specifies that RTS line will be high if bytes are available for transmission.\nAfter transmission RTS will be low (Win32).").asInt32();
    config2.SerialParams.xinenb = config.check("xinenb",Value(0),"Enable/disable software flow control on input.").asInt32();
    config2.SerialParams.xoutenb = config.check("xoutenb",Value(0),"Enable/disable software flow control on output.").asInt32();
    config2.SerialParams.modem = config.check("modem",Value(0),"Specifies if device is a modem (POSIX). If not set modem status lines are ignored. ").asInt32();
    config2.SerialParams.rcvenb = config.check("rcvenb",Value(0),"Enable/disable receiver (POSIX).").asInt32();
    config2.SerialParams.dsrenb = config.check("dsrenb",Value(0),"Controls whether DSR is disabled or enabled (Win32).").asInt32();
    config2.SerialParams.dtrdisable = config.check("dtrdisable",Value(0),"Controls whether DTR is disabled or enabled.").asInt32();
    config2.SerialParams.databits = config.check("databits",Value(7),"Data bits. Valid values 5, 6, 7 and 8 data bits. Additionally Win32 supports 4 data bits.").asInt32();
    config2.SerialParams.stopbits = config.check("stopbits",Value(1),"Stop bits. Valid values are 1 and 2.").asInt32();

    if (config.check("line_terminator_char1", "line terminator character for receiveLine(), default '\r'")) {
        line_terminator_char1 = config.find("line_terminator_char1").asInt32();
    }

    if (config.check("line_terminator_char2", "line terminator character for receiveLine(), default '\n'")) {
        line_terminator_char2 = config.find("line_terminator_char2").asInt32();
    }

    return open(config2);
}

bool SerialDeviceDriver::close() {
    _serial_dev.close();
    return true;
}

ReturnValue SerialDeviceDriver::setDTR(bool value)
{
    ACE_TTY_IO::Serial_Params arg;
    int ret = _serial_dev.control(_serial_dev.GETPARAMS, &arg);
    if (ret == -1) {
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    arg.dtrdisable = !value;
    ret = _serial_dev.control(_serial_dev.SETPARAMS, &arg);
    if (ret == -1) {
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    return ReturnValue_ok;
}

ReturnValue SerialDeviceDriver::sendString(const std::string& msg)
{
    if (msg.size() > 0)
    {
        if (verbose)
        {
            yCDebug(SERIALPORT, "Sending string: %s", msg.c_str());
        }

        // Write message to the serial device
        ssize_t bytes_written = _serial_dev.send_n((void*)msg.c_str(), msg.size());

        if (bytes_written == -1)
        {
            yCError(SERIALPORT, "Unable to write to serial port");
            return ReturnValue::return_code::return_value_error_method_failed;
        }
    }
    else
    {
        if (verbose) {
            yCDebug(SERIALPORT, "The input command bottle contains an empty string.");
        }
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    return ReturnValue_ok;
}

ReturnValue SerialDeviceDriver::sendByte(unsigned char byt)
{
    ssize_t bytes_written = _serial_dev.send_n((void*)byt, 1);
    if (bytes_written == -1)
    {
        yCError(SERIALPORT, "Unable to write to serial port");
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    return ReturnValue_ok;
}

ReturnValue SerialDeviceDriver::sendBytes(const std::vector<unsigned char>& msg)
{
    if (msg.size() > 0)
    {
        if (verbose)
        {
            yCDebug(SERIALPORT, "Sending string: %s", msg);
        }

        // Write message in the serial device
        ssize_t bytes_written = _serial_dev.send_n((void*)msg.data(), msg.size());

        if (bytes_written == -1)
        {
            yCError(SERIALPORT, "Unable to write to serial port");
            return ReturnValue::return_code::return_value_error_method_failed;
        }
    }
    else
    {
        if (verbose) {
            yCDebug(SERIALPORT, "The input message is empty. \n");
        }
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    return ReturnValue_ok;
}

ReturnValue SerialDeviceDriver::receiveByte(unsigned char& c)
{
    char chr;

    //this function call blocks
    ssize_t bytes_read = _serial_dev.recv ((void *) &chr, 1);

    if (bytes_read == -1)
    {
        yCError(SERIALPORT, "Error in SerialDeviceDriver::receive()");
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    if (bytes_read == 0)
    {
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    c=chr;
    return ReturnValue_ok;
}

ReturnValue SerialDeviceDriver::flush(size_t& flushed)
{
    char chr[100];
    flushed = 0;
    ssize_t bytes_read = 0;
    do {
        bytes_read = _serial_dev.recv((void*)&chr, 100);
        flushed += bytes_read;
        if (flushed > MAX_FLUSHED_BYTES) {
            break; // to prevent endless loop
        }
    } while (bytes_read > 0);
    return ReturnValue_ok;
}

ReturnValue  SerialDeviceDriver::flush()
{
    size_t dummy_counter = 0;
    return flush(dummy_counter);
}

ReturnValue SerialDeviceDriver::receiveBytes(std::vector<unsigned char>& bytes, const int MaxSize)
{
    char* buffer = new char[MaxSize];

    // this function call blocks
    ssize_t bytes_read = _serial_dev.recv((void*)buffer, MaxSize);

    bytes.clear();
    bytes.resize(bytes_read);
    for (size_t i = 0; i < bytes_read; bytes_read++)
    {
        bytes[i] = buffer[i];
    }

    delete[] buffer;
    return ReturnValue_ok;
}

ReturnValue SerialDeviceDriver::receiveLine(std::vector<char>& line, const int MaxLineLength)
{
    int i;
    for (i = 0; i < MaxLineLength -1; ++i)
    {
        char recv_ch;
        int n = receiveChar(recv_ch);
        if (n <= 0)
        {
            //this invalidates the whole line, because no line terminator \n was found
            return 0;

            //use this commented code here if you do NOT want to invalidate the line
            //buffer[i] = '\0';
            //return i;
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

ReturnValue SerialDeviceDriver::receiveString(std::string& msg)
{
    const int msgSize = 1001;
    char message[1001];

    //this function call blocks
    ssize_t bytes_read = _serial_dev.recv ((void *) message, msgSize - 1);

    if (bytes_read == -1)
    {
        yCError(SERIALPORT, "Error in SerialDeviceDriver::receive()");
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    if (bytes_read == 0)
    { //nothing there
        return ReturnValue_ok;
    }

    message[bytes_read] = 0;

    if (verbose)
    {
        yCDebug(SERIALPORT, "Data received from serial device: %s", message);
    }


    // Return the message
    msg=message;

    return ReturnValue_ok;
}
