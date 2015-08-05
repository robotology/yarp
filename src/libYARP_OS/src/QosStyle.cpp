// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/QosStyle.h>


yarp::os::QosStyle::QosStyle() :
        threadPriority(-1),
        threadPolicy(-1),
        packetPriority(-1){
}

bool yarp::os::QosStyle::setPacketPriorityByDscp(const ConstString& dscp) {
    int value = yarp::os::QosStyle::getDSCPByVocab(yarp::os::Vocab::encode(dscp));
    if(value == -1)
        return false;
    packetPriority = value<<2;
    return true;
}


void yarp::os::QosStyle::setPacketPriorityByType(PacketPriorityType priority) {
    packetPriority = ((int)priority)<<2;
}



//                Class 1          Class 2         Class 3         Class 4
//          ------------------------------------------------------------------
// Low Drop	 | AF11 (DSCP 10)	AF21 (DSCP 18)	AF31 (DSCP 26)	AF41 (DSCP 34)
// Med Drop	 | AF12 (DSCP 12)	AF22 (DSCP 20)	AF32 (DSCP 28)	AF42 (DSCP 36)
// High Drop | AF13 (DSCP 14)	AF23 (DSCP 22)	AF33 (DSCP 30)	AF43 (DSCP 38)
int yarp::os::QosStyle::getDSCPByVocab(int vocab) {
    int dscp;
    switch(vocab) {
        case VOCAB3('C','S','0')    : dscp = 0; break;
        case VOCAB3('C','S','1')    : dscp = 8; break;
        case VOCAB3('C','S','2')    : dscp = 16; break;
        case VOCAB3('C','S','3')    : dscp = 24; break;
        case VOCAB3('C','S','4')    : dscp = 32; break;
        case VOCAB3('C','S','5')    : dscp = 40; break;
        case VOCAB3('C','S','6')    : dscp = 48; break;
        case VOCAB3('C','S','7')    : dscp = 56; break;
        case VOCAB4('A','F','1','1'): dscp = 10; break;
        case VOCAB4('A','F','1','2'): dscp = 12; break;
        case VOCAB4('A','F','1','3'): dscp = 14; break;
        case VOCAB4('A','F','2','1'): dscp = 18; break;
        case VOCAB4('A','F','2','2'): dscp = 20; break;
        case VOCAB4('A','F','2','3'): dscp = 22; break;
        case VOCAB4('A','F','3','1'): dscp = 26; break;
        case VOCAB4('A','F','3','2'): dscp = 28; break;
        case VOCAB4('A','F','3','3'): dscp = 30; break;
        case VOCAB4('A','F','4','1'): dscp = 34; break;
        case VOCAB4('A','F','4','2'): dscp = 36; break;
        case VOCAB4('A','F','4','3'): dscp = 38; break;
        case VOCAB2('V','A')        : dscp = 44; break;
        case VOCAB2('E','F')        : dscp = 46; break;
        default: dscp = -1;
    };
    return dscp;
}
