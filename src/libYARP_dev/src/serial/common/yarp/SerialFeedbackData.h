/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file SerialFeedbackData.h
 * @brief Implementation of SerialFeedbackData class
 * @version 1.0
 * @date 26-Jul-06 12:16:12 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: SerialFeedbackData.h,v 1.1 2007-05-30 10:26:45 alex_bernardino Exp $
 */

#ifndef _SERIALFEEDBACKDATAH_ 
#define _SERIALFEEDBACKDATAH_

#include <ace/OS.h>
#include <ace/String_Base.h>
#include <ace/Task.h>

/** 
 * @class SerialFeedbackData
 * @brief Contains data used by the Serial Handler to detect the end of a serial
 * command and send the response back to the command sender.
 */
class SerialFeedbackData
{

public:

    SerialFeedbackData ()
    {
        // Initialize local parameters.
        _delimiter_string = 0;
        _command_sender   = 0;
        _bytes_to_read    = 0;
        _counted_bytes    = 0;
        _use_string_delimiter      = false;
        _use_bytesnumber_delimiter = false;
        _use_nul_delimiter = false;
        _streaming = false;
    };  /* constructor */

    ~SerialFeedbackData()
    {
        if ( _delimiter_string != 0) free(_delimiter_string);
    };

    /** 
     * setCommandSender 
     * 
     * @param command_sender The pointer to the Task that has sent the serial
     * command.  
     */
    void setCommandSender( ACE_Task<ACE_MT_SYNCH> * command_sender){ _command_sender = command_sender; };
    bool getStreamingMode(){ return _streaming;};
    void setStreamingMode(bool streaming){ _streaming = streaming;};

    /** 
     * getCommandSender
     * 
     * @return 
     */
    ACE_Task<ACE_MT_SYNCH> * getCommandSender(){ return _command_sender;};

    /** 
     * setSerialResponseDelimiter
     * 
     * @param delimiter It is a string that should contain a caractheristic
     * reponse of the serial device indicating the end of a response. E.g. The
     * Barrett Hand issues a "=>" when it is expecting a new command. This
     * special string can be used to detect the end of a command response. This
     * response finalization method inhibits the use of a byte count to detect
     * the end of the response. 
     *
     * @return -1 Problem in memory reservation.
     * @return 0 All ok
     */
     int setSerialResponseDelimiter(char * delimiter);

    /** 
     * Sets the numbers of bytes that are expected in the response.
     * 
     * @param number_of_bytes It the number of bytes that the Command senders
     * expect to read from the delivered command. The activation of this
     * modality deactivates the use of a string to detect the end of a response
     * command.
     */
    void setSerialResponseDelimiter(int bytes_to_read); 

    /** 
     * setSerialResponseDelimiterNul Sets the NUL character '\0' as the serial
     * response delimiter.
     */
    void setSerialResponseDelimiterNul();

    /** 
     * checkSerialResponseEnd Checks is the serial response has finish.
     *
     * @param delimiter_position Where, in the response string the delimiter was
     * @param serial_reponse A pointer to the readed data.
     * @param response_size The number of bytes readed.
     * found.
     * @return 1 if a end condition has been detected.
     * @return 2 if no end condition has been activated.
     * @return 0 if the end condition has not been meet.
     */
    int checkSerialResponseEnd(
        unsigned int &delimiter_position,
        const char * serial_response, 
        const int number_of_readed_bytes = 0 );
protected:

private:
    ACE_Task<ACE_MT_SYNCH> * _command_sender;      /// Pointer to the DGSTask that has sent the command.
    ACE_String_Base<char> * _delimiter_string;   /// The delimiter string that detects the end of a serial response.
    int _bytes_to_read;             /// The number of bytes that are expected to be read for a response.
    bool _use_string_delimiter;      /// Determines whether or not use the string
                                    /// delimiter to detect the end of a response
    bool _use_bytesnumber_delimiter; /// Determines whether or not use a count of
                                    /// readed bytes to control the reponse limit.
    bool _use_nul_delimiter;
    int _counted_bytes; /// A counter that acculates the number of bytes readed so far.
    bool _streaming;
}; 


#endif

