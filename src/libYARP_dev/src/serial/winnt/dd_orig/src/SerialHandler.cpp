/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file SerialHandler.cpp
 *  Contains the implementation of the Serial Handler
 * @version 1.0
 * @date 21-Jun-06 1:53:39 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 * @todo Write a correct method of finalization of the system.
 */

/*
 * $Id: SerialHandler.cpp,v 1.1 2007-05-30 22:47:18 alex_bernardino Exp $
 */
#include <string.h>
#include "SerialHandler.h"
#include <ace/Configuration.h>
#include <ace/Configuration_Import_Export.h>
#include <ace/Time_Value.h>
#include <ace/High_Res_Timer.h>
#include <iostream>
#include <string>

/** 
 *  SerialHandler::SerialHandler 
 */
SerialHandler::SerialHandler (void)
{
    ACE_TRACE("SerialHandler::SerialHandler");
}

/** 
 *  SerialHandler::~SerialHandler Destructor.
 * @todo close correctly all the streams
 */
SerialHandler::~SerialHandler (void)
{
    ACE_TRACE("SerialHandler::~SerialHandler");
}

/** 
 *  SerialHandler::initialize
 * @return 
 * @todo make serial initialization flexible allowing command line parameters
 * or reads from a configuration file.
 */
int SerialHandler::initialize(int argc, ACE_TCHAR *argv[]) 
{
  ACE_TRACE("SerialHandler::initialize");

  read_configuration_file("DGS_MCU_Driver");

  // Initialize serial port
  _serialConnector.connect(_serial_dev, ACE_DEV_Addr(_commchannel.c_str()), 0,
      ACE_Addr::sap_any, 0, O_RDWR|FILE_FLAG_OVERLAPPED); 

  // Set serial port parameters
  ACE_TTY_IO::Serial_Params myparams;
  myparams.baudrate   = _baudrate;
  myparams.xonlim     = 0;
  myparams.xofflim    = 0;
  myparams.parityenb  = true;
  myparams.paritymode = "FALSE";
  myparams.databits   = 8;
  myparams.stopbits   = 1;
  myparams.readmincharacters = 1;
  myparams.readtimeoutmsec   = 10000;

  // Set TTY_IO parameter into the ACE_TTY_IO device(_serial_dev)
  if (_serial_dev.control (ACE_TTY_IO::SETPARAMS, &myparams) == -1)
      ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p control\n"),
              _commchannel.c_str()), -1);

  // Asociate the ACE_Asynch_Read_Stream with the TTY serial device
  if (this->_serial_read_stream.open (*this, _serial_dev.get_handle()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("ACE_Asynch_Read_Stream::open")), -1);

  // Idem for ACE_Asynch_Write_Stream
  if (this->_serial_write_stream.open (*this, _serial_dev.get_handle()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("%p\n"),
                ACE_TEXT("ACE_Asynch_Write_Stream::open")), -1);

  ACE_DEBUG ((LM_NOTICE, 
          ACE_TEXT("SerialHandler::open: streams and Asynch Operations opened sucessfully\n")));
/*{{{*/
  // Start an asynchronous read stream
  ////if (this->initiate_read_stream () == -1)
  ////  return -1;
  
  /* Sending a ? to the serial {{{*/
  /*
  char message[]="?\r";
  ACE_Message_Block message_block(2);

  if (message_block.copy(message, 2)) {
        ACE_ERROR ((LM_ERROR,
                    ACE_TEXT ("Error copiando mensaje\n")));
  }

  //ssize_t bytes_written = _serial_dev.send_n ("hola\n", 5);
  if (this->_serial_write_stream.write (message_block, 2 ) == -1)
  {
      ACE_ERROR ((LM_ERROR, "%p\n", "Error escribiendo conyo"));
      return 0;
  }*/
  /*}}}*/
  // Acceptor configuration
  /*}}}*/
  
  //Start the keyboad reading thread
  activate();
  return 0;
}

int SerialHandler::initialize(const char * commchannel, ACE_TTY_IO::Serial_Params params) 
{
  ACE_TRACE("SerialHandler::initialize");

  read_configuration_file("DGS_MCU_Driver");

  // Initialize serial port
  _serialConnector.connect(_serial_dev, ACE_DEV_Addr(commchannel), 0,
      ACE_Addr::sap_any, 0, O_RDWR|FILE_FLAG_OVERLAPPED); 

  
  // Set TTY_IO parameter into the ACE_TTY_IO device(_serial_dev)
  if (_serial_dev.control (ACE_TTY_IO::SETPARAMS, &params) == -1)
      ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p control\n"),
              commchannel), -1);

  // Asociate the ACE_Asynch_Read_Stream with the TTY serial device
  if (this->_serial_read_stream.open (*this, _serial_dev.get_handle()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("ACE_Asynch_Read_Stream::open")), -1);

  // Idem for ACE_Asynch_Write_Stream
  if (this->_serial_write_stream.open (*this, _serial_dev.get_handle()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("%p\n"),
                ACE_TEXT("ACE_Asynch_Write_Stream::open")), -1);

  ACE_DEBUG ((LM_NOTICE, 
          ACE_TEXT("SerialHandler::open: streams and Asynch Operations opened sucessfully\n")));
/*{{{*/
  // Start an asynchronous read stream
  ////if (this->initiate_read_stream () == -1)
  ////  return -1;
  
  /* Sending a ? to the serial {{{*/
  /*
  char message[]="?\r";
  ACE_Message_Block message_block(2);

  if (message_block.copy(message, 2)) {
        ACE_ERROR ((LM_ERROR,
                    ACE_TEXT ("Error copiando mensaje\n")));
  }

  //ssize_t bytes_written = _serial_dev.send_n ("hola\n", 5);
  if (this->_serial_write_stream.write (message_block, 2 ) == -1)
  {
      ACE_ERROR ((LM_ERROR, "%p\n", "Error escribiendo conyo"));
      return 0;
  }*/
  /*}}}*/
  // Acceptor configuration
  /*}}}*/
  
  //Start the keyboad reading thread
  activate();
  return 0;
}

/** 
 *  SerialHandler::svc 
 * @return 
 * @todo end correctly the svc loop.
 */
int SerialHandler::svc()
{
    ACE_TRACE(ACE_TEXT("SerialHandler::svc"));

    for (;;)
    {
        ACE_Message_Block * serial_block;
        int result = getq(serial_block);

        if (result == -1)
        {
            ACE_DEBUG((LM_NOTICE, 
                    ACE_TEXT("%l SerialHandler::svc Problem Reading block from Queue\n")));
        } 
        else 
        {
            ACE_DEBUG((LM_NOTICE, 
                    ACE_TEXT("%l SerialHandler::svc Procesing Message_block\n")));
            // Write in the serial device.
            if (this->_serial_write_stream.write (*serial_block,
                    serial_block->length()) == -1)
            {
                ACE_ERROR ((LM_ERROR, 
                        "%l \n", "Error writing in svc SerialHandler"));
                return 0;
            }
            //serial_block->release();
            //DGSTask * command_sender = ACE_reinterpret_cast(DGSTask *,
            //serial_block->cont()->rd_ptr());
            SerialFeedbackData * feedback_data =
                ACE_reinterpret_cast(SerialFeedbackData *,
                    serial_block->cont()->rd_ptr());
            ACE_Task<ACE_MT_SYNCH> * command_sender = feedback_data->getCommandSender();
            //command_sender->IsAlive();
        }
    }
    ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l SerialHandler::svc Exiting\n")));
    return 0;
}

/** 
 *  SerialHandler::initiate_read_stream
 * @return 
 */
int
SerialHandler::initiate_read_stream (ACE_Message_Block * response_data_block)
{
  ACE_TRACE("SerialHandler::initiate_read_stream");

  // Create Message_Block
  ACE_Message_Block *mb = 0;
  ACE_NEW_RETURN (mb, ACE_Message_Block (BUFSIZ + 1), -1);

  //glue both the mb and the response_data_block that contains all the
  //necessary data to detect the response condition and the pointer to the 
  //feedback Task
  mb->cont(response_data_block);

  // Inititiate an asynchronous read from the stream
  if (this->_serial_read_stream.read (*mb, mb->size () - 1) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("ACE_Asynch_Read_Stream::read")), -1);

  ACE_DEBUG ((LM_NOTICE, 
          ACE_TEXT("SerialHandler:initiate_read_stream: Asynch Read stream issued sucessfully\n")));
  return 0;
}

/** 
 *  SerialHandler::handle_read_stream
 * @param result 
 */
void
SerialHandler::handle_read_stream (const ACE_Asynch_Read_Stream::Result &result)
{
    ACE_TRACE("SerialHandler::handle_read_stream");

    ACE_High_Res_Timer timer;
    timer.start ();
  /*{{{*/
  ACE_DEBUG ((LM_NOTICE, "********************\n"));
  //ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_to_read", result.bytes_to_read ()));
  //ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle ()));
  ACE_DEBUG ((LM_NOTICE, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
  //ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "act", (u_long) result.act ()));
  ACE_DEBUG ((LM_NOTICE, "%s = %d\n", "success", result.success ()));
  //ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "completion_key", (u_long) result.completion_key ()));
  //ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error ()));
  ACE_DEBUG ((LM_NOTICE, "********************\n"));
  /*}}}*/
  // Watch out if you need to enable this... the ACE_Log_Record::MAXLOGMSGLEN/*{{{*/
  // value controls to max length of a log record, and a large output
  // buffer may smash it.
//#if 0
  //ACE_DEBUG ((LM_INFO, "%s",
              //result.message_block ().rd_ptr ()));/*}}}*/
              /*{{{
  ////ACE_OS::printf("%s", result.message_block().rd_ptr());
  // Start an asynchronous read stream
  ////this->initiate_read_stream ();
  }}}*/
  // Go into the processing of the result in case we have read something.
  if ( result.bytes_transferred() != 0)
  {
      ACE_ASSERT( result.message_block().length() <= BUFSIZ);
      ACE_ASSERT( result.bytes_transferred() <= BUFSIZ );
      ACE_ASSERT( result.success() == 1);

      //Get the feedbackdata block from the message_block.
      SerialFeedbackData * feedback_data =
          ACE_reinterpret_cast(SerialFeedbackData *,
              result.message_block().cont()->rd_ptr());

      // Check if the  delimiter condition is meet for this message.
      unsigned int delimiter_pos = 0;
      int response = feedback_data->checkSerialResponseEnd(delimiter_pos,
          result.message_block().rd_ptr(), result.message_block().length());

      // Get the sender of the command.
      ACE_Task<ACE_MT_SYNCH> * command_sender = feedback_data->getCommandSender();
      ACE_ASSERT( command_sender != NULL);
      size_t space = result.message_block().space();

      // Proceed depending on which is the status of the response.
      switch(response){
          case 0: // We have not finish. Continue reading form the serial device.
              ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT( "%l handle_read_stream continuing reading\n")));
              ACE_ASSERT( space > 0 );
              if (this->_serial_read_stream.read (result.message_block(), space-1) == -1)
                  ACE_ERROR ((LM_ERROR, ACE_TEXT("%p\n"),
                          ACE_TEXT("ACE_Asynch_Read_Stream::read")));
              break;
          case 1: // He have got a termination condition.
              // If the delimiter is exactly at the end of the message
              if ( (result.message_block().length() == (delimiter_pos + 1))
                  || (delimiter_pos == 0))
              {
                  ACE_DEBUG ((LM_WARNING,ACE_TEXT( "%l handle_read_stream termination condition reached \n")));
                  ACE_Message_Block * response_block = result.message_block().cont();
                  result.message_block().cont(NULL);
                  command_sender->putq(&result.message_block());

                  if (feedback_data->getStreamingMode())
                      initiate_read_stream (response_block);
              }
              else //We have found a delimiter but it is immerse in the middle of the message.
              {
                  // Task: divide the message. The first part till the nul is
                  // sent to the receiver. The other part is keep and used to
                  // accumulate the rest of the command from the serial device.
                  ACE_DEBUG ((LM_WARNING, 
                          ACE_TEXT ("%l handle_read_stream termination condition in the middle of a message\n")));

                  // Create Message_Block, copy the valid part of the
                  // received block and send it the command sender
                  ACE_Message_Block *tmp_message_block = 0;
                  ACE_NEW_NORETURN (tmp_message_block, ACE_Message_Block (delimiter_pos + 1));
                  ACE_ASSERT( tmp_message_block != NULL);
                  if (tmp_message_block->copy(result.message_block().rd_ptr(), delimiter_pos + 1 )) 
                      ACE_ERROR ((LM_ERROR,"%p%l", ACE_TEXT ("%I%N%l Error coping user message block\n")));
                  command_sender->putq(tmp_message_block);

                  // Crunch the rest of the message and send it back to the
                  // serial dev to read again.
                  result.message_block().rd_ptr(delimiter_pos+1);
                  result.message_block().crunch();
                  space = result.message_block().space();
                  ACE_ASSERT( space >= 1);
                  ACE_ASSERT( space != 1);
                  if (this->_serial_read_stream.read (result.message_block(), space-1) == -1)
                      ACE_ERROR ((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("ACE_Asynch_Read_Stream::read")));
              }
              break;
          case 2: // There is not termination condition. Just send back what we have and continue reading.
              ACE_DEBUG ((LM_DEBUG,ACE_TEXT( "%l handle_read_stream no termination condition. Continuing reading\n")));
              ACE_Message_Block * response_block = result.message_block().cont();
              result.message_block().cont(NULL);
              command_sender->putq(&result.message_block());
              initiate_read_stream (response_block);
              break;
      }
  }
  timer.stop ();

  ACE_Time_Value measured;
  timer.elapsed_time (measured);
  ACE_DEBUG((LM_INFO, ACE_TEXT("Microseconds: %d\n"),measured.usec()));
}

/** 
 *  SerialHandler::handle_write_stream
 * @param result 
 */
void SerialHandler::handle_write_stream (const ACE_Asynch_Write_Stream::Result &result)
{
    ACE_TRACE("SerialHandler::handle_write_stream");
    ACE_DEBUG ((LM_INFO, ACE_TEXT("handle_write_stream called\n")));

    // Reset pointers/*{{{*/
    ////result.message_block ().rd_ptr (result.message_block ().rd_ptr () - result.bytes_transferred ());

    ////result.message_block ().rd_ptr ()[result.bytes_transferred ()] = '\0';

    ////ACE_DEBUG ((LM_DEBUG, "********************\n"));/*}}}*/
    /*{{{*/
    ACE_DEBUG ((LM_INFO, "%s = %d\n", "bytes_to_write", result.bytes_to_write ()));
    ///ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle ()));
    ACE_DEBUG ((LM_INFO, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
    ACE_DEBUG ((LM_INFO, ACE_TEXT("Getting the command sender pointer\n")));
    /*}}}*/

    ACE_Message_Block * response_data_block = result.message_block().cont();

    result.message_block().cont(NULL);
    result.message_block().release();

    this->initiate_read_stream (response_data_block);
    ////ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "act", (u_long) result.act ()));/*{{{*/
    ////ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "success", result.success ()));
    ////ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "completion_key", (u_long) result.completion_key ()));
    ////ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error ()));
    ////ACE_DEBUG ((LM_DEBUG, "********************\n"));
    // Watch out if you need to enable this... the ACE_Log_Record::MAXLOGMSGLEN
    // value controls to max length of a log record, and a large output
    // buffer may smash it.
#if 0
    ACE_DEBUG ((LM_DEBUG, "%s = %s\n",
            "message_block",
            result.message_block ().rd_ptr ()));
#endif  /* 0 *//*}}}*/
}


/** 
 * SerialHandler::read_configuration_file
 * 
 * @param filename 
 * 
 * @return 
 */
int SerialHandler::read_configuration_file(char * filename)
{
  /// Getting the configuration information from the conf file
  // Let build the file name
  ACE_TCHAR config_file [MAXPATHLEN];
  ACE_String_Base<char> conffilename(filename);
  conffilename += ACE_TEXT(".conf");
  ACE_OS_String::strcpy (config_file, conffilename.c_str());

  // Let create the configuration heap and import the conf file
  ACE_Configuration_Heap config;
  config.open();
  ACE_Registry_ImpExp config_importer(config);
  if (config_importer.import_config(config_file) == -1)
      ACE_ERROR_RETURN( (LM_ERROR, ACE_TEXT("%p\n"), config_file), -1);

  // Let start reading the conf file by opening a section
  ACE_Configuration_Section_Key status_section;
  if (config.open_section (config.root_section(), ACE_TEXT("CommParameters"),
          0, status_section) == -1)
      ACE_ERROR_RETURN (( LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("Can't open CommParameters section")), -1);

  // Let read the udp connection port from the conf file
  if (config.get_string_value(status_section, ACE_TEXT("ListenPort"), _udpport) == -1)
      ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ListenPort does ") ACE_TEXT("not exist\n")),-1);
  ACE_OS::printf("This is the _udpport: %s\n", _udpport.c_str());

  // Let read the serial communication port
  if (config.get_string_value(status_section, ACE_TEXT("CommChannel"), _commchannel) == -1)
      ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("CommChannel does ") ACE_TEXT("not exist\n")),-1);
  ACE_OS::printf("This is the comm port: %s\n", _commchannel.c_str());
  ///@todo Make the client port open with the conf file reading
  u_short client_port = ACE_DEFAULT_SERVICE_PORT; /* This seems to be 20003*/

  // Let read the serial communication port
  ACE_TString baudrate;
  if (config.get_string_value(status_section, ACE_TEXT("BaudRate"), baudrate) == -1)
      ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("BaudRate does ") ACE_TEXT("not exist\n")),-1);
  ACE_OS::printf("This is the baudrate: %s\n", baudrate.c_str());
  _baudrate = ACE_OS::atoi(baudrate.c_str());

  return 0;
}

