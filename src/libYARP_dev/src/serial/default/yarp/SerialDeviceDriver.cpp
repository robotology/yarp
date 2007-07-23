// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alex Bernardino, Carlos Beltran
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/SerialDeviceDriver.h>

#include <stdio.h>
#include <stdlib.h>

#include <yarp/os/Time.h>

using namespace yarp::os;
using namespace yarp::dev;

#include "SerialHandler.h"
#include "SerialFeedbackData.h"

inline SerialHandler& RES(void *res) { return *(SerialHandler *)res; }

SerialDeviceDriver::SerialDeviceDriver() {
    system_resources = (SerialHandler*) new SerialHandler();
}

SerialDeviceDriver::~SerialDeviceDriver() {
    close();
}


bool SerialDeviceDriver::open(SerialDeviceDriverSettings& config) 
{
    if(RES(system_resources).initialize(config.CommChannel, config.SerialParams) < 0)
        return false;
    return true;
}


bool SerialDeviceDriver::open(yarp::os::Searchable& config) {
    SerialDeviceDriverSettings config2;
    strcpy(config2.CommChannel, config.check("comport",Value("COM3"),"name of the serial channel").asString().c_str());
    config2.SerialParams.baudrate = config.check("baudrate",Value(9600),"Specifies the baudrate at which the communication port operates.").asInt();
    config2.SerialParams.xonlim = config.check("xonlim",Value(0),"Specifies the minimum number of bytes in input buffer before XON char is sent. Negative value indicates that default value should be used (Win32)").asInt();
    config2.SerialParams.xofflim = config.check("xofflim",Value(0),"Specifies the maximum number of bytes in input buffer before XOFF char is sent. Negative value indicates that default value should be used (Win32). ").asInt();
    config2.SerialParams.readmincharacters = config.check("readmincharacters",Value(1),"Specifies the minimum number of characters for non-canonical read (POSIX).").asInt();
    config2.SerialParams.readtimeoutmsec = config.check("readtimeoutmsec",Value(10000),"Specifies the time to wait before returning from read. Negative value means infinite timeout.").asInt();
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
    open(config2);
    return true;
}

bool SerialDeviceDriver::close(void) {
    return true;
}

bool SerialDeviceDriver::send(const Bottle& msg)
{
    int error = 0;

    ACE_OS::printf("Received string: %s\n", msg.toString().c_str());
    int message_size = msg.toString().length();
    ACE_Message_Block * message_block = 0;
    ACE_NEW_NORETURN( message_block, ACE_Message_Block(message_size+1));
    //CPPUNIT_ASSERT( message_block != NULL);

    if (message_block->copy(msg.toString().c_str(), message_size)) 
        ACE_ERROR ((LM_ERROR,"%p%l", 
                ACE_TEXT ("%I%N%l Error coping user message block\n")));

    SerialFeedbackData * feedback_data = 0;
    ACE_NEW_NORETURN( feedback_data, SerialFeedbackData());
    //CPPUNIT_ASSERT(feedback_data != NULL);
    feedback_data->setCommandSender(this);

    // Setting the delimiter.
    if ( feedback_data->_delimiter_string != 0 ) 
       feedback_data->setSerialResponseDelimiter(feedback_data->_delimiter_string);
    else if ( feedback_data->_use_nul_delimiter )
    feedback_data->setSerialResponseDelimiter("=>");
    feedback_data->setSerialResponseDelimiterNul();

    ACE_Message_Block * pointer_block = 0;
    ACE_NEW_NORETURN ( pointer_block, ACE_Message_Block( ACE_reinterpret_cast(
                char *, feedback_data)));
    //CPPUNIT_ASSERT( pointer_block != NULL);

    //glue both block message_block and pointer_block
    message_block->cont(pointer_block);

    //Write the message
    error = RES(system_resources).putq(message_block);
    //CPPUNIT_ASSERT( error != -1);
    return true;
} 

bool SerialDeviceDriver::receive(Bottle& msg)
{
    int error = 0;

    ACE_Message_Block * response_block;
    error = this->getq(response_block);
    //CPPUNIT_ASSERT(error != -1);

    ACE_Message_Block * complete_message_block= 0;
    ACE_NEW_NORETURN( complete_message_block, ACE_Message_Block(response_block->length()+1));
    //CPPUNIT_ASSERT(complete_message_block != NULL);
    error = complete_message_block->copy( response_block->rd_ptr(), response_block->length());
    //CPPUNIT_ASSERT(error != -1);
    response_block->release();
    //add a string terminator for the bottle
    complete_message_block->wr_ptr(response_block->length()+1);
    *(complete_message_block->wr_ptr()) = 0;
    //copy to bottle
    msg.addString(complete_message_block->rd_ptr());
    complete_message_block->release();
    return true;
}
