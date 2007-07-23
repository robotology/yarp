/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file SerialFeedbackData.cpp
 * @brief 
 * @version 1.0
 * @date 26-Jul-06 12:07:39 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: SerialFeedbackData.cpp,v 1.1 2007-07-23 13:17:53 alex_bernardino Exp $
 */

#include "SerialFeedbackData.h"

/** 
 * SerialFeedbackData::setSerialResponseDelimiter
 */
int 
SerialFeedbackData::setSerialResponseDelimiter(char * delimiter)
{
    //ACE_NEW_RETURN(_delimiter_string, ACE_String_Base<char>(delimiter),-1);
    _use_string_delimiter      = true;
    _use_bytesnumber_delimiter = false;
    _use_nul_delimiter         = false;
    return 0;
}

/** 
 * SerialFeedbackData::setSerialResponseDelimiter
 */
void
SerialFeedbackData::setSerialResponseDelimiter(int bytes_to_read)
{
    _bytes_to_read = bytes_to_read;
    _use_bytesnumber_delimiter = true;
    _use_string_delimiter      = false;
    _use_nul_delimiter         = false;
}

/** 
 * SerialFeedbackData::setSerialResponseDelimiterNul
 */
void SerialFeedbackData::setSerialResponseDelimiterNul()
{
    _use_nul_delimiter         = true;
    _use_bytesnumber_delimiter = false;
    _use_string_delimiter      = false;
}

/** 
 * SerialFeedbackData::checkSerialResponseTermination
 */
int SerialFeedbackData::checkSerialResponseEnd(unsigned int &delimiter_position, const
    char * serial_response, const int response_size)
{
    if ( _use_string_delimiter || response_size == 0 )
    {
        /*
        ACE_String_Base<char> _serial_response_string(serial_response, response_size) ;
        ssize_t result = _serial_response_string.find (*_delimiter_string);

        delimiter_position = 0;

        if (result == ACE_String_Base_Const::npos )
            return 0;
        else
            return 1;
            */
        //if (result) return 1;
        //else return 0;
    }
    else if ( _use_bytesnumber_delimiter )
    {
        if (response_size >= _bytes_to_read)
        {
            delimiter_position = _bytes_to_read - 1;
            return 1;
        }
        else return 0;
    }
    else if ( _use_nul_delimiter )
    {
        if ( serial_response [response_size - 1] == '\0')
        {
            delimiter_position = response_size - 1;
            return 1;
        }
        else
        {
            for ( int i = 0; i < response_size; i += 1 )
                if (serial_response[i] == '\0')
                {
                    delimiter_position = i;
                    return 1;
                }
        }
        return 0;
    }

    return 2; //No termination condition has been activated
}

