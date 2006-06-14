// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef _YARP2_VOCAB_
#define _YARP2_VOCAB_

#include <yarp/os/NetInt32.h>
#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        class Vocab;
    }
}

// We need a macro for efficient switching.
// Use as, for example, VOCAB('s','e','t')
#define VOCAB(a,b,c,d) ((((yarp::os::NetInt32)(d))<<24)+(((yarp::os::NetInt32)(c))<<16)+(((yarp::os::NetInt32)(b))<<8)+((yarp::os::NetInt32)(a)))
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
class yarp::os::Vocab {
public:
    static NetInt32 encode(const char *str);
    static ConstString decode(NetInt32 code);
};

#endif
