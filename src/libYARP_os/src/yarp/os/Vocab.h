/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_VOCAB_H
#define YARP_OS_VOCAB_H

#include <yarp/os/NetInt32.h>

#include <string>

namespace yarp {
namespace os {

// We need a constexpr for efficient switching.
// Use as, for example, createVocab('s','e','t')
constexpr yarp::conf::vocab32_t createVocab(char a, char b = 0, char c = 0, char d = 0)
{
    return ((yarp::conf::vocab32_t)a)       +
           ((yarp::conf::vocab32_t)b << 8)  +
           ((yarp::conf::vocab32_t)c << 16) +
           ((yarp::conf::vocab32_t)d << 24);
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
namespace Vocab {

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

} // namespace Vocab
} // namespace os
} // namespace yarp

#ifndef YARP_NO_DEPRECATED // since YARP 3.1.0
YARP_DEPRECATED_MSG("Use yarp::os::createVocab() instead")
constexpr int32_t VOCAB(char a, char b, char c, char d)
{
    return yarp::os::createVocab(a, b, c, d);
}
YARP_DEPRECATED_MSG("Use yarp::os::createVocab() instead")
constexpr int32_t VOCAB4(char a, char b, char c, char d)
{
    return yarp::os::createVocab(a, b, c, d);
}
YARP_DEPRECATED_MSG("Use yarp::os::createVocab() instead")
constexpr int32_t VOCAB3(char a, char b, char c)
{
    return yarp::os::createVocab(a, b, c, 0);
}
YARP_DEPRECATED_MSG("Use yarp::os::createVocab() instead")
constexpr int32_t VOCAB2(char a, char b)
{
    return yarp::os::createVocab(a, b, 0, 0);
}
YARP_DEPRECATED_MSG("Use yarp::os::createVocab() instead")
constexpr int32_t VOCAB1(char a)
{
    return yarp::os::createVocab(a, 0, 0, 0);
}
#endif // YARP_NO_DEPRECATED

#endif // YARP_OS_VOCAB_H
