/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_VOCAB32_H
#define YARP_OS_VOCAB32_H

#include <yarp/os/NetInt32.h>

#include <string>

namespace yarp::os {

// We need a constexpr for efficient switching.
/**
 * Create a vocab from chars.
 *
 * Use as, for example, yarp::os::createVocab32('s','e','t')
 *
 * @param a first character of the vocab
 * @param b second character of the vocab
 * @param c third character of the vocab
 * @param d fourth character of the vocab
 */
constexpr yarp::conf::vocab32_t createVocab32(char a, char b = 0, char c = 0, char d = 0)
{
    return (static_cast<yarp::conf::vocab32_t>(a))       +
           (static_cast<yarp::conf::vocab32_t>(b) << 8)  +
           (static_cast<yarp::conf::vocab32_t>(c) << 16) +
           (static_cast<yarp::conf::vocab32_t>(d) << 24);
}

/**
 * Short readable codes.  They are integers, for efficient switching,
 * but have a readable (and writable) string representation.
 * Codes are limited to 4 characters long (to fit in a standard
 * network integer, NetInt32).
 * This is a compromise to allow the creation of messages that
 * can be parsed very efficiently by machine but nevertheless are
 * human readable and writable.
 * \code
 *   switch(code) {
 *      case createVocab('s','e','t'): // switch on "set"
 *          ...
 *          break;
 *      case createVocab('s','t','o','p'): // switch on "stop"
 *          ...
 *          break;
 *   }
 * \endcode
 * Your program will be efficient, and the codes used if they
 * traverse the network will be human readable/writable.
 */
namespace Vocab32 {

/**
 * Convert a string into a vocabulary identifier.  If the string
 * is longer than four characters, only the first four characters
 * are used.
 * @param str the string to convert
 * @result the integer equivalent of the string form of the identifier
 */
YARP_os_API NetInt32 encode(const std::string& str);

/**
 * Convert a vocabulary identifier into a string.
 * @param code the vocabulary identifier to convert
 * @result the string equivalent of the integer form of the identifier
 */
YARP_os_API std::string decode(NetInt32 code);

} // namespace Vocab32

} // namespace yarp::os

#endif // YARP_OS_VOCAB32_H
