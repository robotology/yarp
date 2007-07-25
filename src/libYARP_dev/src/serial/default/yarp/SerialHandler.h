/* vim:set tw=78: set sw=4: set ts=4: */

/*
 * Copyright (C) 2007 Carlos Beltran-Gonzalez
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/*
 * RCS-ID:$Id: SerialHandler.h,v 1.2 2007-07-25 16:08:47 beltran Exp $
 */
#ifndef __SERIALHANDLERH__
#define __SERIALHANDLERH__

#include <ace/streams.h>
#include <ace/Log_Msg.h>
#include <ace/OS.h>
#include <ace/Proactor.h>
#include <ace/Asynch_IO.h>
#include <ace/Asynch_IO_Impl.h>
#include <ace/Message_Block.h>
#include <ace/OS_main.h>
#include <ace/TTY_IO.h>
#include <ace/Task.h>
#include <ace/CDR_Stream.h>
#include <ace/DEV_Connector.h>
#include "../../DGSTask.h"
#include <ace/String_Base.h>
//#include "SerialFeedbackData.h"

/** 
 * @class SerialHandler
 *  Implements the Serial Adaptor
 */
class SerialHandler : public DGSTask, public ACE_Service_Handler
{

public:
    // Constructor and destructor
    SerialHandler (void);
    ~SerialHandler (void);

    /** 
     *  initialize starts the inizialization of the serial device and attach it
     * to the reading streams.
     * 
     * @return 
     */
    int initialize(int argc, ACE_TCHAR *argv[]);
    int initialize(const char* commchannel, ACE_TTY_IO::Serial_Params params);

    /** 
     *  svc Reads commands from the queue and writes the message block in the
     * serial device
     * @return 
     */
    virtual int svc();

    void setCommandSender (ACE_Task * command_sender){ _command_sender = command_sender;};

protected:

    /** 
     *  handle_read_stream this is called when asynchronous reads from the
     *  serial device is complete.
     * 
     * @param result 
     */
    virtual void handle_read_stream (const ACE_Asynch_Read_Stream::Result &result);

    /** 
     *  handle_write_stream This is called when asynchronous writes from the
     *  serial device is complete.
     * 
     * @param result 
     */
    virtual void handle_write_stream (const ACE_Asynch_Write_Stream::Result &result);

private:
    //int initiate_read_stream(void);
    // Initiate an asynchronous <read> operation on the socket

    /** 
     *  initiate_read_stream 
     * 
     * @return 
     */
    int initiate_read_stream (ACE_Message_Block *);

    /**
     *  Parameters.
     */

    /// The stream for reading from the serial line.
    ACE_Asynch_Read_Stream _serial_read_stream; 
    /// The stream for writing to the serial line.
    ACE_Asynch_Write_Stream _serial_write_stream;
    /// The TTY device for the serial line.
    ACE_TTY_IO _serial_dev; 
    /// The connector used to connect to the serial line.
    ACE_DEV_Connector _serialConnector;
    int flag;
    /// 
    int _baudrate;
    ACE_Task * _command_sender;

};
#endif /* __SERIALHANDLER__ */ 
