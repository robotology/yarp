//=============================================================================
// Copyright © 2003 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with PGR.
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
#ifndef __PGRERROR_H__
#define __PGRERROR_H__

//=============================================================================
//
// PGRError.h - This file defines a standard error reporting mechanism for
//  all PGR errors
//
// NOTE: In order to use this, you must add the following lines of code to
//  each of the files that you are using the macros with!
//
// #define PGR_PROJECT_NAME "Project Name"
// #define PGR_FILE_NAME    "$RCSfile: pgrerror.h,v $"
// #define PGR_FILE_VERSION "$Revision: 1.1 $"
//
//=============================================================================


//=============================================================================
// Includes
//=============================================================================
#include <time.h>

#define MAX_MESSAGES      4  // messages confirming things are okay.
#define MED_MESSAGES      3
#define MIN_MESSAGES      2
#define CRITICAL_MESSAGES 1
#define NO_MESSAGES       0  // no messages at all

//
// This sets stuff up so that ship debug builds actually display messages.
//
#ifdef _DEBUG
#define MESSAGE_LEVEL     MAX_MESSAGES
#else
#ifdef SHIP_DEBUG
#define MESSAGE_LEVEL     MAX_MESSAGES       // maximum error messages
#else
#define MESSAGE_LEVEL     CRITICAL_MESSAGES  // only critical error messages.
#endif
#endif


#ifndef PGR_ERROR_DISPLAY_TEXT
#define PGR_ERROR_DISPLAY_TEXT 0
#endif

#ifndef PGR_ERROR_DISPLAY_MSGBOX
#define PGR_ERROR_DISPLAY_MSGBOX 1
#endif


#ifdef WIN32
// use message box only if compiled for windows

#pragma warning( disable : 4127 ) // conditional expression is constant


//
// Name: PGR_ERROR_MESSAGE4
//
// Description: 
//  This macro displays an error message on the screen given four parameters -
//  the message header, the error message, its number and the level at which 
//  it should be displayed.
//
// Arguments:
//  MESSAGE_HEADER - the message header.
//  ERROR_MESSAGE  - the error message to be displayed.
//  ERROR_NUMBER   - the error's number.
//  ERROR_LEVEL    - this is the debug level at which the error should be displayed.
//
#define PGR_ERROR_MESSAGE4( MESSAGE_HEADER, ERROR_MESSAGE, ERROR_NUMBER, ERROR_LEVEL )\
{ \
   char _szFileName_[50]; \
   char _szVersion_[20]; \
   char _szLastPGRErrorMessage_[256]; \
   time_t ltime; \
   time( &ltime ); \
   sscanf( PGR_FILE_NAME+1,"RCSfile: %s,v $", _szFileName_ ); \
   sscanf( PGR_FILE_VERSION+11,"%s $", _szVersion_ ); \
   if ( ERROR_NUMBER == 0 ) \
   { \
      sprintf(_szLastPGRErrorMessage_,"%s v%s (line %d)\nBuild Time: %s %s\nCurrent Time: %s\n\nError: %s", \
      _szFileName_, _szVersion_,__LINE__,__DATE__,__TIME__, ctime( &ltime ), ERROR_MESSAGE ); \
   }\
   else \
   { \
      sprintf(_szLastPGRErrorMessage_,"%s v%s (line %d)\nBuild Time: %s %s\nCurrent Time: %s\n\nError(0x%x): %s", \
      _szFileName_, _szVersion_,__LINE__,__DATE__,__TIME__, ctime( &ltime), ERROR_NUMBER, ERROR_MESSAGE ); \
   } \
   if ( MESSAGE_LEVEL >= ERROR_LEVEL && PGR_ERROR_DISPLAY_MSGBOX > 0 ) \
   { \
      ::MessageBox( NULL, _szLastPGRErrorMessage_, MESSAGE_HEADER, MB_ICONWARNING ); \
   }\
   if ( MESSAGE_LEVEL >= ERROR_LEVEL && PGR_ERROR_DISPLAY_TEXT > 0 ) \
   { \
      printf( _szLastPGRErrorMessage_ ); \
   }\
}

#else // #ifdef WIN32

//
// Linux version
//
#define PGR_ERROR_MESSAGE4( MESSAGE_HEADER, ERROR_MESSAGE, ERROR_NUMBER, ERROR_LEVEL )\
{ \
   char _szFileName_[50]; \
   char _szVersion_[20]; \
   char _szLastPGRErrorMessage_[256]; \
   sscanf( PGR_FILE_NAME+1, "RCSfile: %s,v $", _szFileName_ ); \
   sscanf( PGR_FILE_VERSION+11, "%s $", _szVersion_ ); \
   \
   if ( ERROR_NUMBER == 0 ) \
   { \
      sprintf( _szLastPGRErrorMessage_,"%s v%s (line %d) Error: %s\n", \
      _szFileName_, _szVersion_, __LINE__, ERROR_MESSAGE ); \
   }\
   else \
   { \
      sprintf( _szLastPGRErrorMessage_,"%s v%s (line %d) Error(0x%x): %s\n", \
      _szFileName_, _szVersion_,__LINE__, ERROR_NUMBER, ERROR_MESSAGE ); \
   } \
   if ( MESSAGE_LEVEL >= ERROR_LEVEL ) \
   { \
      fprintf( stderr, _szLastPGRErrorMessage_ ); \
   }\
}

#endif // #ifdef WIN32

    
//
// Name: PGR_ERROR_MESSAGE3  
//
// Description: 
//  This macro displays an error message on the screen given three parameters -
//  the error message, its number and the level at which it should be displayed.
//
// Arguments:
//  ERROR_MESSAGE - the error message to be displayed.
//  ERROR_NUMBER  - the error's number.
//  ERROR_LEVEL   - this is the debug level at which the error should be displayed.
//

#define PGR_ERROR_MESSAGE3( ERROR_MESSAGE, ERROR_NUMBER, ERROR_LEVEL ) \
   PGR_ERROR_MESSAGE4( PGR_PROJECT_NAME, ERROR_MESSAGE, ERROR_NUMBER, ERROR_LEVEL )


//
// Name: PGR_ERROR_MESSAGE2
//
// Description: 
//  This macro displays an error message on the screen given two parameters - 
//  the error message and the level at which it should be displayed.
//
// Arguments:
//  ERROR_MESSAGE - the error message to be displayed.
//  ERROR_LEVEL   - this is the debug level at which the error should be displayed.
//
#define PGR_ERROR_MESSAGE2( ERROR_MESSAGE, ERROR_LEVEL ) \
   PGR_ERROR_MESSAGE3( ERROR_MESSAGE, 0, ERROR_LEVEL )


//
// Name: PGR_ERROR_MESSAGE
//
// Description: 
//  This macro displays an error message on the screen given a single parameter.
//
// Arguments:
//  ERROR_MESSAGE - the error message to be displayed.
//
#define PGR_ERROR_MESSAGE( ERROR_MESSAGE ) \
   PGR_ERROR_MESSAGE2( ERROR_MESSAGE, MAX_MESSAGES )


#ifdef WIN32
//
// Name: PGR_ERROR_SYSTEM_MESSAGE
//
// Description: 
//  This macro displays a win32 system error message
//
// Arguments:
//  ERROR_NUMBER - the win32 error number as returned by GetLastError();
//
#define PGR_ERROR_SYSTEM_MESSAGE( ERROR_NUMBER, ERROR_LEVEL ) \
{  \
   LPVOID lpMsgBuf;  \
\
   FormatMessage(    \
    FORMAT_MESSAGE_ALLOCATE_BUFFER |   \
    FORMAT_MESSAGE_FROM_SYSTEM |       \
    FORMAT_MESSAGE_IGNORE_INSERTS,     \
    NULL,			       \
    ERROR_NUMBER,		       \
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
    (LPTSTR) &lpMsgBuf,		       \
    0,				       \
    NULL );			       \
\
   PGR_ERROR_MESSAGE4( "Win32 System Error", (LPCTSTR)lpMsgBuf, ERROR_NUMBER, ERROR_LEVEL ); \
\
   LocalFree( lpMsgBuf );  \
\
}     

#else // #ifdef WIN32

//
// PGR_ERROR_SYSTEM_MESSAGE() does nothing under Linux.
//
#define PGR_ERROR_SYSTEM_MESSAGE( ERROR_NUMBER, ERROR_LEVEL ) 

#endif  // #ifdef WIN32

#endif // #ifndef _PGRERROR_H__
