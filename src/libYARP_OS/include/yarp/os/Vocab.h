/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_OS_VOCAB_H
#define YARP_OS_VOCAB_H

#include <yarp/os/NetInt32.h>
#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        class Vocab;
    }
}

// We need a macro for efficient switching.
// Use as, for example, VOCAB('s','e','t')
#define VOCAB(a,b,c,d) ((((int)(d))<<24)+(((int)(c))<<16)+(((int)(b))<<8)+((int)(a)))
#define VOCAB4(a,b,c,d) VOCAB((a),(b),(c),(d))
#define VOCAB3(a,b,c) VOCAB((a),(b),(c),(0))
#define VOCAB2(a,b) VOCAB((a),(b),(0),(0))
#define VOCAB1(a) VOCAB((a),(0),(0),(0))

/**
 * Short readable codes.  They are integers, for efficient switching,
 * but have a readable (and writable) string representation.
 * Codes are limited to 4 characters long (to fit in a standard
 * network integer, NetInt32).
 * This is a compromise to allow the creation of messages that
 * can be parsed very efficiently by machine but nevertheless are
 * human readable and writable.
 * When switching on a Vocab code, we suggest you use the
 * VOCABn macro defined in yarp/os/Vocab.h.
 * \code
 *   switch(code) {
 *      case VOCAB3('s','e','t'): // switch on "set"
 *          ...
 *          break;
 *      case VOCAB4('s','t','o','p'): // switch on "stop"
 *          ...
 *          break;
 *   }
 * \endcode
 * Your program will be efficient, and the codes used if they
 * traverse the network will be human readable/writable.
 *
 */
class YARP_OS_API yarp::os::Vocab {
public:
    /**
     * Convert a string into a vocabulary identifier.  If the string
     * is longer than four characters, only the first four characters
     * are used.
     * @param str the string to convert
     * @result the integer equivalent of the string form of the identifier
     */
    static NetInt32 encode(const ConstString& str);

    /**
     * Convert a vocabulary identifier into a string.
     * @param code the vocabulary identifier to convert
     * @result the string equivalent of the integer form of the identifier
     */
    static ConstString decode(NetInt32 code);
};

#endif // YARP_OS_VOCAB_H
