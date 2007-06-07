/*
	extras.h
*/

/* Copyright(c) 1991-1996 by Motion Engineering, Inc.  All rights reserved.
 *
 * This software  contains proprietary and  confidential information  of
 * Motion Engineering Inc., and its suppliers.  Except as may be set forth
 * in the license agreement under which  this software is supplied, use,
 * disclosure, or  reproduction is prohibited without the prior express 
 * written consent of Motion Engineering, Inc.
 */

#ifndef __EXTRAS_H
#	define __EXTRAS_H

#ifdef __cplusplus
extern "C" {
#	endif

int16 FNTYPE get_tuner_data(int16 axis, int16 data_length, long PTRTYPE * apos,  long PTRTYPE * cpos, P_INT time, P_INT state,
	P_INT voltage) ;
int16 FNTYPE get_coord_data(int16 axis, int16 data_length, long PTRTYPE * x_p,  long PTRTYPE * y_p, P_INT time) ;

#ifdef __cplusplus
	} ;
#	endif

#endif /* __EXTRAS_H */

