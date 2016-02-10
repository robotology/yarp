// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alex Bernardino, Carlos Beltran-Gonzalez, Francesco Giovannini (francesco.giovannini@iit.it)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef SerialDeviceDriverh
#define SerialDeviceDriverh

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/SerialInterfaces.h>
#include <yarp/os/Bottle.h>

#include <ace/DEV_Connector.h>
#include <ace/TTY_IO.h>
#include <ace/OS_NS_stdio.h>

namespace yarp {
    namespace dev {
        class SerialDeviceDriverSettings;
        class SerialDeviceDriver;
    }
}

using namespace yarp::os;

class yarp::dev::SerialDeviceDriverSettings 
{
public:
    char CommChannel[100]; // Contains the name of the com port "COM1", "COM2" (windows) or "/etc/stty0", "/dev/stty1" (linux), etc...
    ACE_TTY_IO::Serial_Params SerialParams;
    /** Serial_Params contains the following variables: */
    /** int baudrate; Specifies the baudrate at which the communication port operates. */
    /** int xonlim; Specifies the minimum number of bytes in input buffer before XON char
        is sent. Negative value indicates that default value should
        be used (Win32). */
    /** int xofflim; Specifies the maximum number of bytes in input buffer before XOFF char
        is sent. Negative value indicates that default value should
        be used (Win32). */
    /** unsigned int readmincharacters; Specifies the minimum number of characters for non-canonical
        read (POSIX). */
    /** int readtimeoutmsec; Specifies the time to wait before returning from read. Negative value
        means infinite timeout. */
    /** const char *paritymode; Specifies the parity mode (EVEN, ODD, NONE). POSIX supports "even" and "odd" parity.
        Additionally Win32 supports "mark" and "space" parity modes. */
    /** bool ctsenb; Enable & set CTS mode. Note that RTS & CTS are enabled/disabled
        together on some systems (RTS/CTS is enabled if either
        <code>ctsenb</code> or <code>rtsenb</code> is set). */
    /** int rtsenb; Enable & set RTS mode. Note that RTS & CTS are enabled/disabled
        together on some systems (RTS/CTS is enabled if either
        <code>ctsenb</code> or <code>rtsenb</code> is set).
        - 0 = Disable RTS.
        - 1 = Enable RTS.
        - 2 = Enable RTS flow-control handshaking (Win32).
        - 3 = Specifies that RTS line will be high if bytes are available
              for transmission. After transmission RTS will be low (Win32). */
    /** bool xinenb; Enable/disable software flow control on input. */
    /** bool xoutenb; Enable/disable software flow control on output. */
    /** bool modem; Specifies if device is a modem (POSIX). If not set modem status
        lines are ignored. */
    /** bool rcvenb; Enable/disable receiver (POSIX). */
    /** bool dsrenb; Controls whether DSR is disabled or enabled (Win32). */
    /** bool dtrdisable; Controls whether DTR is disabled or enabled. */
    /** unsigned char databits; Data bits. Valid values 5, 6, 7 and 8 data bits.
        Additionally Win32 supports 4 data bits. */
    /** unsigned char stopbits; Stop bits. Valid values are 1 and 2. */
};

/**
 * @ingroup dev_impl_media
 *
 * A basic Serial Communications Link (RS232, USB).
 * 
 */
class yarp::dev::SerialDeviceDriver : public DeviceDriver, public ISerialDevice
{
private:
    SerialDeviceDriver(const SerialDeviceDriver&);
    void operator=(const SerialDeviceDriver&);

    ACE_TTY_IO _serial_dev;
    ACE_DEV_Connector _serialConnector;
    bool verbose;     // If enabled (1), the data sent/received by the serial device is print on screen
    char line_terminator_char1;
    char line_terminator_char2;

public:
    SerialDeviceDriver();

    virtual ~SerialDeviceDriver();

    virtual bool open(yarp::os::Searchable& config);

    /**
     * Configures the device.
     * @param The serial device configuration object
     * @return true on success
     */
    bool open(SerialDeviceDriverSettings& config);

    virtual bool close(void);

    /**
     * Sends a string of chars to the serial communications channel.
     * @param msg the string to send
     * @return true on success
     */
    virtual bool send(const Bottle& msg);
    virtual bool send(char *msg, size_t size);
    //bool putMessage(Bottle& msg, bool waitreply, double replytimeout, Bottle& reply, char *replydelimiter, int replysize );
    /**
     * Gets the existing chars in the receive queue.
     * @param msg - the received string
     * @return - true on success; false if no messages available
     */
    virtual bool receive(Bottle& msg);
    /**
     * Gets one single char from the receive queue.
     * @param chr - the received char.
     * @return - 0 if no chars are received; 1 if one char is received.
     */
    virtual int  receiveChar(char& chr);

    /**
     * Gets an array of bytes (unsigned char) with size <= 'size' parameter. The array is NOT null terminated.
     * @param bytes - a previously allocated buffer where the received data is stored.
     * @param size - the size of the 'bytes' parameter.
     * @return - the number of received bytes. The function returns 0 if no bytes are received.
     */
    virtual int  receiveBytes(unsigned char* bytes, const int size);

    /**
    * Gets one line (a sequence of chars with a ending '\\n' or '\\r') from the receive queue. The ending '\\n''\\r' chars are not removed in the returned line.
    * \param line - a previously allocated buffer where the received line is stored.
    * \param MaxLineLength - the size of the 'line' parameter.
    * \return - the number of received characters (including the '\n''\r' chars, plus the buffer terminator '\\0'). The function returns 0 if no chars are received.
    */
    virtual int  receiveLine(char* line, const int MaxLineLength);

    /**
    * Enable/Disable DTR protocol
    * @param enable Enable/Disable DTR protocol
    * @return true on success
    */
    virtual bool setDTR(bool value);

    /**
     * Flushes the internal buffer.
     * @return - the number of flushed characters.
     */
    virtual int  flush();
};

#endif
