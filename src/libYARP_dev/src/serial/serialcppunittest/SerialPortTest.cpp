// SerialPortTest.cpp

#include <cppunit/config/SourcePrefix.h>
#include <ace/OS.h>
#include <ace/OS_main.h>
#include "SerialPortTest.h"

// Registers the fixture into the 'registry'
//CPPUNIT_TEST_SUITE_REGISTRATION( SerialPortTest);

///BHand bh;

////SerialHandler serialhandler;
////if (serialhandler.initialize (argc, argv) == -1)
////    return -1;

void SerialPortTest::setUp()
{
    //serialhandler = new SerialHandler();
}

void SerialPortTest::tearDown()
{
    //delete serialhandler;
}

void SerialPortTest::testInitialize()
{
    int result = 0;
    result = serialhandler.initialize(0, NULL);
    CPPUNIT_ASSERT( result != -1); 
}

void SerialPortTest::testSerialComms()
{
    testSerialMessage("TEST=>",6);
    testSerialMessage("hola\0",5);
    testSerialMessage("\n\0\ttest",7);
    testSerialMessage("3444333222244455533322211\n\n\n",28);
    testSerialMessage("TEST=>TEST",10);
    char testmessage[7]; 
    testmessage[0] = 0x38;
    testmessage[1] = 0x10;
    testmessage[2] = 0x12;
    testmessage[3] = 0x13;
    testmessage[4] = 0x16;
    testmessage[5] = 0x17;
    testmessage[6] = 0x20;
    testSerialMessage(testmessage,7);
}

void SerialPortTest::testSerialMessage(char * message, int message_size)
{
    int error = 0;

    ACE_Message_Block * complete_message_block= 0;
    ACE_NEW_NORETURN( complete_message_block, ACE_Message_Block(message_size+1));
    CPPUNIT_ASSERT(complete_message_block!= NULL);

    ACE_Message_Block * message_block = 0;
    ACE_NEW_NORETURN( message_block, ACE_Message_Block(message_size+1));
    CPPUNIT_ASSERT( message_block != NULL);

    if (message_block->copy(message, message_size)) 
        ACE_ERROR ((LM_ERROR,"%p%l", 
                ACE_TEXT ("%I%N%l Error coping user message block\n")));

    SerialFeedbackData * feedback_data = 0;
    ACE_NEW_NORETURN( feedback_data, SerialFeedbackData());
    CPPUNIT_ASSERT(feedback_data != NULL);
    feedback_data->setCommandSender(this);

    // Setting the delimiter.
    //if ( _delimiter_string != 0 ) 
     //   feedback_data->setSerialResponseDelimiter((char *)_delimiter_string->c_str());
    //else if ( usingNulDelimiter )
    //feedback_data->setSerialResponseDelimiter("=>");
    ///feedback_data->setSerialResponseDelimiterNul();

    ACE_Message_Block * pointer_block = 0;
    ACE_NEW_NORETURN ( pointer_block, ACE_Message_Block( ACE_reinterpret_cast(
                char *, feedback_data)));
    CPPUNIT_ASSERT( pointer_block != NULL);

    //glue both block message_block and pointer_block
    message_block->cont(pointer_block);

    //Write the message
    error = serialhandler.putq(message_block);
    CPPUNIT_ASSERT( error != -1);
    ACE_Message_Block * response_block;

    do{
        error = this->getq(response_block);
        CPPUNIT_ASSERT(error != -1);
        error = complete_message_block->copy( response_block->rd_ptr(),
            response_block->length());
        CPPUNIT_ASSERT(error != -1);
        response_block->release();
    }while (complete_message_block->length() < message_size);
    
    CPPUNIT_ASSERT( complete_message_block->length() == message_size);

    error = strncmp(message,complete_message_block->rd_ptr(),message_size);
    CPPUNIT_ASSERT( error == 0);
    error = memcmp(message,complete_message_block->rd_ptr(),message_size);
    CPPUNIT_ASSERT( error == 0);
    complete_message_block->release();
}
int SerialPortTest::svc()
{
    return 1;
}

