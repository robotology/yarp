//============================================================================
//
//  File Name:     types.h
//  Description:   Needed for Unix versions
//  Created:       03/01/2002
//  Author:        Yury Altshuler
//
//  Copyright:     InterSense 2002 - All rights Reserved.
//
//
//=============================================================================
#ifndef _ISENSE_INC_types_h
#define _ISENSE_INC_types_h

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

typedef BOOL                Bool;
typedef HWND                Hwnd;

#else

typedef unsigned char       BYTE;
typedef unsigned long       DWORD;  
typedef long                LONG;
typedef long                Bool;
typedef unsigned short      WORD;
typedef long                Hwnd;

#endif
#endif



