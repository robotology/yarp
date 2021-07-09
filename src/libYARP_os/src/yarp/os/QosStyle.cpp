/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/QosStyle.h>
#include <yarp/os/Vocab.h>

#include <cstdlib>
#include <string>


yarp::os::QosStyle::QosStyle() :
        threadPriority(-1),
        threadPolicy(-1),
        packetPriority(-1)
{
}

void yarp::os::QosStyle::setPacketPriorityByDscp(PacketPriorityDSCP dscp)
{
    if (dscp != DSCP_Invalid && dscp != DSCP_Undefined) {
        packetPriority = ((int)dscp) << 2;
    } else {
        packetPriority = -1;
    }
}


void yarp::os::QosStyle::setPacketPriorityByLevel(PacketPriorityLevel level)
{
    if (level != PacketPriorityInvalid && level != PacketPriorityUndefined) {
        packetPriority = ((int)level) << 2;
    } else {
        packetPriority = -1;
    }
}

bool yarp::os::QosStyle::setPacketPriority(const std::string& priority)
{
    size_t p = priority.find(':');
    if (p == std::string::npos) {
        return false;
    }

    std::string key = priority.substr(0, p);
    std::string value = priority.substr(p + 1);
    if (key.length() <= 0 || value.length() <= 0) {
        return false;
    }

    if (key == "LEVEL") {
        PacketPriorityLevel level = getLevelByVocab(yarp::os::Vocab32::encode(value));
        if (level != PacketPriorityInvalid && level != PacketPriorityUndefined) {
            setPacketPriorityByLevel(level);
            return true;
        }
    } else if (key == "DSCP") {
        PacketPriorityDSCP dscp = getDSCPByVocab(yarp::os::Vocab32::encode(value));
        if (dscp != DSCP_Invalid && dscp != DSCP_Undefined) {
            setPacketPriorityByDscp(dscp);
            return true;
        }
    } else if (key == "TOS") {
        char* p;
        int tos = strtol(value.c_str(), &p, 10);
        if (p == nullptr && tos >= 0) {
            setPacketPrioritybyTOS(tos);
            return true;
        }
    }

    packetPriority = -1;
    return false;
}

yarp::os::QosStyle::PacketPriorityDSCP yarp::os::QosStyle::getPacketPriorityAsDSCP() const
{
    switch (packetPriority) {
    case -1:
        return DSCP_Invalid;
    case (0 << 2):
        return DSCP_CS0;
    case (8 << 2):
        return DSCP_CS1;
    case (16 << 2):
        return DSCP_CS2;
    case (24 << 2):
        return DSCP_CS3;
    case (32 << 2):
        return DSCP_CS4;
    case (40 << 2):
        return DSCP_CS5;
    case (48 << 2):
        return DSCP_CS6;
    case (56 << 2):
        return DSCP_CS7;
    case (10 << 2):
        return DSCP_AF11;
    case (12 << 2):
        return DSCP_AF12;
    case (14 << 2):
        return DSCP_AF13;
    case (18 << 2):
        return DSCP_AF21;
    case (20 << 2):
        return DSCP_AF22;
    case (22 << 2):
        return DSCP_AF23;
    case (26 << 2):
        return DSCP_AF31;
    case (28 << 2):
        return DSCP_AF32;
    case (30 << 2):
        return DSCP_AF33;
    case (34 << 2):
        return DSCP_AF41;
    case (36 << 2):
        return DSCP_AF42;
    case (38 << 2):
        return DSCP_AF43;
    case (44 << 2):
        return DSCP_VA;
    case (46 << 2):
        return DSCP_EF;
    default:
        return DSCP_Undefined;
    }
}

yarp::os::QosStyle::PacketPriorityLevel yarp::os::QosStyle::getPacketPriorityAsLevel() const
{
    switch (packetPriority) {
    case -1:
        return PacketPriorityInvalid;
    case (0 << 2):
        return PacketPriorityNormal;
    case (10 << 2):
        return PacketPriorityLow;
    case (36 << 2):
        return PacketPriorityHigh;
    case (44 << 2):
        return PacketPriorityCritical;
    default:
        return PacketPriorityUndefined;
    }
}


// |           |     Class 1    |     Class 2    |     Class 3    |     Class 4    |
// |-----------|----------------|----------------|----------------|----------------|
// | Low Drop  | AF11 (DSCP 10) | AF21 (DSCP 18) | AF31 (DSCP 26) | AF41 (DSCP 34) |
// | Med Drop  | AF12 (DSCP 12) | AF22 (DSCP 20) | AF32 (DSCP 28) | AF42 (DSCP 36) |
// | High Drop | AF13 (DSCP 14) | AF23 (DSCP 22) | AF33 (DSCP 30) | AF43 (DSCP 38) |
yarp::os::QosStyle::PacketPriorityDSCP yarp::os::QosStyle::getDSCPByVocab(yarp::conf::vocab32_t vocab)
{
    switch (vocab) {
    case yarp::os::createVocab32('C', 'S', '0'):
        return DSCP_CS0;
    case yarp::os::createVocab32('C', 'S', '1'):
        return DSCP_CS1;
    case yarp::os::createVocab32('C', 'S', '2'):
        return DSCP_CS2;
    case yarp::os::createVocab32('C', 'S', '3'):
        return DSCP_CS3;
    case yarp::os::createVocab32('C', 'S', '4'):
        return DSCP_CS4;
    case yarp::os::createVocab32('C', 'S', '5'):
        return DSCP_CS5;
    case yarp::os::createVocab32('C', 'S', '6'):
        return DSCP_CS6;
    case yarp::os::createVocab32('C', 'S', '7'):
        return DSCP_CS7;
    case yarp::os::createVocab32('A', 'F', '1', '1'):
        return DSCP_AF11;
    case yarp::os::createVocab32('A', 'F', '1', '2'):
        return DSCP_AF12;
    case yarp::os::createVocab32('A', 'F', '1', '3'):
        return DSCP_AF13;
    case yarp::os::createVocab32('A', 'F', '2', '1'):
        return DSCP_AF21;
    case yarp::os::createVocab32('A', 'F', '2', '2'):
        return DSCP_AF22;
    case yarp::os::createVocab32('A', 'F', '2', '3'):
        return DSCP_AF23;
    case yarp::os::createVocab32('A', 'F', '3', '1'):
        return DSCP_AF31;
    case yarp::os::createVocab32('A', 'F', '3', '2'):
        return DSCP_AF32;
    case yarp::os::createVocab32('A', 'F', '3', '3'):
        return DSCP_AF33;
    case yarp::os::createVocab32('A', 'F', '4', '1'):
        return DSCP_AF41;
    case yarp::os::createVocab32('A', 'F', '4', '2'):
        return DSCP_AF42;
    case yarp::os::createVocab32('A', 'F', '4', '3'):
        return DSCP_AF43;
    case yarp::os::createVocab32('V', 'A'):
        return DSCP_VA;
    case yarp::os::createVocab32('E', 'F'):
        return DSCP_EF;
    default:
        return DSCP_Invalid;
    };
}


yarp::os::QosStyle::PacketPriorityLevel yarp::os::QosStyle::getLevelByVocab(yarp::conf::vocab32_t vocab)
{
    switch (vocab) {
    case yarp::os::createVocab32('N', 'O', 'R', 'M'):
        return PacketPriorityNormal;
    case yarp::os::createVocab32('L', 'O', 'W'):
        return PacketPriorityLow;
    case yarp::os::createVocab32('H', 'I', 'G', 'H'):
        return PacketPriorityHigh;
    case yarp::os::createVocab32('C', 'R', 'I', 'T'):
        return PacketPriorityCritical;
    default:
        return PacketPriorityInvalid;
    }
}
