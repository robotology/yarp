/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006 Alexandre Bernardino
 * SPDX-FileCopyrightText: 2006 Carlos Beltran-Gonzalez
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ISERIALDEVICE_H
#define YARP_DEV_ISERIALDEVICE_H

#include <yarp/os/Bottle.h>
#include <yarp/dev/api.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/ReturnValue.h>


namespace yarp::dev {

/**
 * \ingroup dev_iface_other
 *
 * \brief A generic interface to serial port devices.
 */
class YARP_dev_API ISerialDevice
{
public:
    virtual ~ISerialDevice();

    /**
     * Sends a string of chars to the serial communications channel.
     * \param msg the string to send
     * \return true on success
     */
    virtual yarp::dev::ReturnValue sendString(const std::string& msg) = 0;

    /**
     * Sends a string of bytes to the serial communications channel.
     * \param msg the string to send
     * \return true on success
     */
    virtual yarp::dev::ReturnValue sendBytes(const std::vector<unsigned char>& line) = 0;

    /**
     * Gets the existing chars in the receive queue.
     * \param msg the received string
     * \return true on success; false if no messages available
     */
    virtual yarp::dev::ReturnValue receiveString(std::string& msg) = 0;

     /**
     * Send one single char to the serial device
     * \param chr the byte to be sent
     * @return true on success
     */
    virtual yarp::dev::ReturnValue sendByte(unsigned char chr) = 0;

    /**
     * Gets one single char from the receive queue.
     * \param chr the received char.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue receiveByte(unsigned char& chr) = 0;

    /**
    * Gets an array of bytes (unsigned char) with size <= 'size' parameter.
    * @param thee returned sequence of bytes.
    * @param MaxSize the maximum number of bytes that we want to read
    * @return true on success
    */
    virtual yarp::dev::ReturnValue receiveBytes(std::vector<unsigned char>& line, const int MaxSize) = 0;

    /**
    * Gets one line (a sequence of chars with a ending '\\n' or '\\r') from the receive queue.
    * The ending '\\n''\\r' chars are not removed in the returned line.
    * \param line the returned sequence of characters.
    * \param MaxLineLength the maximum number of chars that we want to read
    * @return true on success
    */
    virtual yarp::dev::ReturnValue receiveLine(std::vector<char>& line, const int MaxLineLength) = 0;

    /**
    * Enable/Disable DTR protocol
    * @param enable Enable/Disable DTR protocol
    * @return true on success
    */
    virtual yarp::dev::ReturnValue setDTR(bool enable) = 0;

    /**
     * Flushes the internal buffer.
     * \param flushed the number of flushed characters.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue flush(size_t& flushed_chars) = 0;

    /**
     * Flushes the internal buffer.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue flush() = 0;
};

} // namespace yarp::dev

#endif // YARP_DEV_ISERIALDEVICE_H
